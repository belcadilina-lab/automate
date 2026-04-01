#include "structs.h"
#include <stdbool.h>
void MaxTransitions(Automate *protocol);
void afficherEtatsAvecTransition(Automate *protocol, char lettre);
bool testerMot(Automate* autom, char * str);
void SaveAcceptedWords(Automate *A);