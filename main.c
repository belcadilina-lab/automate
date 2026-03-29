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
    char etats[20];
	int nbr_etat;
    Transition transitions[50];
	int nbr_trans;
    char Alphabet[10];
	int nbr_alph;
    char etat_initiaux[5];
	int inic;//nbr des etats initial
    char etat_finaux[10];
	int finc;//nbr des etats finaux
}Automate;
void supprimerEpsilons(Automate *A);
void checkFile(FILE* test)
{
    if(test == NULL) perror("erreur ouverture");
    else printf("fichier ouvert avec succés\n");
}
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
			    if(rechercherAlphabet(protocol, val) == false && val != 'E'){
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
void Union(void)
{
    Automate autom1;
    Automate autom2;
    FILE* U = fopen("src/automate1.dot","r");
    checkFile(U);
    FILE* V = fopen("src/automate2.dot","r");
    checkFile(V);
    FILE* W = fopen("src/resultat.dot","w");
    checkFile(W);
    readDot(&autom1,"src/automate1.dot");
    readDot(&autom2,"src/automate2.dot");
    fprintf(W, "digraph G {\n");
    fprintf(W, "init [shape=point];\n");
    fprintf(W, "fin [shape=point];\n");
    fprintf(W, "init -> q;\n");
    fprintf(W, "q -> i [label=\"3\"] ;\n");
    fprintf(W, "q -> I [label=\"3\"] ;\n");    
    for(int i = 0 ; i< autom1.inic ; i++)
    fprintf(W, "i -> %c [label=\"3\"];\n",autom1.etat_initiaux[i]);

    for(int k = 0; k<autom1.nbr_trans ; k++)
    {
        fprintf(W, "%c -> %c [label=\"%c\"] ;\n",autom1.transitions[k].etat_dep,autom1.transitions[k].etat_arriv,autom1.transitions[k].lettre);    
    }

    for(int i = 0 ; i< autom2.inic ; i++)
    fprintf(W, "I -> %c [label=\"3\"];\n",autom2.etat_initiaux[i]);

    for(int k = 0; k<autom2.nbr_trans ; k++)
    {
        fprintf(W, "%c -> %c [label=\"%c\"] ;\n",autom2.transitions[k].etat_dep,autom2.transitions[k].etat_arriv,autom2.transitions[k].lettre);    
    }
    //etats finaux

    for(int i = 0 ; i< autom1.finc ; i++)
    fprintf(W, "%c -> f [label=\"3\"];\n",autom1.etat_finaux[i]);


    for(int i = 0 ; i< autom2.finc ; i++)
    fprintf(W, "%c -> F [label=\"3\"];\n",autom2.etat_finaux[i]);

  
    fprintf(W, "f -> p [label=\"3\"];\n");
    fprintf(W, "F -> p [label=\"3\"];\n");   
    fprintf(W, "p -> fin;\n");   



    fprintf(W, "}");
fclose(U);
fclose(V);
fclose(W);
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


//fct verifier qu'un transition n'existe deja dans le tableau des transition
bool transitionExiste(Transition *tab, int nbr, char dep, char arriv, char lettre) {
    for (int i = 0; i < nbr; i++) {
        if (tab[i].etat_dep == dep && tab[i].etat_arriv == arriv && tab[i].lettre == lettre) {
            return true;
        }
    }
    return false;
}

//calcul le epsilon-close de chaque etat
void calculFermetureEpsilon(Automate *A, char etat, char *fermeture, int *taille) {
    fermeture[0] = etat; // L'état lui-même fait partie de sa fermeture
    *taille = 1;
    //parcoure A->transitions pour trouver tous les etats accessibles par epsilon 'e'
    for (int i = 0; i < *taille; i++) {
        char etat_courant = fermeture[i];
        for (int j = 0; j < A->nbr_trans; j++) {
            if (A->transitions[j].etat_dep == etat_courant && A->transitions[j].lettre == 'e') {
                char etat_suivant = A->transitions[j].etat_arriv;
                
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
    char accessibles[20];
    int nbr_acc = 0;
    for (int i = 0; i < A->inic; i++) {
        accessibles[nbr_acc] = A->etat_initiaux[i];
        nbr_acc++;
    }
    
    bool nouveau_trouve;//trouver tous les etats accessibles
    do {
        nouveau_trouve = false;
        for (int i = 0; i < nbr_acc; i++) {
            char etat_courant = accessibles[i];

            for (int j = 0; j < A->nbr_trans; j++) {
                if (A->transitions[j].etat_dep == etat_courant) {
                    char etat_dest = A->transitions[j].etat_arriv;
                    
                    bool existe = false;//verifier que etat_dest n'est pas deja accessible
                    for (int k = 0; k < nbr_acc; k++) {
                        if (accessibles[k] == etat_dest) {
                            existe = true;
                            break;
                        }
                    }
                    if (!existe) {
                        accessibles[nbr_acc] = etat_dest;
                        nbr_acc++;
                        nouveau_trouve = true;
                    }
                }
            }
        }
    } while (nouveau_trouve);

    for (int i = 0; i < nbr_acc; i++) {//supprimer les etas inaccesibles du tableau des etats
        A->etats[i] = accessibles[i];
    }
    A->nbr_etat = nbr_acc;

    Transition trans_utiles[50];//tableau temporaire pour supprimer les transitions des etats inaccessibles
    int nbr_t_utiles = 0;
    for (int i = 0; i < A->nbr_trans; i++) {
        bool dep_accessible = false;
        for (int k = 0; k < nbr_acc; k++) {
            if (A->transitions[i].etat_dep == accessibles[k]) { 
                dep_accessible = true;
                break;
            }
        }
        //si l'etat depart est accessible, on garde la transition
        if (dep_accessible) {
            trans_utiles[nbr_t_utiles] = A->transitions[i];
            nbr_t_utiles++;
        }
    }
    for (int i = 0; i < nbr_t_utiles; i++) {// Remplacer les anciennes transitions
        A->transitions[i] = trans_utiles[i];
    }
    A->nbr_trans = nbr_t_utiles;

    char finaux_utiles[10];//de meme pour les etats finaux
    int nbr_f_utiles = 0;
    for (int i = 0; i < A->finc; i++) {
        bool est_accessible = false;
        for (int k = 0; k < nbr_acc; k++) {
            if (A->etat_finaux[i] == accessibles[k]) {
                est_accessible = true;
                break;
            }
        }
        if (est_accessible) {
            finaux_utiles[nbr_f_utiles] = A->etat_finaux[i];
            nbr_f_utiles++;
        }
    }
    for (int i = 0; i < nbr_f_utiles; i++) {
        A->etat_finaux[i] = finaux_utiles[i];
    }
    A->finc = nbr_f_utiles;

    printf("Les etats inaccessibles ont ete supprimes.\n");
}
// fonction supprime les epsilons dans une automate
void supprimerEpsilons(Automate *A) {
    Transition nouvelles_trans[100]; // Tableau temporaire pour stocker les nouvelles transitions
    int nbr_nouv = 0;//taile tableu nouv

            for (int i = 0; i < A->nbr_etat; i++) {//parcour les etat du l'automate
                char etat_actuel = A->etats[i];
                char fermeture[A->nbr_etat];//epsilon-close de chaque etat
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
                    char etat_p = fermeture[j];
                    
                    // Chercher les transitions partant de p qui ne sont PAS des epsilons
                    for (int k = 0; k < A->nbr_trans; k++) {
                        if (A->transitions[k].etat_dep == etat_p && A->transitions[k].lettre != '3') {
                            if (!transitionExiste(nouvelles_trans, nbr_nouv, etat_actuel, A->transitions[k].etat_arriv, A->transitions[k].lettre)) {
                                nouvelles_trans[nbr_nouv].etat_dep = etat_actuel;
                                nouvelles_trans[nbr_nouv].etat_arriv = A->transitions[k].etat_arriv;
                                nouvelles_trans[nbr_nouv].lettre = A->transitions[k].lettre;
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
bool Exists(Automate* autom, char * str){
    supprimerEpsilons(autom);
    char courant[20], suivant[20];
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
                if(autom->transitions[j].etat_dep == courant[s] && autom->transitions[j].lettre == str[i])
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
		if(Exists(A,mot) == true){
		 	fprintf(fptr,"%s \n",mot);
		}
	}
    fclose(fptr);
    fclose(ftxt);
}
typedef struct Fragment { // représente un petit automate temporaire
    int debut;
    int fin;
} Fragment;

typedef struct PileFragments { // stocker les fragments pendant la construction
    Fragment tab[100];
    int sommet;
} PileFragments;

typedef struct PileChar { // lire les opérateurs, il faut les empiler pour savoir l'ordre et la priorité
    char tab[100];
    int sommet;
} PileChar;

void initPileFragments(PileFragments *p) { // initialiser la pile
    p->sommet = -1;
}

void pushFragment(PileFragments *p, Fragment f) {  // empiler les fragments
    p->tab[++(p->sommet)] = f;
}

Fragment popFragment(PileFragments *p) {  // dépiler les fragments
    return p->tab[(p->sommet)--];
}

void initPileChar(PileChar *p) { // initialiser la pile des opérateurs
    p->sommet = -1;
}

void pushChar(PileChar *p, char c) { // empiler un opérateur
    p->tab[++(p->sommet)] = c;
}

char popChar(PileChar *p) {
    return p->tab[(p->sommet)--];
}

char topChar(PileChar *p) {  // regarde le sommet de la pile sans le modifier
    return p->tab[p->sommet];
}

bool pileCharVide(PileChar *p) { // vérifier si la pile est vide
    return p->sommet == -1;
}

int nouvelEtat() {  // créer les états
    static int etat = 1;
    return etat++;
}

void ajouterEtatSiAbsent(Automate *A, char e) { // vérifier si l'état est absent pour l'ajouter
    if (!rechercherEtat(A, e) && A->nbr_etat < 20) {
        A->etats[A->nbr_etat++] = e;
    }
}

void ajouterAlphabetSiAbsent(Automate *A, char c) { // ajouter une lettre
    if (c != 'e' && !rechercherAlphabet(A, c) && A->nbr_alph < 10) {
        A->Alphabet[A->nbr_alph++] = c;
    }
}

void ajouterTransition(Automate *A, char dep, char arriv, char lettre) {
    if (A->nbr_trans >= 50) {
        printf("Erreur : nombre maximum de transitions atteint\n");
        return;
    }

    if (transitionExiste(A->transitions, A->nbr_trans, dep, arriv, lettre)) {
        return; // ne pas ajouter si elle existe déjà
    }

    A->transitions[A->nbr_trans].etat_dep = dep;
    A->transitions[A->nbr_trans].etat_arriv = arriv;
    A->transitions[A->nbr_trans].lettre = lettre;
    A->nbr_trans++;

    ajouterEtatSiAbsent(A, dep);
    ajouterEtatSiAbsent(A, arriv);
    ajouterAlphabetSiAbsent(A, lettre);
}

// construction des fragments
Fragment automateLettre(Automate *A, char lettre) {
    char debut = nouvelEtat();
    char fin = nouvelEtat();
    ajouterTransition(A, debut, fin, lettre);
    Fragment f = {debut, fin};
    return f;
}
Fragment concatFragments(Automate *A, Fragment f1, Fragment f2) {
    ajouterTransition(A, f1.fin, f2.debut, 'e'); //concatener deux fragments en ajoutant les transitions epsilon
    Fragment f = {f1.debut, f2.fin};
    return f;
}
Fragment unionFragments(Automate *A, Fragment f1, Fragment f2) {
    char debut = nouvelEtat();
    char fin = nouvelEtat();
    ajouterTransition(A, debut, f1.debut, 'e'); //soit choisir d'aller vers le premier fragment
    ajouterTransition(A, debut, f2.debut, 'e'); //soit par le deuxieme
    ajouterTransition(A, f1.fin, fin, 'e');
    ajouterTransition(A, f2.fin, fin, 'e');
    Fragment f = {debut, fin};
    return f;
}
Fragment etoileFragment(Automate *A, Fragment f) {
    char debut = nouvelEtat();
    char fin = nouvelEtat();
    ajouterTransition(A, debut, f.debut, 'e'); //commence une répétition de f
    ajouterTransition(A, debut, fin, 'e'); //permet de ne pas prendre f du tout (0 répétition)
    ajouterTransition(A, f.fin, f.debut, 'e'); // repetition
    ajouterTransition(A, f.fin, fin, 'e'); //permet de terminer après n répétitions
    Fragment nf = {debut, fin};
    return nf;
}

 Fragment plusFragment(Automate *A, Fragment f) {
    // Boucle : relie la fin du fragment à son début
    ajouterTransition(A, f.fin, f.debut, 'e');

    // Le fragment conserve le même début et fin
    Fragment nf = {f.debut, f.fin};
    return nf;
}


int priorite(char op) {
    if (op == '*') return 3;
	if (op == '^') return 3; //plus grande priorité
    if (op == '.') return 2;
    if (op == '+') return 1;
    return 0; // '(' ou autres
}
void construireAutomateThompson(const char *regex, Automate *A) {
    PileFragments pileF; //initialisation des piles
    PileChar pileC;
    initPileFragments(&pileF);
    initPileChar(&pileC);

    Fragment f, f1, f2;
    char c; //caractère courant de la regex

    for (int i = 0; regex[i] != '\0'; i++) { //parcours l'expression reguliere
        c = regex[i];

        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) { // si c est un symbole
            f = automateLettre(A, c);   //on cree un fragment
            if (!pileCharVide(&pileC) && topChar(&pileC) == '.') { //Si la pile d’opérateurs n’est pas vide et que le sommet de la pile est un opérateur de concaténation, alors on doit dépiler cet opérateur et effectuer la concaténation des deux fragments correspondants
                popChar(&pileC);
                f2 = popFragment(&pileF);
                f = concatFragments(A, f2, f);
            }
            pushFragment(&pileF, f); //on empile le nouveau fragment
        }
        else if (c == '*') { //cas si on a l'etoile
            f = popFragment(&pileF);
            f = etoileFragment(A, f);
            pushFragment(&pileF, f);
        }
        else if (c == '^') { // plus (1 ou plusieurs)
    f = popFragment(&pileF);
    f = plusFragment(A, f);  
    pushFragment(&pileF, f);
}
        
        else if (c == '+') {
            pushChar(&pileC, '+');
        }
        else if (c == '(') {
            pushChar(&pileC, '(');
        }
        else if (c == ')') {
            while (!pileCharVide(&pileC) && topChar(&pileC) != '(') {
                char op = popChar(&pileC);
                f2 = popFragment(&pileF);
                f1 = popFragment(&pileF);
                if (op == '+') f = unionFragments(A, f1, f2);
                else if (op == '.') f = concatFragments(A, f1, f2);
                pushFragment(&pileF, f);
            }
            if (!pileCharVide(&pileC) && topChar(&pileC) == '(') popChar(&pileC);
        }

        // concat implicite
        if (i + 1 < strlen(regex)) {
            char next = regex[i + 1];
            if (((c >= 'a' && c <= 'z') || c == '*' || c == ')') &&
                ((next >= 'a' && next <= 'z') || next == '(')) {
                pushChar(&pileC, '.');
            }
        }
    }

    // appliquer les opérateurs restants
    while (!pileCharVide(&pileC)) {
        char op = popChar(&pileC);
        f2 = popFragment(&pileF);
        f1 = popFragment(&pileF);
        if (op == '+') f = unionFragments(A, f1, f2);
        else if (op == '.') f = concatFragments(A, f1, f2);
        pushFragment(&pileF, f);
    }

    Fragment finalF = popFragment(&pileF); // depilement de l'automate construit
    A->etat_initiaux[0] = finalF.debut; //declaration de l'etat initial
    A->inic = 1;
    A->etat_finaux[0] = finalF.fin; //declaration de l'etat final
    A->finc = 1;
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
            case 8 : {
                supprimerEpsilons(&a);
                supprimerEtatsInaccessibles(&a);
                automateShow(a);
                break;
            }
            case 9:{
                Union();
                break;
            }
            case 10: {
                char regex[100];
                printf("Entrer l'expression reguliere: ");
                scanf("%s", regex);
                // réinitialiser l'automate avant de le construire
                a.nbr_etat = 0;
                a.nbr_trans = 0;
                a.nbr_alph = 0;
                a.inic = 0;
                a.finc = 0;

                // construire l'automate avec Thompson
                construireAutomateThompson(regex, &a);

                printf("Automate genere avec succes a partir de l'expression reguliere '%s'.\n", regex);
                automateShow(a);
                break;
            }
            case 11: {
                Automate a1, a2, c;
                printf("\n--- Chargement de l'automate A1 ---\n");
                readDot(&a1, "automate_a1.dot");

                printf("\n--- Chargement de l'automate A2 ---\n");
                readDot(&a2, "automate_a2.dot");

                if(a1.nbr_etat == 0 || a2.nbr_etat == 0){
                    printf("Erreur : un des automates est vide ou mal charge !\n");
                    break;
                }
               // c = concatAutomates(&a1, &a2);

                printf("\n===== AUTOMATE RESULTAT  =====\n");
                automateShow(c);
                sauvgarder(c);
                printf("\nFichier genere : automate_concate.dot\n");
                break;
            }
            case 0 : printf("Fin du programme\n"); break;
            default : printf("Entrer un choix valide s'il vous plait!\n");
        }
	}while(output !=0);
	return 0;
}