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
	for(int i = 0 ; i< protocol->nbr_etat;i++){
		if(protocol->etats[i] == etatChar ){
			return true;
		}
	}
	return false;
}

bool rechercherAlphabet(Automate *protocol, char etatChar){
	for(int i = 0 ; i< protocol->nbr_alph;i++){
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
	FILE* f = fopen(fichier,"r");
	if(f == NULL){
		printf("Fichier introuvable, veuillez vous assurez de l'emplacement du fichier.");
        return;
	}
	while(fgets(ligne, sizeof(ligne), f)){
	    if(sscanf(ligne," %c -> %c [label=\"%c\"];",&src,&dest,&val) == 3){
		//stockage des transitions
			protocol->transitions[i].etat_dep = src;
			protocol->transitions[i].etat_arriv = dest;
			protocol->transitions[i].lettre = val;
			i++;
		//stockage des etats
		    if(protocol->nbr_etat < 10){
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
			    if(rechercherAlphabet(protocol, val) == false){
					protocol->Alphabet[protocol->nbr_alph] = val;
					protocol->nbr_alph++;
			    }
            }
	    }
    //stockage des etats initiaux et finaux
	    else if(sscanf(ligne," init -> %c;", &src) == 1){
			protocol->etat_initiaux[protocol->inic] = src;
			protocol->inic++;
	    }
        else if(sscanf(ligne," %c -> %[^;];", &src,buff) == 2){
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
    printf("\n");
}
void saisirAutomate(Automate *protocol){
    int i,t,ini,fin;
    char src,dest,val;

    protocol->nbr_etat = 0;
    protocol->nbr_trans = 0;
    protocol->nbr_alph = 0;
    protocol->inic = 0;
    protocol->finc = 0;

    printf("Combien de transitions ? ");
    scanf("%d",&t);

    for(i=0;i<t;i++)
    {
        printf("Transition %d (depart arrivee lettre): ",i+1);
        scanf(" %c %c %c",&src,&dest,&val);

        // stocker transition
        protocol->transitions[i].etat_dep = src;
        protocol->transitions[i].etat_arriv = dest;
        protocol->transitions[i].lettre = val;
        protocol->nbr_trans++;

        if(!rechercherEtat(protocol,src)){
            protocol->etats[protocol->nbr_etat++] = src;
        }

        if(!rechercherEtat(protocol,dest)){
            protocol->etats[protocol->nbr_etat++] = dest;
        }

        if(!rechercherAlphabet(protocol,val)){
            protocol->Alphabet[protocol->nbr_alph++] = val;
        }
    }

    printf("Combien d'etats initiaux ? ");
    scanf("%d",&ini);

    for(i=0;i<ini;i++){
        printf("Etat initial %d : ",i+1);
        scanf(" %c",&protocol->etat_initiaux[protocol->inic++]);
    }

    printf("Combien d'etats finaux ? ");
    scanf("%d",&fin);

    for(i=0;i<fin;i++){
        printf("Etat final %d : ",i+1);
        scanf(" %c",&protocol->etat_finaux[protocol->finc++]);
    }
}
void MaxTransitions(Automate *protocol){

    int sorties[10] = {0};
    int entrees[10] = {0};
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
            printf("%c ", protocol->etats[i]);
    }

    printf("\nEtats avec max transitions entrantes (%d): ", maxEntr);
    for(i = 0; i < protocol->nbr_etat; i++){
        if(entrees[i] == maxEntr)
            printf("%c ", protocol->etats[i]);
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
                protocol->transitions[j].lettre == lettre) {

                if (!aTransition) {
                    printf("Etat [%c] -> ", protocol->etats[i]);
                    aTransition = true;
                    found = true;
                }
                printf("[%c] ", protocol->transitions[j].etat_arriv);
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
bool rechercherEtatFinale(Automate *protocol, char etatChar){
	for(int i = 0 ; i< protocol->finc; i++){
		if(protocol->etat_finaux[i] == etatChar ){
			return true;
		}
	}
		return false;

}
bool Exists(Automate* protcol, char * str){
    char act;
    bool isThere;
    for(int k = 0 ; k<protcol->inic ; k++)
    {
	act = protcol->etat_initiaux[k];
	for(int i = 0; i<strlen(str) ; i++)
    {
		isThere = false;
		for(int j = 0; j<protcol->nbr_trans ; j++)
        {
			if(protcol->transitions[j].etat_dep == act && protcol->transitions[j].lettre == str[i])
            {
				act = protcol->transitions[j].etat_arriv;
				isThere = true;
				break;
			}
		}
        if(!isThere) break;	
	}
    if(isThere == false) continue; 
    else{
        if(!rechercherEtatFinale(protcol,act)){
            isThere = false;
            continue;
        }
        else
            break;
    }	
    }
    return isThere;
}
void sauvgarder(Automate a){
    int i;
    FILE *f = fopen("automate_utilisateur.dot", "w");
    if(f == NULL) {
        printf("erreur lors la creation du fichier\n");
        return;
    }
    fprintf(f, "digraph G {\n");
    fprintf(f, "init [shape=point];\n");
    fprintf(f, "fin [shape=point];\n");
    for( i = 0; i < a.inic; i++){
         fprintf(f, "init -> %c;\n", a.etat_initiaux[i]);
    }
    for(i = 0; i < a.nbr_trans; i++){
        fprintf(f, "%c -> %c [label=\"%c\"];\n",a.transitions[i].etat_dep,a.transitions[i].etat_arriv,a.transitions[i].lettre);
    }
    for(i = 0; i < a.finc; i++){
        fprintf(f, "%c -> fin;\n", a.etat_finaux[i]);
    }
    fprintf(f, "}\n");
    fclose(f);
    printf("Fichier 'automate_utilisateur.dot' genere avec succes.\n");
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
		if(Exists(A,mot) == true){
		 	fprintf(fptr,"%s \n",mot);
		}
	}
    fclose(fptr);
    fclose(ftxt);
}
int menu(void){
		int choice;
        printf("\033[1mNB : Dans le cas des données preexistantes, charger le fichier ecrase ceux de la structure.\033[0m");
		printf("\n-----------AUTOMATE--------\n");
        printf("1. Lire l'automate depuis graph.dot .  \n2. Afficher les informations de l'automate.  \n");
        printf("3. Generer un fichier.dot .  \n4. Afficher l'etat avec le plus grand nombre des transitions.  \n");
        printf("5. Afficher les etat avec transition etiquete .  \n6. Tester un mot  \n7. extraire Mots accepter ds un fichier.\n");
        printf("0. Quitter le programme.\nEffectuer un choix: ");
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
                break;}
            case 3 :{
                int x;
                printf("\033[1mSi vous choisissez (1) Vos données de programme seront ecrasés!!!\n \033[0m\n");
                printf("Pour generer depuis une autre automate entrer 1. \nPour generer depuis l'ancienne automate entrer 2.\nvotre choix: ");
                do{
                    scanf("%d",&x);
                    if(x!=1 && x!=2)
                        printf("choix invalide,veuillez ressayez: \n");
                }while(x!=1 && x!=2);
                if( x == 1){
                    saisirAutomate(&a);
                    sauvgarder(a);break;
                }
                else{
                    sauvgarder(a);break;
                }}
            case 4 : {
                MaxTransitions(&a);break;}
            case 5 : {
                char c;
                printf("entrer la lettre : ");
                scanf(" %c",&c);
                afficherEtatsAvecTransition(&a,c);
                break;
            }
            case 6 :{
                char mot[20];
                printf("Entrer le mot a lire: ");
                scanf("%s",mot);
                while(getchar() != '\n');
                if(Exists(&a,mot))
                {
                    printf("Existe\n");
                }
                else {
                    printf("Introuvable\n");
                }
                break;
            }
            case 7 : {
                SaveAcceptedWords(&a);
                break;
            }
            case 0 : printf("Fin du programme\n"); break;
            default : printf("Entrer un choix valide s'il vous plait!\n");
        }
	}while(output !=0);
    free(&a);
	return 0;
}