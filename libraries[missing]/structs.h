#include<stdio.h>
#include<stdlib.h>

typedef struct Transition{
    char etat_dep;
    char etat_arriv;
    char lettre;
}Transition;
typedef struct Automate{
    char etats[10];
    Transition transitions[20];
    char Alphabet[5];
    char etat_initial[10];
    char etat_final[10];
}Automate;

