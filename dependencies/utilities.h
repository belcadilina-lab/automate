#include "structs.h"
#include <stdbool.h>

void initAutomate(Automate* a1);
void recopieEtats(Automate *A, int Etats[], int taille);
void recopieEtatsInitiale(Automate *A, int Etats[], int taille);
void recopieEtatsFinaux(Automate *A, int Etats[], int taille);
void recopieTransition(Automate *A, Transition trans[], int taille);
bool rechercherAlphabet(Automate *protocol, char etatChar);
bool rechercherEtatFinale(Automate *protocol, int etatChar);
bool transitionExiste(Transition *tab, int nbr, int dep, int arriv, char lettre);
bool rechercherEtat(Automate *protocol, int etatChar);
void FusionneEtatsInitialsFinals(Automate *A);
void fromAtoEps(Automate* result,Automate* a1);
void fromEpsToA(Automate* result,Automate* a1);
void changeState(Automate* auto1, Automate* auto2);
void copy(Automate* auto1, Automate* result);