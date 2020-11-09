/*
 * File:   main.c
 * Author: aurelien
 *
 * Created on 9 mars 2020, 14:18
 */

// CONFIG1
#pragma config FOSC = INTOSC       // Oscillator Selection Bits (ECH, External Clock, High Power Mode (4-20 MHz): device clock supplied to CLKIN pins)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = ON       // Brown-out Reset Enable (Brown-out Reset enabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = ON        // Internal/External Switch Over (Internal External Switch Over mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PPS1WAY = ON     // Peripheral Pin Select one-way control (The PPSLOCK bit cannot be cleared once it is set by software)
#pragma config ZCD = OFF        // Zero Cross Detect Disable Bit (ZCD disable.  ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PLLEN = ON       // PLL Enable Bit (4x PLL is always enabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = ON         // Low-Voltage Programming Enable (Low-voltage programming enabled)

// CONFIG3
#pragma config WDTCPS = WDTCPS1F// WDT Period Select (Software Control (WDTPS))
#pragma config WDTE = OFF        // Watchdog Timer Enable (WDT enabled)
#pragma config WDTCWS = WDTCWSSW// WDT Window Select (Software WDT window size control (WDTWS bits))
#pragma config WDTCCS = SWC     // WDT Input Clock Selector (Software control, controlled by WDTCS bits)

//Frequence d'oscillation
#define _XTAL_FREQ 4000000

//Bibliotheques
#include <xc.h>

//Fichiers secondaires
#include "PWM.h"
#include "Interruption.h"
#include "Bluetooth.h"
#include "SPI.h"



//variable global
unsigned char vitesse_consigne = 0;
unsigned char vitesse_actuelle = 0;
unsigned char direction = 128;
unsigned char Niv_Bat = 100;
int Etat = 0;

void init_Pin(void);
void init_Osc(void);
void init_ADC(void);
void arret(void);
void niveau_batterie(void);

void main(void) {
    //Initialisation
    init_Pin();
    init_Osc();
    init_PWM();
    init_Bluetooth();
    init_interruption();
    init_SPI();
    init_ADC();
    
    //Boucle infinie
    while(1)
    {
        if(Etat)                        //Etat en marche
        {
            LATAbits.LATA2 = 1;         //LED signalant l'etat de marche
            
            PWM_angle(direction);       //MAJ de la direction
        }else{
            vitesse_consigne = 0;
        }
        
        if (vitesse_consigne > vitesse_actuelle)
        {
            vitesse_actuelle++;
        }else if (vitesse_consigne < vitesse_actuelle)
        {
            vitesse_actuelle--;
        }
        SPI_write(1, vitesse_actuelle);      //Transmission de la vitesse voulu au dsPIC
        niveau_batterie();              //MAJ du niveau de la batterie
        __delay_ms(10);
    }
    return;
}

void init_Pin(void){
    LATA = 0b00000000;
    PORTA = 0b00000000;
    TRISA = 0b00110000;
    ANSELA = 0b00000000;
    WPUA = 0b00000000;
    
    LATB = 0b00000000;
    PORTB = 0b00000000;
    TRISB = 0b00110000;
    ANSELB = 0b00000000;
    WPUB = 0b00000000;
    
    LATC = 0b00000000;
    PORTC = 0b00000000;
    TRISC = 0b00000000;
    ANSELC = 0b00000000;
    WPUC = 0b00000000;
    return;
}


void init_Osc(void){
    //Oscillateur a 4 MHz
    OSCCONbits.IRCF = 0b1101;
}

void init_ADC(void)
{
    ADCON0bits.CHS = 0b00011;           //AN3 -> RA4
    ADCON0bits.ADON = 1;                //ADC ON
    ADCON1bits.ADFM = 1;                //Resultat align� a droite
    ADCON1bits.ADCS = 0b100;            //Fosc/4
    ADCON1bits.ADPREF = 0b00;           //Reference � Vcc
}

void arret(void)            //Procedure d'arret
{
    Etat = 0;
    LATAbits.LATA2 = 0;
    
    vitesse_consigne = 0;
    direction = 128;
    PWM_angle(direction);
}

void niveau_batterie(void)  //Aquisition du niveau de la batterie + envoie au telephone
{
    ADCON0bits.ADGO = 1;                        //Lecture du niveau de la batterie
    while (ADCON0bits.ADGO) {}                  //Attente de la lecture
    char data [6] = {'*', 'B', '0', '0', '0', '*'};
    if (Niv_Bat >=100)
    {
        data[2] = '1';
    }else{
        data[2] = '0';
        data[3] = Niv_Bat/10 +48;               //Bidouille de conversion d'un entier en caract�re ASCII
        data[4] = Niv_Bat%10 +48;
    }
    Bluetooth_write(6 , data);                  //Envoie au smartphone
}
