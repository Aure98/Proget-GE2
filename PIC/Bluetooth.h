//Entete du fichier Interruption.c

//Variables globales
//extern int vitesse;
//extern int direction;

//Fonctions
void init_Bluetooth(void);
void Bluetooth_write(const unsigned int taille , char Data_B[5]);
void analyse_data_B_recu(char Data_B_recu[10]);
