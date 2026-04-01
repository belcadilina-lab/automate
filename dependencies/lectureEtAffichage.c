#include "structs.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "verification.h"
void readDot(Automate *protocol,char *fichier){
	int i=0,inic,finc,src,dest;
    char val,ligne[100], buff[20];
	initAutomate(protocol);
	FILE* f = fopen(fichier,"r");
	if(f == NULL){
		printf("Fichier introuvable, veuillez vous assurez de l'emplacement du fichier.");
        return;
	}
	while(fgets(ligne, sizeof(ligne), f)){
	    if(sscanf(ligne," %d -> %d [label=\"%c\"];",&src,&dest,&val) == 3){
		//stockage des transitions
			protocol->transitions[i].etat_dep = src;
			protocol->transitions[i].etat_arriv = dest;
			protocol->transitions[i].lettre[0] = val;
			i++;
		//stockage des etats
		    if(protocol->nbr_etat < 20){
			    if(rechercherEtat(protocol, src) == false){
					protocol->etats[protocol->nbr_etat] = src;
					protocol->nbr_etat++;
			    }
			    if(rechercherEtat(protocol, dest) == false){
					protocol->etats[protocol->nbr_etat] = dest;
					protocol->nbr_etat++;
			    }
            }
		//stockage des alphabets
		    if(protocol->nbr_alph < 8){
			    if(rechercherAlphabet(protocol, val) == false && val != 'E'){
					protocol->Alphabet[protocol->nbr_alph] = val;
					protocol->nbr_alph++;
			    }
            }
	    }
    //stockage des etats initiaux et finaux
	    else if(sscanf(ligne," init -> %d;", &src) == 1){
			protocol->etat_initiaux[protocol->inic] = src;
			protocol->inic++;
	    }
        else if(sscanf(ligne," %d -> %[^;];", &src,buff) == 2){
		    if (strcmp(buff,"fin") != 0)
                continue ;
		    protocol->etat_finaux[protocol->finc] = src;
		    protocol->finc++;
	    }
	    else continue;
	}
	protocol->nbr_trans = i;
	fclose(f);
}

void automateShow(Automate protocol){
	int i;
	printf("Voici la liste des etats :\n");
	for( i = 0; i< protocol.nbr_etat ; i++){
		printf("[%d] ",protocol.etats[i]);
	}
	printf("\nVoici la liste d'alphabet : \n");
	for(i = 0; i< protocol.nbr_alph ; i++){
		printf("[%c] ", protocol.Alphabet[i]);
	}
	printf("\nVoici la liste des transitions :\n");
	for( i = 0; i< protocol.nbr_trans ; i++){
		printf("[%d] -> %c -> [%d]\n",protocol.transitions[i].etat_dep,protocol.transitions[i].lettre[0],protocol.transitions[i].etat_arriv);
	}
	printf("\nVoici la liste des etats initiaux :\n");
	for( i = 0; i< protocol.inic ; i++){
		printf("[%d] ",protocol.etat_initiaux[i]);
	}
	printf("\nVoici la liste des etats finaux :\n");
	for( i = 0; i< protocol.finc ; i++){
		printf("[%d] ",protocol.etat_finaux[i]);
	}
    printf("\n");
}
void sauvgarder(Automate a,const char* str){
    int i;
    FILE *f = fopen(str, "w");
    checkFile(f);
    fprintf(f, "digraph G {\n");
    fprintf(f, "init [shape=point shape=point style=filled, fillcolor=white color=white];\n");
    fprintf(f, "fin [shape=point shape=point style=filled, fillcolor=white color=white];\n");
    for( i = 0; i < a.inic; i++){
         fprintf(f, "init -> %d;\n", a.etat_initiaux[i]);
    }
    for(i = 0; i < a.nbr_trans; i++){
        fprintf(f, "%d -> %d [label=\"%c\"];\n",a.transitions[i].etat_dep,a.transitions[i].etat_arriv,a.transitions[i].lettre[0]);
    }
    for(i = 0; i < a.finc; i++){
        fprintf(f, "%d -> fin;\n", a.etat_finaux[i]);
    }
    fprintf(f, "}\n");
    fclose(f);
    printf("Fichier 'automate_utilisateur.dot' genere avec succes.\n");
}
