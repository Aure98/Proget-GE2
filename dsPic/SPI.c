/*
 * File:   SPI.c
 * Author: aurel
 *
 * Created on 24 mars 2020, 14:55
 */


#include <xc.h>
extern int vitesse;

void init_SPI(void)
{
    SPI1CONbits.MODE16=0;       //Donnees sur 8 bits
    SPI1CONbits.CKP=1;          //Polarite clock inverser
    SPI1CONbits.CKE=0;          //Actif sur front montant
    SPI1CONbits.SSEN=0;         //Pas d'utilisation de SS
    SPI1CONbits.MSTEN=0;        //Mode esclave
    SPI1CONbits.SMP=0;          //A 0 en mode esclave
    SPI1STATbits.SPIROV=0;      //Pas d'overflow
    
    SPI1STATbits.SPIEN=1;       //Activation du SPI
    
    //Interruption SPI
    IFS0bits.SPI1IF=0;          //Flag a 0
    IPC2bits.SPI1IP=0b101;      //Priorite 5
    IEC0bits.SPI1IE=1;          //Activation de l'interruption
}


void __attribute__ ((interrupt(auto_psv))) _SPI1Interrupt(void){
	IFS0bits.SPI1IF=0;
	vitesse= SPI1BUF;
	
}