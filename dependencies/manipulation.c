#include "structs.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "verification.h"
#include "operations.h"
#include "utilities.h"
void MaxTransitions(Automate *protocol){

    int sorties[20] = {0};
    int entrees[20] = {0};
    int i,j;

    // compter les transitions POUR CHAQUE ETAT
    for(j = 0; j < protocol->nbr_trans; j++){

        for(i = 0; i < protocol->nbr_etat; i++){

            if(protocol->transitions[j].etat_dep == protocol->etats[i])
                sorties[i]++;

            if(protocol->transitions[j].etat_arriv == protocol->etats[i])
                entrees[i]++;
        }
    }

    int maxSort = 0, maxEntr = 0;

    for(i = 0; i < protocol->nbr_etat; i++){
        if(sorties[i] > maxSort)
            maxSort = sorties[i];

        if(entrees[i] > maxEntr)
            maxEntr = entrees[i];
    }

    printf("Etats avec max transitions sortantes (%d): ", maxSort);
    for(i = 0; i < protocol->nbr_etat; i++){
        if(sorties[i] == maxSort)
            printf("%d ", protocol->etats[i]);
    }

    printf("\nEtats avec max transitions entrantes (%d): ", maxEntr);
    for(i = 0; i < protocol->nbr_etat; i++){
        if(entrees[i] == maxEntr)
            printf("%d ", protocol->etats[i]);
    }

    printf("\n");
}
void afficherEtatsAvecTransition(Automate *protocol, char lettre) {

    printf("Transitions etiquetees par '%c' :\n", lettre);
    bool found  =false;
    for (int i = 0; i < protocol->nbr_etat; i++) {

        bool aTransition = false;

        for (int j = 0; j < protocol->nbr_trans; j++) {

            if (protocol->transitions[j].etat_dep == protocol->etats[i] &&
                protocol->transitions[j].lettre[0] == lettre) {

                if (!aTransition) {
                    printf("Etat [%d] -> ", protocol->etats[i]);
                    aTransition = true;
                    found = true;
                }
                printf("[%d] ", protocol->transitions[j].etat_arriv);
            }
        }
        if (aTransition) {
            printf("\n");
        }
    }
    if (!found) {
            printf("n'existe pas\n");
        }
}
bool testerMot(Automate* autom, char * str){
    supprimerEpsilons(autom);
    supprimerEtatsInaccessibles(autom);
    int courant[20], suivant[20];
    int tailleCourant = 0, tailleSuivant = 0;

    for(int k = 0; k<autom->inic ; k++)
    {
        courant[tailleCourant] = autom->etat_initiaux[k];
        tailleCourant++;
    }

    for(int i = 0; i< strlen(str); i++)
    {
        tailleSuivant = 0;
        
        for(int s = 0; s< tailleCourant ; s++)
        {
            for(int j = 0; j< autom->nbr_trans; j++)
            {
                if(autom->transitions[j].etat_dep == courant[s] && autom->transitions[j].lettre[0] == str[i])
                {
                    suivant[tailleSuivant] = autom->transitions[j].etat_arriv;
                    tailleSuivant++;
                }
            }
        }
        if(tailleSuivant==0) return false;

        for(int x = 0; x<tailleSuivant; x++) courant[x] = suivant[x];
        tailleCourant = tailleSuivant;
    }
    for(int s= 0; s<tailleCourant ; s++)
    {
        if(rechercherEtatFinale(autom,courant[s])) return true;
    }
    return false;
}
void SaveAcceptedWords(Automate *A){
	char mot[10];
	FILE *ftxt = fopen("fichier.txt","r");
    FILE *fptr = fopen("MotsAccepter.txt","w");
	if (ftxt == NULL || fptr == NULL){
		printf("Fichier introuvable, veuillez vous assurez de l'emplacement du fichier.");
		return;
	}
	while( fscanf(ftxt,"%s",mot) == 1){
		if(testerMot(A,mot) == true){
		 	fprintf(fptr,"%s \n",mot);
		}
	}
    fclose(fptr);
    fclose(ftxt);
}