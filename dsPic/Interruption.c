/*
 * File:   Interruption.c
 * Author: aurel
 *
 * Created on 24 mars 2020, 14:56
 */
#define FCY 1842500

#include <xc.h>
#include <libpic30.h>

//Fonctions globales
void alim_phase(int angle);
//Variables globales
extern int vitesse;

void init_interruption(void)
{
    INTCON1bits.NSTDIS = 0;     //Pas d'imbrication des interruption
    
    //Interruption SPI
    IFS0bits.SPI1IF=0;          //Flag a 0
    IPC2bits.SPI1IP=0b111;      //Priorite Max (7)
    IEC0bits.SPI1IE=1;          //Activation de l'interruption
    
    //Interruption Timer 1
    IFS0bits.T1IF = 0;          //Flag a 0
    //IEC0bits.T1IE = 1;
}

/*void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void)
{
    IFS0bits.SPI1IF = 0;
    vitesse = SPI1BUF;
    //vitesse = (vitesse + 1) %255;
    LATBbits.LATB0 ^= 1;
}*/

void __attribute__ ((interrupt(auto_psv))) _SPI1Interrupt(void){
	_LATB0^=1;
	IFS0bits.SPI1IF=0;
	int datain= SPI1BUF;
	vitesse=datain;
}	