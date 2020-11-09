//Entete du fichier Gestion_moteur.c

//Fonctions
void Mode_pas_pas(void);
void Mode_autopilote(void);

void __attribute__((interrupt, no_auto_psv )) _IC1Interrupt(void);
void __attribute__((interrupt, no_auto_psv )) _IC2Interrupt(void);
void __attribute__((interrupt, no_auto_psv )) _IC7Interrupt(void);
void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt(void);

void init_timer(void);
void Init_IC1(void);
void Init_IC2(void);
void Init_IC7(void);

