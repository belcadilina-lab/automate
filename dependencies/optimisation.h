#include "structs.h"
void trois(Automate* a);
void determiniserAutomate(Automate *nonD, Automate *D);
bool contientEtatFinal(EnsembleEtats *ens, Automate *AFN);
void ajouterEtat_EnsSansRep(EnsembleEtats *ens, int etat);
Automate minimiserMoore(Automate *A);
void genererMotsRec(Automate *A, int etat, char *mot_courant, int pos, FILE *f);
void ecrireMotsAutomateMinimal(Automate *A, const char *nom_fichier);
void produitAutomates(Automate *A1, Automate *A2, Automate *C);
void supprimerEtatsMorts(Automate *A);