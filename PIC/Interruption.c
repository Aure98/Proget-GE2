/*
 * File:   Interruption.c
 * Author: aurel
 *
 * Created on 9 mars 2020, 22:42
 */

#include <xc.h>
#include "PWM.h"
#include "Bluetooth.h"
#include "SPI.h"

//Constante
#define Tension_min_Bat 550
#define Tension_max_Bat 685

//Variables globales et fonction
extern int Etat;
extern unsigned char Niv_Bat;
void arret(void);

int rg_data_B_recu = 10;
char Data_B_recu [10] = {'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A'};

void init_interruption(void)
{
    //Desactivation des interruptions
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 1;    //Activation des interruption des peripheriques
    
    //Liaison UART
    PIE1bits.RCIE = 1;      //Activation de l'interruption Receive
    PIR1bits.RCIF = 0;      //Flag a 0
    
    //Liaison SPI
    PIE1bits.SSP1IE = 0;    //Activation de l'interruption a la reception
    PIR1bits.SSP1IF = 0;    //Flag a 0;
    
    //ADC Batterie
    PIE1bits.ADIE = 1;                  //Activation de l'interruption ADC
    PIR1bits.ADIF = 0;                  //Flag a 0
    
    //Activation des interruptions
    INTCONbits.GIE = 1;
    
}

void __interrupt() Interuption(void){
    INTCONbits.GIE = 0;         //Desactivation des interruptions
    
    //Reception Bluetooth
    if (PIR1bits.RCIF)
    {
        if (RC1REG == 'H'){             //Si on recoit un H on allume
            Etat = 1;
        }else if (RC1REG == 'h'){       //Si on recoit un h on arret
            arret();
        }else if (RC1REG == 'P'){       //Debut des intructions de position
            rg_data_B_recu = 0;
            
            //SPI_write(1, RC1REG);
        }else if (RC1REG == 'p' && rg_data_B_recu != 10){       //Fin des instructions de position
            //SPI_write(1, RC1REG);
            Data_B_recu[rg_data_B_recu] = RC1REG;
            rg_data_B_recu = 10;
            analyse_data_B_recu(Data_B_recu);   //Analyse des donnees de position recu
        }else if (rg_data_B_recu <10){  //Stockage des 10 caracteres suivant le debut
            //SPI_write(1, RC1REG);
            Data_B_recu[rg_data_B_recu] = RC1REG;
            rg_data_B_recu++;
        }
        PIR1bits.RCIF = 0;              //Reset du flag
    }
    
    //ADC
    if (PIR1bits.ADIF)
    {
        PIR1bits.ADIF = 0;                  //Reset du flag
        int res = ADRESH *256 + ADRESL;     //Lecture du resultat
        
        if (res > Tension_max_Bat)          //Calcul du "pourcentage" de charge
        {
            Niv_Bat = 100;
        }else{
            Niv_Bat = (res-Tension_min_Bat)*100/(Tension_max_Bat-Tension_min_Bat);
        }
        
        if (res < Tension_min_Bat)          //Arret si tension trop basse
        {
            arret();
        }
    }
    
    INTCONbits.GIE = 1;         //Activation des interruptions
}
