#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dependencies/structs.h"
#include "dependencies/verification.h"
#include "dependencies/lectureEtAffichage.h"
#include "dependencies/utilities.h"
#include "dependencies/manipulation.h"
#include "dependencies/operations.h"
#include "dependencies/optimisation.h"
int menu(void){
		int choice;
        printf("\n-----------AUTOMATE--------\n");
        printf("1. Lire l'automate depuis graph.dot .  \n2. Afficher les informations de l'automate.  \n");
        printf("3. Generer un fichier.dot .  \n4. Afficher l'etat avec le plus grand nombre des transitions.  \n");
        printf("5. Afficher les etat avec transition etiquete .  \n6. Tester un mot  \n7. extraire Mots accepter ds un fichier.\n");
        printf("8. supprimer les epsilons dans l'automate.\n");
        printf("9. union de 2 automates.\n");
        printf("10. Generer un automate a partir d'une expression reguliere.\n");
        printf("11. Concatenation de 2 automates.\n");
        printf("12. Generer une expression reguliere a partir d'un automate.\n");
        printf("13. determinisation de l'automate\n");
        printf("14. Generer les trois fichiers (automate initial, deterministe, minimaliste).\n");
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
                break; 
            }
            case 2 : {
                automateShow(a);
                break;}
            case 3 :{
                sauvgarder(a,"automate_utilisateur.dot");
                break;
                }
            case 4 : {
                MaxTransitions(&a);
                break;}
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
                if(testerMot(&a,mot))
                {
                    printf("Le mot est accepte.\n");
                }
                else {
                    printf("Le mot n'est pas accepte.\n");
                }
                break;
            }
            case 7 : {
                SaveAcceptedWords(&a);
                break;
            }
            case 8 : {
                supprimerEpsilons(&a);
                supprimerEtatsInaccessibles(&a);
                automateShow(a);
                break;
            }
            case 9:{
                /* if(choix==1)
                {

                    UnionFichiers("src/automate1.dot","src/automate1.dot");
                } ******/
               /*
                    else if(choix == 2)
                {
                */


                Automate* a4 = malloc(sizeof(Automate));
                Automate* a3 = malloc(sizeof(Automate)) ;
                Automate* resultant = malloc(sizeof(Automate));
                initAutomate(resultant);
                initAutomate(a3);
                initAutomate(a4);              
                readDot(a4,"src/automate1.dot");
                readDot(a3,"src/automate2.dot");
                UnionStructure(a3,a4,resultant);
                a = *resultant;
            //}
                
                break;
            }
            
            case 10: {
                char regex[100]; 
                printf("Veuillez entrer votre expression reguliere (sans espaces) : ");
                scanf("%s", regex); 
                a.nbr_etat = 0; 
                a.nbr_trans = 0; 
                a.nbr_alph = 0; 
                a.inic = 0; 
                a.finc = 0;
                printf("\nConstruction de l'automate pour l'expression : %s\n", regex);
                construireAutomateThompson(regex, &a);
                automateShow(a);
                break;
            }
            case 11: {
                Automate a1, a2;
                readDot(&a1, "src/automate1.dot");
                readDot(&a2, "src/automate2.dot");
                if(a1.nbr_etat == 0 || a2.nbr_etat == 0){
                    printf("Erreur : un des automates est vide ou mal charge !\n");
                    break;
                }
                a = concatAutomates(&a1,&a2,&a);
                automateShow(a);
                break;
            }
            case 12:{
                if(a.nbr_etat == 0){
                    printf("Erreur : l'automate est vide ou mal charge !\n");
                    break;
                }
                char regex[200] = "";
                genererRegexDepuisAutomate(&a, regex);
                printf("voici l'expression reguliere du l'automate:\n");
                printf("%s\n\n", regex);
                break;
            }
            case 13:{
                Automate D;
                printf("Automate determinise :\n");
                determiniserAutomate(&a,&D);
                automateShow(D);
                a = D;
                break;
            }
            case 14:{
                Automate M;
                initAutomate(&M);
                M = minimiserMoore(&a);
                printf("Automate minimaliste :\n");
                automateShow(M);
                a = M;
                break;
            }
            case 15:{
            Automate* aInit = malloc(sizeof(Automate));
            initAutomate(aInit);
            readDot(aInit,"demo.dot");
            trois(aInit);
            break;
            }
            case 16:{
                printf("Minimisation de l'automate...\n");
                const char *nomFichier = "mots_reconnus.txt";
                ecrireMotsAutomateMinimal(&a, nomFichier);
                break;
            }
            case 0 : printf("Fin du programme\n"); break;
            default : printf("Entrer un choix valide s'il vous plait!\n");
        }
	}while(output !=0);
	return 0;
}