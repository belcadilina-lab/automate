#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "operations.h"
#include "utilities.h"
#include "structs.h"
#include "lectureEtAffichage.h"
#include <stdlib.h>

//structure pour représenter les nouvelles etats

//ajoute un etat a un ensemble sans créer de doublon
void ajouterEtat_EnsSansRep(EnsembleEtats *ens, int etat) {
    for (int i = 0; i < ens->nbr; i++) {
        if (ens->etats[i] == etat) return;
    }
    ens->etats[ens->nbr] = etat;
    ens->nbr++;
}

//tester si deux ensembles contiennent les memes etats
bool ensemblesEgaux(EnsembleEtats *e1, EnsembleEtats *e2){
    if(e1->nbr != e2->nbr) return false;
    
    for(int i = 0; i < e1->nbr; i++){
        bool trouve = false;
        for(int j = 0; j < e2->nbr; j++){
            if (e1->etats[i] == e2->etats[j]){
                trouve = true;
                break;
            }
        }
        if (!trouve) return false;//si on trouve une seul etat diff en sort du fct
    }
    return true;
}

//verifie si un ensemble de vient final ou pas
bool contientEtatFinal(EnsembleEtats *ens, Automate *AFN) {
    for (int i = 0; i < ens->nbr; i++){
        for (int j = 0; j < AFN->finc; j++) {
            if (ens->etats[i] == AFN->etat_finaux[j]) return true;
        }
    }
    return false;
}
void determiniserAutomate(Automate *nonD, Automate *D) {
    initAutomate(D);
    D->nbr_alph = nonD->nbr_alph;
    recopieAlphabet(D,nonD->Alphabet,nonD->nbr_alph);
    //tableau pour stocker tous les nouveaux ensemble des etats
    EnsembleEtats Ens_nouv[50];
    int totalEns = 0;
    int ensemble_traite = 0;
    
    EnsembleEtats init;//regroupe les etats initial du nonD dans un ensemble des etats
    init.nbr = 0;
    init.id = 0;
    for(int i = 0; i < nonD->inic; i++) {
        init.etats[init.nbr] = nonD->etat_initiaux[i];
        init.nbr++;
    }
    
    Ens_nouv[totalEns++] = init;//total Ens = 1
    D->etats[D->nbr_etat++] = init.id;//stoker l'etat initiale dans l'automate D
    D->etat_initiaux[D->inic++] = init.id;

    //cherche de nouvelles ensemble
    while (ensemble_traite < totalEns) {
        EnsembleEtats *courant = &Ens_nouv[ensemble_traite];//etat dep dans D  

        //pour chaque lettre de l'alphabet
        for(int i = 0; i < nonD->nbr_alph; i++) {
            char lettre = nonD->Alphabet[i];
            EnsembleEtats Ens_dest ;
            Ens_dest.nbr = 0;
            Ens_dest.id = -1;

            for(int j = 0; j < courant->nbr; j++){
                int etat_dep = courant->etats[j];//chercher les destinations possibles depuis les etats de Ensemble de dep

                for(int k = 0; k < nonD->nbr_trans; k++) {
                    if(nonD->transitions[k].etat_dep == etat_dep && nonD->transitions[k].lettre[0] == lettre){
                        ajouterEtat_EnsSansRep(&Ens_dest, nonD->transitions[k].etat_arriv);//ajouter l'etat_arrive dans ensemble dest apres verification
                    }
                }
            }

            if (Ens_dest.nbr > 0) {
                // verifier si ce groupe existe dans notre tableau Ens_nouv
                int exist = -1;
                for (int k = 0; k < totalEns; k++) {
                    if (ensemblesEgaux(&Ens_dest, &Ens_nouv[k])){//comparer 2 ensemble entre eux
                        exist = Ens_nouv[k].id;//conserver la meme id d'Ens existant pour ajout des transition
                        break;
                    }
                }

                //s'il n'existe pas, on l'ajout
                if(exist == -1){
                    Ens_dest.id = totalEns; //nouvel id unique
                    Ens_nouv[totalEns] = Ens_dest;
                    totalEns++;
                    exist = Ens_dest.id;//
                    D->etats[D->nbr_etat] = exist;//ajout de nouvel etat(id de Ens_nouv) a D 
                    D->nbr_etat++;
                }

                //ajouter des transitions dans l'Automate D
                D->transitions[D->nbr_trans].etat_dep = courant->id;
                D->transitions[D->nbr_trans].etat_arriv = exist;//id de Ens_nouv ou un Ens deja existant
                D->transitions[D->nbr_trans].lettre[0] =  lettre;
                D->transitions[D->nbr_trans].lettre[1] = '\0';
                D->nbr_trans++;
            }
        }
        ensemble_traite++;//ensemle traite parmi tous les nouvelle Ens decouvertent 
    }

    for(int i = 0; i < totalEns; i++){// definir les nouvelles etas finale
        if(contientEtatFinal(&Ens_nouv[i], nonD)) {
            D->etat_finaux[D->finc++] = Ens_nouv[i].id;
        }
    }
}

void trois(Automate* a)
{
    printf("success");
    Automate* aDet = malloc(sizeof(Automate));
    Automate* aMin = malloc(sizeof(Automate));
    initAutomate(aDet);
    initAutomate(aMin);
    determiniserAutomate(a,aDet);
    sauvgarder(*aDet,"automateDeterministe.dot");
    printf("success");
    /* minimiser(aInit,aDet);
    sauvgarder(*aDet,"../automateDeterminise.dot");   */  

}
int trouverDestination(Automate *A, int etat, char symbole) {
    for (int i = 0; i < A->nbr_trans; i++) {
        if (A->transitions[i].etat_dep == etat && A->transitions[i].lettre[0] == symbole)
            return A->transitions[i].etat_arriv;
    }
    return -1; // État puits implicite ou erreur
}

int indiceEtat(Automate *A, int etat) {
    for (int i = 0; i < A->nbr_etat; i++) {
        if (A->etats[i] == etat) return i;
    }
    return -1;
}

int estFinal(Automate *A, int etat) {
    for (int i = 0; i < A->finc; i++) {
        if (A->etat_finaux[i] == etat) return 1;
    }
    return 0;
}

void copierAlphabet(Automate *dest, Automate *src){
    dest->nbr_alph = src->nbr_alph;
    for(int i = 0; i < src->nbr_alph; i++)
        dest->Alphabet[i] = src->Alphabet[i];
}

Automate minimiserMoore(Automate *A) {
    int n = A->nbr_etat;
    int groupe[20]; //classe de l'état à la position i dans A->etats[]
    int nouveau_groupe[20];//tableau temporaire pour calculer la nouvelle partition

    // 1. Partition initiale P0 (Finaux vs Non-Finaux)
    for (int i = 0; i < n; i++) {
        groupe[i] = estFinal(A, A->etats[i]) ? 1 : 0;
    }

    int stable = 0;
    while (!stable) {
        stable = 1;
        int groupe_courant = 0;

        // On réinitialise nouveau_groupe à -1 pour marquer les états non traités
        for (int i = 0; i < n; i++) nouveau_groupe[i] = -1;

        for (int i = 0; i < n; i++) {
            if (nouveau_groupe[i] != -1) continue;

            // L'état i définit un nouveau groupe
            nouveau_groupe[i] = groupe_courant;

            for (int j = i + 1; j < n; j++) {
                if (nouveau_groupe[j] != -1) continue;

                // Deux états sont équivalents s'ils sont dans le même groupe actuel
                // ET s'ils vont vers les mêmes groupes pour chaque symbole
                if (groupe[i] == groupe[j]) {
                    int identiques = 1;
                    for (int k = 0; k < A->nbr_alph; k++) {
                        char s = A->Alphabet[k];
                        int destI = trouverDestination(A, A->etats[i], s);
                        int destJ = trouverDestination(A, A->etats[j], s);

                        int gI = (destI == -1) ? -1 : groupe[indiceEtat(A, destI)];
                        int gJ = (destJ == -1) ? -1 : groupe[indiceEtat(A, destJ)];

                        if (gI != gJ) {
                            identiques = 0;
                            break;
                        }
                    }
                    if (identiques) {
                        nouveau_groupe[j] = groupe_courant;
                    }
                }
            }
            groupe_courant++;
        }

        for (int i = 0; i < n; i++) {
            if (groupe[i] != nouveau_groupe[i]) {
                stable = 0;
                break;
            }
        }

        // Mise à jour pour la prochaine itération
        for (int i = 0; i < n; i++) groupe[i] = nouveau_groupe[i];
    }

    // 3. Construction de l'automate minimal M
    Automate M;
    // Initialiser M à zéro/vide ici... (initAutomate(&M))

    initAutomate(&M);
    copierAlphabet(&M, A);

    // Déterminer le nombre de groupes uniques
    int max_g = 0;
    for (int i = 0; i < n; i++) if (groupe[i] > max_g) max_g = groupe[i];
    M.nbr_etat = max_g + 1;
    for (int i = 0; i < M.nbr_etat; i++) M.etats[i] = i;

    // État initial
    M.etat_initiaux[0] = groupe[indiceEtat(A, A->etat_initiaux[0])];
    M.inic = 1;
    
    // États finaux (un groupe est final si l'un de ses membres l'était)
    for (int g = 0; g <= max_g; g++) {
        for (int i = 0; i < n; i++) {
            if (groupe[i] == g && estFinal(A, A->etats[i])) {
                M.etat_finaux[M.finc++] = g;
                break; // Un seul représentant suffit
            }
        }
    }

    // Transitions
    for (int g = 0; g <= max_g; g++) {
        // Prendre le premier état trouvé appartenant au groupe g comme représentant
        int rep = -1;
        for (int i = 0; i < n; i++) {
            if (groupe[i] == g) { rep = i; break; }
        }

        for (int k = 0; k < A->nbr_alph; k++) {
            char s = A->Alphabet[k];
            int dest = trouverDestination(A, A->etats[rep], s);
            if (dest != -1) {
                int g_dest = groupe[indiceEtat(A, dest)];
                // Ajouter la transition (g --s--> g_dest) dans M
                M.transitions[M.nbr_trans].etat_dep = g;
                M.transitions[M.nbr_trans].etat_arriv = g_dest;
                M.transitions[M.nbr_trans].lettre[0] = s;
                M.transitions[M.nbr_trans].lettre[1] = '\0';
                M.nbr_trans++;
            }
        }
    }

    return M;
}

// Fonction récursive pour générer les mots
void genererMotsRec(Automate *A, int etat, char *mot_courant, int pos, FILE *f) {
    if (pos >= 5) return;
    if (estFinal(A, etat)) {
        mot_courant[pos] = '\0';
        fprintf(f, "%s\n", mot_courant);
    }

    for (int i = 0; i < A->nbr_trans; i++) {
        if (A->transitions[i].etat_dep == etat) {
            mot_courant[pos] = A->transitions[i].lettre[0];
            genererMotsRec(A, A->transitions[i].etat_arriv, mot_courant, pos + 1, f);
        }
    }
}

// Fonction principale qui minimise d'abord l'automate, puis écrit les mots
void ecrireMotsAutomateMinimal(Automate *A, const char *nom_fichier) {
    Automate M = minimiserMoore(A);
    FILE *f = fopen(nom_fichier, "w");
    if (!f) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", nom_fichier);
        return;
    }
    char mot_courant[5 + 1];
    genererMotsRec(&M, M.etat_initiaux[0], mot_courant, 0, f);
    fclose(f);
}