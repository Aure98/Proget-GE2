/*
 * File:   Gestion_moteur.c
 * Author: aurel
 *
 * Created on 21 avril 2020, 17:38
 */

#define FCY 1842500

#include <xc.h>
#include "PWM.h"

extern int vitesse;
extern int secteur;

void Mode_pas_pas(void)
{
    T1CONbits.TON = 1;                  //Activation du Timer 1
    T2CONbits.TON = 0;                  //Desactivation du Timer 2
    IEC0bits.T2IE = 0;                  //Desactivation de l'interruption du Timer 2
    
    //Desactivation de la detection de Va Vb Vc
    IEC0bits.IC1IE = 0;
    IEC0bits.IC2IE = 0;
    IEC1bits.IC7IE = 0;
    
    if (vitesse == 0)
    {
        OVDCON = 0;
    }else{
        //Actualisation en debut de tour de la periode et du DC
        PR1 = 13035/vitesse;
        int DC = PTPER*vitesse/255;
        PDC1 = DC;
        PDC2 = DC;
        PDC3 = DC;

        
        for (secteur=0; secteur<6; secteur++)   //Balayage des secteurs de 0 a 5
        {
            while(!IFS0bits.T1IF){}         //Tempo timer1
            IFS0bits.T1IF = 0;
            alim();                  //MaJ du secteur dans lequel on se trouve
        }
        secteur = 5;
    }
}

void Mode_autopilote(void)
{
    T1CONbits.TON = 0;                  //Desactivation du Timer 1
    T2CONbits.TON = 1;                  //Activation du Timer 2
    IEC0bits.T2IE = 1;                  //Activation de l'interruption du Timer 2
    
    //Actualisation en debut de tour de la periode et du DC
    int retard = 20;                        //Retard du au filtage (en degres)
    PR2 = FCY/1059*retard/vitesse;         //Cf Rapport Page 49
    int DC = PTPER*vitesse/255;
    PDC1 = DC;
    PDC2 = DC;
    PDC3 = DC;
    
    //Activation de la detection de Va Vb Vc
    IEC0bits.IC1IE = 1;
    IEC0bits.IC2IE = 1;
    IEC1bits.IC7IE = 1;
    
}

void __attribute__((interrupt, no_auto_psv )) _IC1Interrupt(void)
{
    if ((PORTDbits.RD0 == 0) && (secteur == 2))              //Front descendant
    {
        secteur = 3;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
        
    }else if ((PORTDbits.RD0 == 1) && (secteur == 5))        //Front Montant
    {
        secteur = 0;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
    }
    IFS0bits.IC1IF = 0;         //on abaisse le flag
}

void __attribute__((interrupt, no_auto_psv )) _IC2Interrupt(void)
{
    if ((PORTDbits.RD1 == 0) && (secteur == 4))              //Front descendant
    {
        secteur = 5;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
        
    }else if ((PORTDbits.RD1 == 1) && (secteur == 1))        //Front Montant
    {
        secteur = 2;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
    }
    IFS0bits.IC2IF = 0;         //on abaisse le flag
}

void __attribute__((interrupt, no_auto_psv )) _IC7Interrupt(void)
{
    if ((PORTBbits.RB4 == 0) && (secteur == 0))              //Front descendant
    {
        secteur = 1;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
        
    }else if ((PORTBbits.RB4 == 1) && (secteur == 3))        //Front Montant
    {
        secteur = 4;            //Passage au secteur suivant
        TMR2 = 0;               //quand le retard sera combler
    }
    IFS1bits.IC7IF = 0;         //on abaisse le flag
}

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;          //Flag a 0
    alim();                     //Mise a jour de OVDCON
}



////////////  Fonction d'initialisation  //////////

void init_timer(void)
{
    //Initialisation Timer 1 pour le Mode Pas a Pas
    T1CONbits.TCS = 0;                  //Selection de l'hologe
    T1CONbits.TGATE = 0;
    
    T1CONbits.TCKPS = 0b01;             //Predivision par 8 (=0b01)
    PR1 = 13035;                         //Periode Max
    
    T1CONbits.TSYNC = 1;                //Synchronisation sur l'horloge
    T1CONbits.TON = 0;                  //Desactivation
    
    IFS0bits.T1IF = 0;                  //Flag a 0
    IEC0bits.T1IE = 0;                  //Desactivation de l'interruption
    
    //Initialisation Timer 2 pour le Mode Autopilote
    T2CONbits.TCS = 0;                  //Selection de l'hologe
    T2CONbits.TGATE = 0;
    
    T2CONbits.TCKPS = 0b00;             //Predivision par 8 (=0b01)
    PR2 = 10000;                        //Periode Max
    
    T2CONbits.T32 = 0;                  //Timer 2 et Timer 3 independants
    T2CONbits.TON = 0;                  //Desactivation
    
    IFS0bits.T2IF = 0;                  //Flag a 0
    IEC0bits.T2IE = 0;                  //Desactivation de l'interruption
}

void Init_IC1(void)                 //Initialisation de la detection du passage a 0 de Va
{
    IEC0bits.IC1IE = 0;             //Desactiver l'interruption IC1
    IFS0bits.IC1IF = 0;             //flag a 0
    IPC0bits.IC1IP = 7;             //priorité = 7
    IC1CONbits.ICM = 0b001;         //Detection des front montant et descendant
}

void Init_IC2(void)                 //Initialisation de la detection du passage a 0 de Vb
{
    IEC0bits.IC2IE = 0;             //Desactiver l'interruption IC2
    IFS0bits.IC2IF = 0;             //flag a 0
    IPC1bits.IC2IP = 7;             //priorité = 7
    IC2CONbits.ICM = 0b001;         //Detection des front montant et descendant
}

void Init_IC7(void)                 //Initialisation de la detection du passage a 0 de Vc
{
    IEC1bits.IC7IE = 0;             //Desactiver l'interruption IC7
    IFS1bits.IC7IF = 0;             //flag a 0
    IPC4bits.IC7IP = 7;             //priorité = 7
    IC7CONbits.ICM = 0b001;         //Detection des front montant et descendant
}

