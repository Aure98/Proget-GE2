/*
 * File:   PWM.c
 * Author: aurelien
 *
 * Created on 23 mars 2020, 15:10
 */

#include <xc.h>

#define FCY 1842500
#define FPWM 20000

extern int vitesse;
extern int secteur;

void init_PWM(void)
{
    PTCONbits.PTMOD = 0;        //Mode freerun
    PTCONbits.PTCKPS = 0b00;    //Prescaller a 1
    PTMRbits.PTDIR = 0;         //Dirrection croissante
    
    PTMR = 0;                   //Initialisation du compteur a 0
    PTPER = FCY/(FPWM) -1;      //=91 si Fpwm = 20kHz
    
    PWMCON1bits.PMOD1 = 1;      //L et H independant
    PWMCON1bits.PEN1H = 1;      //Activation de H
    PWMCON1bits.PEN1L = 1;      //Activation de L
    PDC1 = PTPER/1024*512;      //Initialisation du DC a 50%
    
    PWMCON1bits.PMOD2 = 1;      //Idem 1
    PWMCON1bits.PEN2H = 1;
    PWMCON1bits.PEN2L = 1;
    PDC2 = PTPER/1024*512;
    
    PWMCON1bits.PMOD3 = 1;      //Idem 1
    PWMCON1bits.PEN3H = 1;
    PWMCON1bits.PEN3L = 1;
    PDC3 = PTPER/1024*512;
    
    OVDCON = 0;                 //Initialisation de la commande de chacun des pins (rien en sortie)
            
    PTCONbits.PTEN = 1;         //Activation du module
}

void alim(void)
{
    //Activation des sorties de PWM en fonction du secteur
    switch (secteur) 
    {
        case 0:
            OVDCON = 0b0000001000000100; //secteur 0 1H et 2L
            //OVDCON = 0b0000000000000001;
            break;
        case 1:
            OVDCON = 0b0000001000010000; //secteur 1 1H et 3L
            //OVDCON = 0b0000000000000010;
            break;
        case 2:
            OVDCON = 0b0000100000010000; //secteur 2 2H et 3L
            //OVDCON = 0b0000000000000100;
            break;
        case 3:
            OVDCON = 0b0000100000000001; //secteur 3 2H et 1L
            //OVDCON = 0b0000000000001000;
            break;
        case 4:
            OVDCON = 0b0010000000000001; //secteur 4 3H et 1L
            //OVDCON = 0b0000000000010000;
            break;
        case 5:
            OVDCON = 0b0010000000000100; //secteur 5 3H et 2L
            //OVDCON = 0b0000000000100000;
            break;
    }
}