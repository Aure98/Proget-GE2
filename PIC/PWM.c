/*
 * File:   PWM.c
 * Author: aurel
 *
 * Created on 9 mars 2020, 20:53
 * 
 * declaration de la PWM
 */
#include <xc.h>

int rapport_cyclique;

void init_PWM(void)
{
    //Init TIMER4
    PR4 = 154;                  //Definition de la periode
    
    T4CLKCONbits.CS = 0b0000;   //Frequence de ref = Fosc/4
    
    T4CONbits.ON = 1;           //ON
    T4CONbits.CKPS = 0b111;     //Prescaller
    T4CONbits.OUTPS = 0b0000;   //Postscaller INUTILE
    
    T4HLTbits.PSYNC = 1;        //synchro sur l'horloge
    T4HLTbits.CKPOL = 0;        //Sur front montant
    T4HLTbits.CKSYNC = 1;       //Activation de la synchro sur l'horloge
    T4HLTbits.MODE = 0b00000;   //Mode Free Run (Software gate)
    
    T4RSTbits.RSEL = 0b1101;    //Reset sur la PWM3
    
    //Init PWM
    PPSLOCK = 0;                //Debloque le PPS
    PWM3CONbits.PWM3EN = 1;     //Activation
    PWM3CONbits.PWM3OUT = 1;    //mettre a 1
    PWM3CONbits.PWM3POL = 0;    //Actif a l'etat haut
    
    CCPTMRSbits.P3TSEL = 0b01;  //PWM3 base sur Timer 4
    RC0PPS = 0b00001110;        //PWM3 lie a RC0
    
    PWM3DCH = 0b00000000;       //Rapport cycle (registre bits de poids forts)
    PWM3DCL = 0b00000000;       //Rapport cycle (registre bits de poids faibles)
    
    PPSLOCK = 1;                //Bloque le PPS
	return;
}

void PWM_angle(unsigned int angle) //Entre 0 et 255
{
    int DC = 66 + angle*21/255;
    PWM3DCH = (DC & 0b0000001111111100) >> 2;
    PWM3DCL = (DC & 0b0000000000000011) << 6;
}
