/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

//Entete du fichier Interruption.c

//Fonctions
void init_interruption(void);
void __attribute__((__interrupt__, auto_psv)) _SPI1Interrupt(void);
