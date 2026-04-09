#include "structs.h"
#include "stdbool.h"
#include "utilities.h"
#include "verification.h"
#include <string.h>
#include "operations.h"
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
int maxEtat(Automate *A) {
    int max = A->etats[0];
    for(int i = 1; i < A->nbr_etat; i++) {
        if(A->etats[i] > max)
            max = A->etats[i];
    }
    return max;
}

Automate concatAutomates(Automate *A1, Automate *A2, Automate *C) {
    int i, j;
    initAutomate(C);
    // Copier A1
    recopieAlphabet(C,A1->Alphabet,A1->nbr_alph);
    recopieEtats(C,A1->etats,A1->nbr_etat);
    recopieTransition(C,A1->transitions,A1->nbr_trans);
    recopieEtatsInitiale(C,A1->etat_initiaux,A1->inic);
    int offset = maxEtat(A1) + 1;

    // Copier A2 avec décalage
    for(i = 0; i < A2->nbr_etat; i++)
        C->etats[C->nbr_etat++] = A2->etats[i] + offset;

    for(i = 0; i < A2->nbr_trans; i++) {
        C->transitions[C->nbr_trans].etat_dep = A2->transitions[i].etat_dep + offset;
        C->transitions[C->nbr_trans].etat_arriv = A2->transitions[i].etat_arriv + offset;
        strcpy(C->transitions[C->nbr_trans].lettre,A2->transitions[i].lettre);
        C->nbr_trans++;
    }

    // Alphabet A2
    for(i = 0; i < A2->nbr_alph; i++) {
        if(!rechercherAlphabet(C, A2->Alphabet[i]))
            C->Alphabet[C->nbr_alph++] = A2->Alphabet[i];
    }

    // Ajouter epsilon
    if(!rechercherAlphabet(C, 'E'))
        C->Alphabet[C->nbr_alph++] = 'E';

    // Transitions epsilon
    for(i = 0; i < A1->finc; i++) {
        for(j = 0; j < A2->inic; j++) {
            C->transitions[C->nbr_trans].etat_dep = A1->etat_finaux[i];
            C->transitions[C->nbr_trans].etat_arriv = A2->etat_initiaux[j] + offset;
            strcpy(C->transitions[C->nbr_trans].lettre, "E");
            C->nbr_trans++;
        }
    }

    // Finaux
    for(i = 0; i < A2->finc; i++)
        C->etat_finaux[C->finc++] = A2->etat_finaux[i] + offset;

    return *C;
}
void initPileAutomates(PileAutomates *p) {
    p->sommet = -1;
}

void pushAutomate(PileAutomates *p, Automate A) {
    p->tab[++(p->sommet)] = A;
}

Automate popAutomate(PileAutomates *p) {
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
Automate automateLettre(char lettre) {
    Automate A;
    initAutomate(&A);

    int debut = nouvelEtat();
    int fin = nouvelEtat();

    ajouterTransition(&A, debut, fin, lettre);

    A.etat_initiaux[0] = debut;
    A.inic = 1;

    A.etat_finaux[0] = fin;
    A.finc = 1;

    return A;
}
Automate etoileAutomate(Automate A) {
    Automate C;
    initAutomate(&C);

    int debut = nouvelEtat();
    int fin = nouvelEtat();
    int i, j;

    // Copier transitions de A
    for (i = 0; i < A.nbr_trans; i++) {
        ajouterTransition(&C,
                          A.transitions[i].etat_dep,
                          A.transitions[i].etat_arriv,
                          A.transitions[i].lettre[0]);
    }

    // Nouvel initial -> initiaux de A
    for (i = 0; i < A.inic; i++) {
        ajouterTransition(&C, debut, A.etat_initiaux[i], 'E');
    }

    // Nouvel initial -> nouveau final (accepte epsilon)
    ajouterTransition(&C, debut, fin, 'E');

    // Finaux de A -> initiaux de A (boucle)
    for (i = 0; i < A.finc; i++) {
        for (j = 0; j < A.inic; j++) {
            ajouterTransition(&C, A.etat_finaux[i], A.etat_initiaux[j], 'E');
        }
    }

    // Finaux de A -> nouveau final
    for (i = 0; i < A.finc; i++) {
        ajouterTransition(&C, A.etat_finaux[i], fin, 'E');
    }

    C.etat_initiaux[0] = debut;
    C.inic = 1;

    C.etat_finaux[0] = fin;
    C.finc = 1;

    return C;
}

Automate plusAutomate(Automate A) {
    Automate C;
    initAutomate(&C);

    int debut = nouvelEtat();
    int fin = nouvelEtat();
    int i, j;

    // Copier transitions de A
    for (i = 0; i < A.nbr_trans; i++) {
        ajouterTransition(&C,
                          A.transitions[i].etat_dep,
                          A.transitions[i].etat_arriv,
                          A.transitions[i].lettre[0]);
    }

    // Nouvel initial -> initiaux de A
    for (i = 0; i < A.inic; i++) {
        ajouterTransition(&C, debut, A.etat_initiaux[i], 'E');
    }

    // Finaux de A -> initiaux de A (boucle)
    for (i = 0; i < A.finc; i++) {
        for (j = 0; j < A.inic; j++) {
            ajouterTransition(&C, A.etat_finaux[i], A.etat_initiaux[j], 'E');
        }
    }

    // Finaux de A -> nouveau final
    for (i = 0; i < A.finc; i++) {
        ajouterTransition(&C, A.etat_finaux[i], fin, 'E');
    }

    C.etat_initiaux[0] = debut;
    C.inic = 1;

    C.etat_finaux[0] = fin;
    C.finc = 1;

    return C;
}

void appliquerOperateur(PileAutomates *pileA, PileChar *pileC) {
    char op = popChar(pileC);
    Automate A2 = popAutomate(pileA);
    Automate A1 = popAutomate(pileA);
    Automate c,r;
    if (op == '+'){
       UnionStructure(&A1, &A2, &c);  // appelle la fonction, elle remplit 'c'
        pushAutomate(pileA, c);} 
    else if (op == '.'){
       concatAutomates(&A1, &A2, &r); // appelle la fonction, elle remplit 'r'
        pushAutomate(pileA, r);
}
}

void construireAutomateThompson(const char *regex, Automate *A) {
    PileAutomates pileA;
    initPileAutomates(&pileA);

    PileChar pileC;
    initPileChar(&pileC);

    for (int i = 0; regex[i] != '\0'; i++) {
        char c = regex[i];

        // Si c'est une lettre ou un chiffre
        if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) {
            pushAutomate(&pileA, automateLettre(c));

            // Si concaténation implicite déjà empilée
            if (!pileCharVide(&pileC) && topChar(&pileC) == '.') {
                appliquerOperateur(&pileA, &pileC);
            }
        }

        // Etoile ou plus
        else if (c == '*' || c == '^') {
            Automate temp = popAutomate(&pileA);

            if (c == '*')
                pushAutomate(&pileA, etoileAutomate(temp));
            else
                pushAutomate(&pileA, plusAutomate(temp));
        }

        // Union ou parenthèse ouvrante
        else if (c == '+' || c == '(') {
            pushChar(&pileC, c);
        }

        // Parenthèse fermante
        else if (c == ')') {
            while (!pileCharVide(&pileC) && topChar(&pileC) != '(') {
                appliquerOperateur(&pileA, &pileC);
            }
            if (!pileCharVide(&pileC)) {
                popChar(&pileC); // retire '('
            }
        }

        // Concaténation implicite
        char next = regex[i + 1];
        if (next != '\0' &&
            (((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '*' || c == '^' || c == ')')) &&
            (((next >= 'a' && next <= 'z') || (next >= '0' && next <= '9') || next == '('))) {
            pushChar(&pileC, '.');
        }
    }

    // Appliquer les opérateurs restants
    while (!pileCharVide(&pileC)) {
        appliquerOperateur(&pileA, &pileC);
    }

    // L'automate final
    *A = popAutomate(&pileA);
}
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
                                nouvelles_trans[nbr_nouv].etat_dep = etat_actuel;
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
                    for (int k = 0; k < Acc.nbr_etat; k++) {
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