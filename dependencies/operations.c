#include "structs.h"
#include "stdbool.h"
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