//Entete du fichier PWM.c

//Variables globales
extern int rapport_cyclique;

//Fonctions
void init_PWM(void);
void PWM_write(unsigned int DC);
void PWM_angle(unsigned int angle);
