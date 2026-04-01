#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
typedef struct Transition{
    int etat_dep;
    int etat_arriv;
    char lettre[200];
}Transition;

typedef struct Automate{
    int etats[20];
	int nbr_etat;
    Transition transitions[50];
	int nbr_trans;
    char Alphabet[10];
	int nbr_alph;
    int etat_initiaux[5];
	int inic;//nbr des etats initial
    int etat_finaux[10];
	int finc;//nbr des etats finaux
}Automate;
void verifyAllocationWord(char* a)
{
    if (a==NULL) 
    {
     perror("Erreur Allocation"); 
     exit(EXIT_FAILURE);
    }
}
void checkFile(FILE* test)
{
    if(test == NULL){
    perror("erreur ouverture");
    exit(EXIT_FAILURE);
    } 
    //else printf("fichier ouvert avec succes\n");
}
void initAutomate(Automate* a1)
{
    a1->finc = 0;
    a1->inic = 0;
    a1->nbr_alph = 0;
    a1->nbr_etat = 0;
    a1->nbr_trans = 0;
    //fonction copie des valeurs
}
void recopieEtats(Automate *A, int Etats[], int taille){
    for(int i =0;i<taille; i++){
        A->etats[i]= Etats[i];
    }
    A->nbr_etat = taille;
}
void recopieEtatsInitiale(Automate *A, int Etats[], int taille){
    for(int i =0;i<taille; i++){
        A->etat_initiaux[i]= Etats[i];
    }
    A->inic = taille;
}
void recopieEtatsFinaux(Automate *A, int Etats[], int taille){
    for(int i =0;i<taille; i++){
        A->etat_finaux[i]= Etats[i];
    }
    A->finc = taille;
}
void recopieTransition(Automate *A, Transition trans[], int taille){
    for(int i =0;i<taille; i++){
        // A->transitions[i].etat_dep= trans[i].etat_dep;
        // A->transitions[i].etat_arriv= trans[i].etat_arriv;
        // strncpy(A->transitions[i].lettre,trans[i].lettre,sizeof(trans[i].lettre));
        A->transitions[i]=trans[i];
    }
    A->nbr_trans = taille;
}
bool rechercherEtat(Automate *protocol, int etatChar){
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
void UnionFichiers(const char* a1, const char* a2)
{
    Automate autom1;
    Automate autom2;
    FILE* U = fopen(a1,"r");
    checkFile(U);
    FILE* V = fopen(a2,"r");
    checkFile(V);
    FILE* W = fopen("src/resultat.dot","w");
    checkFile(W);
    readDot(&autom1,"src/automate1.dot");
    readDot(&autom2,"src/automate2.dot");
    fprintf(W, "digraph G {\n");
    fprintf(W, "init [shape=point style=filled, fillcolor=white color=white ];\n");
    fprintf(W, "fin [shape=point style=filled, fillcolor=white color=white];\n");
    fprintf(W, "init -> 100;\n");
    //stock les etats initiales du 1er automate et ses transitions   
    for(int i = 0 ; i< autom1.inic ; i++)
    fprintf(W, "100 -> %d [label=\"E\"];\n",autom1.etat_initiaux[i]);

    for(int k = 0; k<autom1.nbr_trans ; k++)
    {
        fprintf(W, "%d -> %d [label=\"%c\"] ;\n",autom1.transitions[k].etat_dep,autom1.transitions[k].etat_arriv,autom1.transitions[k].lettre[0]);    
    }
    //stock les etats initiales du 2eme automate et ses transitions
    for(int i = 0 ; i< autom2.inic ; i++)
    fprintf(W, "100 -> %d [label=\"E\"];\n",autom2.etat_initiaux[i]);

    for(int k = 0; k<autom2.nbr_trans ; k++)
    {
        fprintf(W, "%d -> %d [label=\"%c\"] ;\n",autom2.transitions[k].etat_dep,autom2.transitions[k].etat_arriv,autom2.transitions[k].lettre[0]);    
    }
    //etats finaux

    for(int i = 0 ; i< autom1.finc ; i++)
    fprintf(W, "%d -> 200 [label=\"E\"];\n",autom1.etat_finaux[i]);


    for(int i = 0 ; i< autom2.finc ; i++)
    fprintf(W, "%d -> 200 [label=\"E\"];\n",autom2.etat_finaux[i]);
    fprintf(W, "200 -> fin;\n");   
    fprintf(W, "}");
fclose(U);
fclose(V);
fclose(W);
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
bool rechercherEtatFinale(Automate *protocol, int etatChar){
	for(int i = 0 ; i< protocol->finc; i++){
		if(protocol->etat_finaux[i] == etatChar ){
			return true;
		}
	}
		return false;

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

//fct verifier qu'un transition n'existe deja dans le tableau des transition
bool transitionExiste(Transition *tab, int nbr, int dep, int arriv, char lettre) {
    for (int i = 0; i < nbr; i++) {
        if (tab[i].etat_dep == dep && tab[i].etat_arriv == arriv && tab[i].lettre[0] == lettre) {
            return true;
        }
    }
    return false;
}

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
typedef struct Fragment { 
    int debut; 
    int fin; 
} Fragment;
typedef struct PileFragments { 
    Fragment tab[100]; 
    int sommet; 
} PileFragments;
typedef struct PileChar { 
    char tab[100]; 
    int sommet; 
} PileChar;
//les piles
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
copy(Automate* auto1, Automate* result)
{
    for(int i= 0; i<auto1->inic ; i++)
    {
        result->inic++;
        if(result->etat_initiaux[result->inic] == auto1->etat_initiaux[i]){
        while(result->etat_initiaux[result->inic] == auto1->etat_initiaux[i]){
            result->etat_initiaux[result->inic]++;
        }

        }


    }
}
void copyToResult(Automate auto1, Automate* result)
{

}
void fromEpsToA(Automate* result)
{
    
    for(int i = 0; i< result->inic ; i++)
    {
        result->nbr_trans++;
        result->transitions[result->nbr_trans].etat_dep = 100;
        result->transitions[result->nbr_trans].etat_arriv = result->etat_initiaux[i];
        strcpy(result->transitions[result->nbr_trans].lettre, "E");
    }

}

void fromAtoEps(Automate* result)
{
        for(int i = 0; i< result->finc ; i++)
    {
        result->nbr_trans++;
        result->transitions[result->nbr_trans].etat_dep = result->etat_finaux[i];
        result->transitions[result->nbr_trans].etat_arriv = 200;
        strcpy(result->transitions[result->nbr_trans].lettre, "E");
    }
}
void UnionStructure(Automate* auto1, Automate* auto2, Automate* result)
{

    initAutomate(result);

    fromEpsToA(auto1);
    fromEpsToA(auto2);
    fromAtoEps(auto1);
    fromAtoEps(auto2);
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
                printf("pour cibler le src entrer src/ suivi de votre nom de fichier");
                printf("Entrer le nom du fichier");
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
                int choix;
                printf("1. Union par fichiers: \n""2. Union dans la structure: \n");
                printf("quel type d'union vous voulez effectuer: ");
                scanf("%d",&choix);
                switch (choix)
                {
                case 1:
                char* autoN = malloc(sizeof(char) * 100);
                char* autoM = malloc(sizeof(char) * 100);
                verifyAllocationWord(autoN);
                verifyAllocationWord(autoM);
                autoN = genererAvecNom();
                autoM = genererAvecNom();
                    UnionFichiers(autoN,autoM);
                    break;
                case 2: 
                Automate* a4;
                Automate* a3;
                Automate* resultant;
                char* buffer1= malloc(sizeof(char) * 100);
                char* buffer2 = malloc(sizeof(char) * 100);
                verifyAllocationWord(buffer1);
                verifyAllocationWord(buffer2);
                autoN = genererAvecNom();
                autoM = genererAvecNom();
                readDot(&a4,buffer1);
                readDot(&a3,buffer2);
                Automate resultant;
                UnionStructure(a3,a4,resultant);
                default:
                    break;
                }
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