#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef struct Transition{
    char etat_dep;
    char etat_arriv;
    char lettre;
}Transition;
typedef struct Automate{
    char etats[10];
	int nbr_etat;
    Transition transitions[20];
	int nbr_trans;
    char Alphabet[8];
	int nbr_alph;
    char etat_initiaux[5];
	int inic;//nbr des etats initial
    char etat_finaux[5];
	int finc;//nbr des etats finaux
}Automate;
bool rechercherEtat(Automate *protocol, char etatChar){
	for(int i = 0 ; i<= protocol->nbr_etat;i++){
		if(protocol->etats[i] == etatChar ){
			return true;
		}
	}
		return false;

}
bool rechercherAlphabet(Automate *protocol, char etatChar){
	for(int i = 0 ; i<= protocol->nbr_alph;i++){
		if(protocol->Alphabet[i] == etatChar ){
			return true;
		}
	}
		return false;
}

void readDot(Automate *protocol,char *fichier){
	int i=0,inic,finc;
	char src,dest,val,ligne[100], buff[20];
	protocol->nbr_etat = 0;
    protocol->nbr_trans = 0;
    protocol->nbr_alph = 0;
	protocol->inic=0;
	protocol->finc=0;
	FILE* f;
	f = fopen(fichier,"r");
	if(f == NULL){
			printf("Fichier introuvable, veuillez vous assurez de l'emplacement du fichier.");
	}
	while(fgets(ligne, sizeof(ligne), f))
	{
	if(sscanf(ligne," %c -> %c [label=\"%c\"];",&src,&dest,&val) == 3){
		//stockage des transitions
				protocol->transitions[i].etat_dep = src;
				protocol->transitions[i].etat_arriv = dest;
				protocol->transitions[i].lettre = val;
				i++;
		//stockage des etats
		if(protocol->nbr_etat < 10){
				if(rechercherEtat(protocol, src) == false)
				{
					protocol->etats[protocol->nbr_etat] = src;
					protocol->nbr_etat++;
				}
				if(rechercherEtat(protocol, dest) == false)
				{
					protocol->etats[protocol->nbr_etat] = dest;
					protocol->nbr_etat++;
				}}
		//stockage des alphabets
		if(protocol->nbr_alph < 8){
				if(rechercherAlphabet(protocol, val) == false)
				{
					protocol->Alphabet[protocol->nbr_alph] = val;
					protocol->nbr_alph++;
				}}
		}//stockage des etats initiaux et finaux
	else if(sscanf(ligne," init -> %c;", &src) == 1)
			{
			 protocol->etat_initiaux[protocol->inic] = src;	
			 protocol->inic++;
			}
	else if(sscanf(ligne," %c -> %[^;];", &src,buff) == 2)
			{
			if (strcmp(buff,"fin") != 0) continue ;
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
		printf("[%c] ",protocol.etats[i]);
	}
	printf("\nVoici la liste d'alphabet : \n");	
	for(i = 0; i< protocol.nbr_alph ; i++){
		printf("[%c] ", protocol.Alphabet[i]);
	}
	printf("\nVoici la liste des transitions :\n");	
	for( i = 0; i< protocol.nbr_trans ; i++){
		printf("[%c] -> %c -> [%c]\n",protocol.transitions[i].etat_dep,protocol.transitions[i].lettre,protocol.transitions[i].etat_arriv);
	}
	printf("\nVoici la liste des etats initiaux :\n");	
	for( i = 0; i< protocol.inic ; i++){
		printf("[%c] ",protocol.etat_initiaux[i]);
	}
	printf("\nVoici la liste des etats finaux :\n");	
	for( i = 0; i< protocol.finc ; i++){
		printf("[%c] ",protocol.etat_finaux[i]);
	}
}
int menu(void){
		int choice;
		printf("-----------AUTOMATE--------\n""1. Lire l'automate depuis graph.dot\n""2. Afficher les informations de l'automate\n""3.Quitter\n""Votre choix: ");
		scanf("%d",&choice);
		return choice;
	}
int main(){
	Automate a;
	int output;
	do{
	output = menu();
	switch(output){
		case 1 : {
			readDot(&a,"demo.dot");
			printf("lecture du fichier avec succes.\n");
			break; }
		case 2 : {
			automateShow(a); 
			printf("\n");
			break;}
		case 3 : printf("Fin du programme\n"); break;
		default : printf("Entrer un choix valide s'il vous plait!\n");
	}
	}while(output !=3);
	return 0;
}