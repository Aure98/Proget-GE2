/*
 * File:   Bluetooth.c
 * Author: aurel
 *
 * Created on 10 mars 2020, 12:35
 */

#define _XTAL_FREQ 4000000

#include <xc.h>
#include "SPI.h"

extern unsigned char direction;
extern unsigned char vitesse_consigne;


void init_Bluetooth(void)
{
    PPSLOCK = 0;                //Debloque le PPS
    //RX -> RB5  par defaut
    //TX -> RB7
    RB7PPS = 0b10010;
    PPSLOCK = 1;                //Bloque le PPS
    
    RC1STAbits.SPEN = 1;        //Reception ON
    RC1STAbits.CREN = 1;        //Reception en continue
    TX1STAbits.TXEN = 1;        //Transmission ON
    TX1STAbits.SYNC = 0;        //Assynchrone
    TX1STAbits.BRGH = 1;        //Haut debit
    
    BAUD1CONbits.RCIDL = 0;     //
    BAUD1CONbits.SCKP = 0;      //Pas d'inversion de la data
    BAUD1CONbits.BRG16 = 1;     //Baud rate sur 16 bits
    
    SP1BRGL = 0x67;             //Baud rate
    SP1BRGH = 0;
}


void Bluetooth_write(const unsigned int taille , char Data_B_envoie[6])
{
    for (int i = 0; i<taille; i++){
        TX1REG = Data_B_envoie[i];      //Envoie d'un octet/caractere
        while(!TX1STAbits.TRMT) {}      //Attente de l'envoie
    }
}

void analyse_data_B_recu(char Data_B_recu[10])      //Analyse des donnees de position recu
{
    int i=0;
    //Extraction de la dirrection
    if(Data_B_recu[i] =='X')
    {
        i++;
        direction = Data_B_recu[i]-48;
        i++;
        if(Data_B_recu[i] !='Y')
        {
            direction = direction*10 + Data_B_recu[i]-48;
            i++;
            if(Data_B_recu[i] !='Y')
            {
                direction = direction*10 + Data_B_recu[i]-48;
                i++;
            }
        }
        
        //Extraction de la vitesse
        i++;
        vitesse_consigne = Data_B_recu[i]-48;
        i++;
        if(Data_B_recu[i] !='p')
        {
            vitesse_consigne = vitesse_consigne*10 + Data_B_recu[i]-48;
            i++;
            if(Data_B_recu[i] !='p')
            {
                vitesse_consigne = vitesse_consigne*10 + Data_B_recu[i]-48;
            }
        }
        vitesse_consigne = 255 - vitesse_consigne;
    }
}