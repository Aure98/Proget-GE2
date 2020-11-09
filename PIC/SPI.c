/*
 * File:   SPI.c
 * Author: aurel
 *
 * Created on 18 mars 2020, 11:05
 */


#include <xc.h>
#define _XTAL_FREQ 4000000

void init_SPI(void)
{
    //PINs
    //SDI -> RB4 (MISO)  Par default
    PPSLOCK = 0;                //Debloque le PPS
    //SCK -> RB6
    RB6PPS = 0b10000;
    //SS  -> RC5
    //SDO -> RC2 (MOSI)
    RC2PPS = 0b10001;           //RC2 relier a SDO
    PPSLOCK = 1;                //Bloque le PPS
    
    //Forme de l'horloge
    SSP1STATbits.CKE = 0;
    SSP1CON1bits.CKP = 1;
    
    SSP1STATbits.SMP = 1;       //Enchantillonage a la fin
    SSP1CON1bits.SSPOV = 0;     //Pas d'overflow
    SSP1CON1bits.SSPM = 0b1010; //Master Mode Clock = Fosc/(4* SSP1ADD+1)
    
    SSP1ADD = 0x09;             //Baud rate 100kHz
    
    SSP1CON1bits.SSPEN = 1;     //Activation
          
    //Deselection de tous les MCs esclaves
    //LATCbits.LATC5 = 1;
    
}

void SPI_write(int SS, char Data_SPI)
{
    SSP1BUF = Data_SPI;             //Envoie des donnees
    while(!PIR1bits.SSP1IF) {}      //Attente de la fin de l'envoie
}