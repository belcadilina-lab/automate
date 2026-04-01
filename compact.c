#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dependencies/structs.h"
#include "dependencies/verification.h"
#include "dependencies/lectureEtAffichage.h"
#include "dependencies/utilities.h"
#include "manipulation.h"

//calcul le epsilon-close de chaque etat
void calculFermetureEpsilon(Automate *A, int etat, int *fermeture, int *taille) {
    fermeture[0] = etat; // L'état lui-même fait partie de sa fermeture
    *taille = 1;
    //parcoure A->transitions pour trouver tous les etats accessibles par epsilon 'E'
    for (int i = 0; i < *taille; i++) {
        int etat_courant = fermeture[i];
        for (int j = 0; j < A->nbr_trans; j++) {
            if (A->transitions[j].etat_dep == etat_courant && A->transitions[j].lettre[0] == 'E') {
                int etat_suivant = A->transitions[j].etat_arriv;
                
                //verifier si etat_suivant n'existe pas deja dans le tab du fermeture
                bool exist = false;
                for (int k = 0; k < *taille; k++) {
                    if (fermeture[k] == etat_suivant) {
                        exist = true;
                        break;
                    }
                }
                if (!exist) {
                    fermeture[*taille] = etat_suivant;
                    (*taille)++;
                }
            }
        }
    }
}
//fct permet supprimer les etats inaccessible
void supprimerEtatsInaccessibles(Automate *A) {
    Automate Acc;
    initAutomate(&Acc);
    recopieEtats(&Acc,A->etat_initiaux, A->inic);
    
    bool nouveau_trouve;//trouver tous les etats accessibles
    do {
        nouveau_trouve = false;
        for (int i = 0; i < Acc.nbr_etat; i++) {
            int etat_courant = Acc.etats[i];

            for (int j = 0; j < A->nbr_trans; j++) {
                if (A->transitions[j].etat_dep == etat_courant) {
                    int etat_dest = A->transitions[j].etat_arriv;
                    
                    bool existe = false;//verifier que etat_dest n'est pas deja accessible
                    for (int k = 0; k < A->nbr_etat; k++) {
                        if (Acc.etats[k] == etat_dest) {
                            existe = true;
                            break;
                        }
                    }
                    if (!existe) {
                        Acc.etats[Acc.nbr_etat] = etat_dest;
                        Acc.nbr_etat++;
                        nouveau_trouve = true;
                    }
                }
            }
        }
    } while (nouveau_trouve);

    recopieEtats(A, Acc.etats, Acc.nbr_etat);

    for (int i = 0; i < A->nbr_trans; i++) {
        bool dep_accessible = false;
        for (int k = 0; k < Acc.nbr_etat ; k++) {
            if (A->transitions[i].etat_dep == Acc.etats[k]) { 
                dep_accessible = true;
                break;
            }
        }
        //si l'etat depart est accessible, on garde la transition
        if (dep_accessible) {
            Acc.transitions[Acc.nbr_trans] = A->transitions[i];
            Acc.nbr_trans++;
        }
    }
    recopieTransition(A, Acc.transitions, Acc.nbr_trans);
    for (int i = 0; i < A->finc; i++) {
        bool est_accessible = false;
        for (int k = 0; k < Acc.nbr_etat; k++) {
            if (A->etat_finaux[i] == Acc.etats[k]) {
                est_accessible = true;
                break;
            }
        }
        if (est_accessible) {
            Acc.etat_finaux[Acc.finc] = A->etat_finaux[i];
            Acc.finc++;
        }
    }
    recopieEtatsFinaux(A, Acc.etat_finaux, Acc.finc);
}
// fonction supprime les epsilons dans une automate
void supprimerEpsilons(Automate *A) {
    Transition nouvelles_trans[100]; // Tableau temporaire pour stocker les nouvelles transitions
    int nbr_nouv = 0;//taile tableu nouv

            for (int i = 0; i < A->nbr_etat; i++) {//parcour les etat du l'automate
                int etat_actuel = A->etats[i];
                int fermeture[A->nbr_etat];//epsilon-close de chaque etat
                int taille = 0; //nombre des element dans le tableau fermeture

                calculFermetureEpsilon(A, etat_actuel, fermeture, &taille);
                //tester si l'etat actuel devient final
                bool devient_final = false;
                for (int j = 0; j < taille; j++) {
                    if (rechercherEtatFinale(A, fermeture[j])) {
                        devient_final = true;
                        break;
                    }
                }
                //verifier que l'etat actuel n'est pas deja un etat finale
                if (devient_final && !rechercherEtatFinale(A, etat_actuel) && A->finc < 10) {
                    A->etat_finaux[A->finc] = etat_actuel;
                    A->finc++;
                }
                //chercher les nouvelles transitions a creer
                for (int j = 0; j < taille; j++) {
                    int etat_p = fermeture[j];
                    
                    // Chercher les transitions partant de p qui ne sont PAS des epsilons
                    for (int k = 0; k < A->nbr_trans; k++) {
                        if (A->transitions[k].etat_dep == etat_p && A->transitions[k].lettre[0] != 'E') {
                            if (!transitionExiste(nouvelles_trans, nbr_nouv, etat_actuel, A->transitions[k].etat_arriv, A->transitions[k].lettre[0])) {
                                nouvelles_trans[nbr_nouv]= A->transitions[k];
                                nbr_nouv++;
                            }
                        }
                    }
                }
            }
    //remplacer les ancienne transition par les nouvelles
    A->nbr_trans = 0;
    int i = 0;
    while(i<nbr_nouv && i<50){
        A->transitions[i] = nouvelles_trans[i];
        A->nbr_trans++;
        i++;
    }
    printf("Les transitions epsilon ont ete supprimees avec succes.\nAfficher l'automate pour y verifier.\n");
}
//changement pour travailler sur NFA
void initPileFragments(PileFragments *p) {
    p->sommet = -1; 
}
void pushFragment(PileFragments *p, Fragment f){
    p->tab[++(p->sommet)] = f; 
}
Fragment popFragment(PileFragments *p){
    return p->tab[(p->sommet)--];
}
void initPileChar(PileChar *p){ 
    p->sommet = -1; 
}
void pushChar(PileChar *p, char c){
    p->tab[++(p->sommet)] = c; 
}
char popChar(PileChar *p) { 
    return p->tab[(p->sommet)--]; 
}
char topChar(PileChar *p) { 
    return p->tab[p->sommet]; 
}
bool pileCharVide(PileChar *p) { 
    return p->sommet == -1; 
}
int nouvelEtat(){ 
    static int compteur = 1; 
    return compteur++; 
}
void ajouterEtatSiAbsent(Automate *A, int e) {
    if (!rechercherEtat(A, e) && A->nbr_etat < 20) 
    A->etats[A->nbr_etat++] = e;
}

void ajouterAlphabetSiAbsent(Automate *A, char c) {
    if (c != 'E' && !rechercherAlphabet(A, c) && A->nbr_alph < 10)
        A->Alphabet[A->nbr_alph++] = c;
}

void ajouterTransition(Automate *A, int dep, int arriv, char lettre) {
    if (A->nbr_trans >= 50) 
        return;
    if(transitionExiste(A->transitions, A->nbr_trans, dep, arriv, lettre))
        return;

    A->transitions[A->nbr_trans].etat_dep = dep;
    A->transitions[A->nbr_trans].etat_arriv = arriv;
    A->transitions[A->nbr_trans].lettre[0] = lettre;
    A->nbr_trans++;

    ajouterEtatSiAbsent(A, dep);
    ajouterEtatSiAbsent(A, arriv);
    ajouterAlphabetSiAbsent(A, lettre);
}
//construction des fragement(automates)
Fragment automateLettre(Automate *A, char lettre) {
    int debut = nouvelEtat();
    int fin = nouvelEtat();
    Fragment f = {debut, fin};
    ajouterTransition(A, f.debut, f.fin, lettre);
    return f;
}

Fragment concatFragments(Automate *A, Fragment f1, Fragment f2) {
    ajouterTransition(A, f1.fin, f2.debut, 'E');
    Fragment f = {f1.debut, f2.fin};
    return f;
}

Fragment unionFragments(Automate *A, Fragment f1, Fragment f2) {
    int debut = nouvelEtat();
    int fin = nouvelEtat();
    Fragment f = {debut, fin};
    ajouterTransition(A, f.debut, f1.debut, 'E');
    ajouterTransition(A, f.debut, f2.debut, 'E');
    ajouterTransition(A, f1.fin, f.fin, 'E');
    ajouterTransition(A, f2.fin, f.fin, 'E');
    return f;
}

Fragment etoileFragment(Automate *A, Fragment f) {
    int debut = nouvelEtat();
    int fin = nouvelEtat();
    Fragment nf = {debut, fin};
    ajouterTransition(A, nf.debut, f.debut, 'E'); 
    ajouterTransition(A, nf.debut, nf.fin, 'E');  
    ajouterTransition(A, f.fin, f.debut, 'E');    
    ajouterTransition(A, f.fin, nf.fin, 'E'); 
    return nf;
}

Fragment plusFragment(Automate *A, Fragment f) {
    ajouterTransition(A, f.fin, f.debut, 'E');
    Fragment nf = {f.debut, f.fin};
    return nf;
}

void appliquerOperateur(PileFragments *pileF, PileChar *pileC, Automate *A) {
    char op = popChar(pileC);
    Fragment f2 = popFragment(pileF);
    Fragment f1 = popFragment(pileF);
    
    if (op == '+') pushFragment(pileF, unionFragments(A, f1, f2));
    else if (op == '.') pushFragment(pileF, concatFragments(A, f1, f2));
}

void construireAutomateThompson(const char *regex, Automate *A) {
    PileFragments pileF; 
    initPileFragments(&pileF);
    PileChar pileC; 
    initPileChar(&pileC);

    for (int i = 0; regex[i] != '\0'; i++) {
        char c = regex[i];

        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) { 
            pushFragment(&pileF, automateLettre(A, c));
            if (!pileCharVide(&pileC) && topChar(&pileC) == '.') {
                appliquerOperateur(&pileF, &pileC, A);
            }
        }
        else if (c == '*' || c == '^') {
            Fragment f = popFragment(&pileF);
            pushFragment(&pileF, (c == '*') ? etoileFragment(A, f) : plusFragment(A, f));
        }
        else if (c == '+' || c == '(') {
            pushChar(&pileC, c);
        }
        else if (c == ')') {
            while (!pileCharVide(&pileC) && topChar(&pileC) != '(') {
                appliquerOperateur(&pileF, &pileC, A);
            }
            if (!pileCharVide(&pileC)) popChar(&pileC); // Retire '('
        }

        // 5. Concaténation implicite (ex: "ab" devient "a.b")
        char next = regex[i + 1];
        if (next != '\0' && ( (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '*' || c == '^' || c == ')') && 
                            ((next >= 'a' && next <= 'z') || (next >= '0' && next <= '9') || next == '(')) {
            pushChar(&pileC, '.');
        }
    }

    // Appliquer les opérateurs restants dans la pile
    while (!pileCharVide(&pileC)) {
        appliquerOperateur(&pileF, &pileC, A);
    }

    // Définition de l'état initial et final
    Fragment finalF = popFragment(&pileF);
    A->etat_initiaux[0] = finalF.debut;
    A->inic = 1;
    A->etat_finaux[0] = finalF.fin;
    A->finc = 1;
}
void FusionneEtatsInitialsFinals(Automate *p);
Automate concatAutomates(Automate *A1, Automate *A2,Automate *C) {
    int i, j;
    initAutomate(C);
    for(i = 0; i < A1->nbr_etat; i++) {
        C->etats[C->nbr_etat++] = A1->etats[i];
    }
    for(i = 0; i < A2->nbr_etat; i++) {
        if(!rechercherEtat(C, A2->etats[i])) {
            C->etats[C->nbr_etat++] = A2->etats[i];
        }
    }
    for(i = 0; i < A1->nbr_alph; i++) {
        if(!rechercherAlphabet(C, A1->Alphabet[i]))
            C->Alphabet[C->nbr_alph++] = A1->Alphabet[i];
    }
    for(i = 0; i < A2->nbr_alph; i++) {
        if(!rechercherAlphabet(C, A2->Alphabet[i]))
            C->Alphabet[C->nbr_alph++] = A2->Alphabet[i];
    }
    if(!rechercherAlphabet(C, 'E')) {
        C->Alphabet[C->nbr_alph++] = 'E';
    }

    for(i = 0; i < A1->nbr_trans; i++) {
        C->transitions[C->nbr_trans++] = A1->transitions[i];
    }
    for(i = 0; i < A2->nbr_trans; i++) {
        C->transitions[C->nbr_trans++] = A2->transitions[i];
    }
    for(i = 0; i < A1->finc; i++) {
        for(j = 0; j < A2->inic; j++) {
            C->transitions[C->nbr_trans].etat_dep = A1->etat_finaux[i];
            C->transitions[C->nbr_trans].etat_arriv = A2->etat_initiaux[j];
            C->transitions[C->nbr_trans].lettre[0] = 'E'; 
            C->nbr_trans++;
        }
    }

    for(i = 0; i < A1->inic; i++) {
        C->etat_initiaux[C->inic++] = A1->etat_initiaux[i];
    }

    for(i = 0; i < A2->finc; i++) {
        C->etat_finaux[C->finc++] = A2->etat_finaux[i];
    }
    return *C;
}
//fct permet ajouter ou fusionner des transitions
void ajouterOuMajTransition(Automate *A, int dep, int arriv, const char *expr) {
    if (strlen(expr) == 0) return;//ignore les expressions vide

    //cherche si une transition existe deja
    for (int i = 0; i < A->nbr_trans; i++) {
        if (A->transitions[i].etat_dep == dep && A->transitions[i].etat_arriv == arriv) {
            //fusionne les 2 expressions qui on meme etat depart et meme etat arrive avec l'operateur +
            char temp[200];
            snprintf(temp, 200, "(%s+%s)", A->transitions[i].lettre, expr);//imprime l'expression dans temp sans depasser 200 cases
            strncpy(A->transitions[i].lettre, temp, 200);
            return;
        }
    }
    //sinon, on cree une nouvelle transition
    A->transitions[A->nbr_trans].etat_dep = dep;
    A->transitions[A->nbr_trans].etat_arriv = arriv;
    strncpy(A->transitions[A->nbr_trans].lettre, expr, 200);
    A->nbr_trans++;
}

//fct fusion des etats a un seul etat initial et un seul etat final
void FusionneEtatsInitialsFinals(Automate *A) {
    int initial = 21;
    int final = 22;
    A->etats[A->nbr_etat++] = initial;
    A->etats[A->nbr_etat++] = final;

    //relier l'etat initial aux anciens états initiaux
    for (int i = 0; i < A->inic; i++) {
        ajouterOuMajTransition(A, initial, A->etat_initiaux[i], "E");
    }
    A->etat_initiaux[0] = initial;
    A->inic = 1;

    //relier les anciens états finaux au super-final
    for (int i = 0; i < A->finc; i++) {
        ajouterOuMajTransition(A, A->etat_finaux[i], final, "E");
    }
    A->etat_finaux[0] = final;
    A->finc = 1;
}
//fonction pour effacer un etat et ses transitions liees
void supprimerEtat(Automate *A, int etat_supp) {
    int i = 0;
    while (i < A->nbr_trans) {
        if (A->transitions[i].etat_dep == etat_supp || A->transitions[i].etat_arriv == etat_supp) {
            //on remplace la transition a supprimer par la derniere du tableau
            A->transitions[i] = A->transitions[A->nbr_trans - 1];
            A->nbr_trans--;
        } else {
            i++;
        }
    }
}
//fct permet extraire une expression entre deux etats
void obtenirExpression(Automate *A, int dep, int arriv, char *resultat) {
    strcpy(resultat, ""); //initilaiser par vide
    for (int i = 0; i < A->nbr_trans; i++) {
        if (A->transitions[i].etat_dep == dep && A->transitions[i].etat_arriv == arriv) {
            strcpy(resultat, A->transitions[i].lettre);
            return;
        }
    }
}
void copy(Automate* auto1, Automate* result)
{
    for(int i= 0; i<auto1->inic ; i++)
    {
        result->etat_initiaux[result->inic++] = auto1->etat_initiaux[i] ;
        
    }

    for(int i= 0; i<auto1->finc ; i++)
    {
        result->etat_finaux[result->finc++] = auto1->etat_finaux[i] ;
    }


    for(int i= 0; i<auto1->nbr_trans ; i++)
    {
        result->transitions[result->nbr_trans++] = auto1->transitions[i];
    }
}
void changeState(Automate* auto1, Automate* auto2)
    {
        int current;
    for(int i = 0 ; i<auto1->nbr_etat ; i++)
    {
        current = auto1->etats[i];
        for(int j = 0 ; j<auto2->nbr_etat ; j++)
        {
            if(current == auto2->etats[j])
            {
                auto1->etats[i]-= auto1->nbr_etat;
                 for(int k = 0; k< auto1->nbr_trans ; k++ )
                        {
                            if(auto1->transitions[k].etat_dep == current)
                            auto1->transitions[k].etat_dep = auto1->etats[i];
                            if(auto1->transitions[k].etat_arriv == current)
                            auto1->transitions[k].etat_arriv = auto1->etats[i];
                        }
                 for(int l = 0 ; l<auto1->inic ; l++)

                 {
                     if(auto1->etat_initiaux[l] == current)
                     auto1->etat_initiaux[l] = auto1->etats[i];
                
                }

                for(int l = 0 ; l<auto1->finc ; l++)

                 {
                     if(auto1->etat_finaux[l] == current)
                     auto1->etat_finaux[l] = auto1->etats[i];
                
                     }
            }
        }


    }
}
void fromEpsToA(Automate* result,Automate* a1)
{
    result->etat_initiaux[0] = 100;
        result->inic = 1;
    for(int i = 0; i< a1->inic ; i++)
    {
        result->transitions[result->nbr_trans].etat_dep = 100;
        result->transitions[result->nbr_trans].etat_arriv = a1->etat_initiaux[i];
        strcpy(result->transitions[result->nbr_trans].lettre, "E");
        result->nbr_trans++;
    }

}

void fromAtoEps(Automate* result,Automate* a1)
{
    result->etat_finaux[0] = 200;
    result->finc = 1;

        for(int i = 0; i< a1->finc ; i++)
    {
        result->transitions[result->nbr_trans].etat_dep = a1->etat_finaux[i];
        result->transitions[result->nbr_trans].etat_arriv = 200;
        strcpy(result->transitions[result->nbr_trans].lettre, "E");
        result->nbr_trans++;
    }
}
void UnionStructure(Automate* auto1, Automate* auto2, Automate* result)
{

    initAutomate(result);
    changeState(auto1,auto2);
    copy(auto1,result);
    copy(auto2,result);
    fromEpsToA(result,auto1);
    fromEpsToA(result,auto2);
    fromAtoEps(result,auto1);
    fromAtoEps(result,auto2);    
}

void genererRegexDepuisAutomate(Automate *A, char *regex) {
    
    FusionneEtatsInitialsFinals(A);
    for (int k = 0; k < A->nbr_etat; k++) {
        int act = A->etats[k];
        if (act == A->etat_initiaux[0] || act == A->etat_finaux[0]) continue;

        //recupere la boucle sur lui-même s'il y en a une
        char tmp_boucle[200], boucle[200] = "";
        obtenirExpression(A, act, act, tmp_boucle);
        if (strlen(tmp_boucle) > 0) {
            snprintf(boucle, 200, "(%s)*", tmp_boucle);
        }

        // Trouver tous les chemins entrant (i -> act) puis sortant (act -> j)
        for (int i = 0; i < A->nbr_trans; i++) {
            if (A->transitions[i].etat_arriv == act && A->transitions[i].etat_dep != act) {
                int etat_i = A->transitions[i].etat_dep;
                char ExpR_i_act[200];
                strcpy(ExpR_i_act, A->transitions[i].lettre);

                for (int j = 0; j < A->nbr_trans; j++) {
                    if (A->transitions[j].etat_dep == act && A->transitions[j].etat_arriv != act) {
                        int etat_j = A->transitions[j].etat_arriv;
                        char ExpR_act_j[200];
                        strcpy(ExpR_act_j, A->transitions[j].lettre);

                        // concatener: (Entree) + (Boucle) + (Sortie)
                        char nouveau_chemin[200];
                        snprintf(nouveau_chemin, 200, "(%s)%s(%s)", ExpR_i_act, boucle, ExpR_act_j);
                        
                        // Ajouter le pont direct de i vers j
                        ajouterOuMajTransition(A, etat_i, etat_j, nouveau_chemin);
                    }
                }
            }
        }
        //supprision de l'etat act a la fin du concatenation
        supprimerEtat(A, act);
    }
    obtenirExpression(A, A->etat_initiaux[0], A->etat_finaux[0], regex);
}

void sauvegarderAvecNom(Automate a)
{
                printf("pour cibler le src entrer src/ suivi de votre nom de fichier");
                printf("Entrer le nom du fichier");
                char name[100];
                fgets(name,100,stdin);
                sauvgarder(a,name);
}

char* genererAvecNom(void)
{
                printf("pour cibler le src entrer src/ suivi de votre nom de fichier\n");
                printf("Entrer le nom du fichier: ");
                while(getchar() != '\n');
                char* name = malloc(100* sizeof(char));
                verifyAllocationWord(name);
                fgets(name,100,stdin);
                return name;
}   
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
            case 0 : printf("Fin du programme\n"); break;
            default : printf("Entrer un choix valide s'il vous plait!\n");
        }
	}while(output !=0);
	return 0;
}