#include "structs.h"
Automate concatAutomates(Automate *A1, Automate *A2,Automate *C);

void UnionStructure(Automate* auto1, Automate* auto2, Automate* result);

void ajouterOuMajTransition(Automate *A, int dep, int arriv, const char *expr);
void supprimerEtat(Automate *A, int etat_supp);

void genererRegexDepuisAutomate(Automate *A, char *regex);
void obtenirExpression(Automate *A, int dep, int arriv, char *resultat);

void initPileFragments(PileFragments *p);
void pushFragment(PileFragments *p, Fragment f);
Fragment popFragment(PileFragments *p);
void initPileChar(PileChar *p);
void pushChar(PileChar *p, char c);
char popChar(PileChar *p);
char topChar(PileChar *p);
bool pileCharVide(PileChar *p);
int nouvelEtat();
void ajouterEtatSiAbsent(Automate *A, int e);
void ajouterAlphabetSiAbsent(Automate *A, char c);
void ajouterTransition(Automate *A, int dep, int arriv, char lettre);
Fragment automateLettre(Automate *A, char lettre);
Fragment concatFragments(Automate *A, Fragment f1, Fragment f2);
Fragment unionFragments(Automate *A, Fragment f1, Fragment f2);
Fragment etoileFragment(Automate *A, Fragment f);
Fragment plusFragment(Automate *A, Fragment f);
void appliquerOperateur(PileFragments *pileF, PileChar *pileC, Automate *A);
void construireAutomateThompson(const char *regex, Automate *A);
void supprimerEpsilons(Automate *A);
void supprimerEtatsInaccessibles(Automate *A);
void calculFermetureEpsilon(Automate *A, int etat, int *fermeture, int *taille);