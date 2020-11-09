/*
 * File:   main.c
 * Author: aurel
 *
 * Created on 23 mars 2020, 14:30
 */

// FOSC
#pragma config FPR = XT                // Primary Oscillator Mode (XTL)
#pragma config FOS = FRC                // Oscillator Source (Internal Fast RC)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = NONE            // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = PGD                // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#define FCY 1842500

//Biblioteque
#include <xc.h>
#include <libpic30.h>

//Fichiers secondaires
#include "PWM.h"
#include "SPI.h"
#include "Gestion_moteur.h"

void init_pin (void);

int vitesse = 0;
int secteur = 0;

int main(void) {
    /////////////////////////////////////////
    //Initialisation
    init_pin();
    init_PWM();
    init_timer();
    init_SPI();
    Init_IC1();
    Init_IC2();
    Init_IC7();
    
    INTCON1bits.NSTDIS = 0;     //Pas d'imbrication des interruptions
    
    //Initialisation Oscillateur
    OSCCONbits.NOSC = 0b01;
    OSCCONbits.OSWEN = 1;
    
    /////////////////////////////////////////
    //Boucle infinie
    while(1)
    {   
        if(vitesse <113)            //3000tr/min = 64
        {
            Mode_pas_pas();
            LATBbits.LATB0 =0;
        }else{
            Mode_autopilote();
            LATBbits.LATB0 =1;
        }
    }
    
    return 0;
}

void init_pin (void)
{
    LATB  = 0;
    PORTB = 0;
    TRISB = 0;
    
    LATE  = 0;
    PORTE = 0;
    TRISE = 0;
    
    LATF  = 0;
    PORTF = 0;
    TRISF = 0;
    
    //SPI
    TRISFbits.TRISF2=1;
    TRISEbits.TRISE8=1;
    
    //Detection Va Vb Vc
    TRISDbits.TRISD0=1;
    TRISDbits.TRISD1=1;
    TRISBbits.TRISB4=1;
    ADPCFG = 0b0000000000010000;        //Permet au pin B4 de fonctionner
}
