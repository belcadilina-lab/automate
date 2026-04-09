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
void supprimerEtatsMorts(Automate *A) {
    int vivants[100]; // Tableau pour stocker les IDs des états vivants (co-accessibles)
    int nbr_vivants = 0;
    
    // 1. Initialiser avec les états finaux (ils sont vivants par définition)
    for (int i = 0; i < A->finc; i++) {
        vivants[nbr_vivants++] = A->etat_finaux[i];
    }
    
    // 2. Remonter les transitions pour trouver tous les autres états vivants
    int nouveau_trouve = 1;
    while (nouveau_trouve) {
        nouveau_trouve = 0;
        
        for (int i = 0; i < A->nbr_trans; i++) {
            int etat_dep = A->transitions[i].etat_dep;
            int etat_arriv = A->transitions[i].etat_arriv;
            
            // Si l'état d'arrivée est vivant, on vérifie si l'état de départ l'est déjà
            int arriv_est_vivant = 0;
            for (int j = 0; j < nbr_vivants; j++) {
                if (vivants[j] == etat_arriv) {
                    arriv_est_vivant = 1;
                    break;
                }
            }
            
            if (arriv_est_vivant) {
                // Vérifier si le départ est déjà dans la liste des vivants
                int dep_est_vivant = 0;
                for (int j = 0; j < nbr_vivants; j++) {
                    if (vivants[j] == etat_dep) {
                        dep_est_vivant = 1;
                        break;
                    }
                }
                
                // Si le départ n'y est pas, on l'ajoute et on relance un tour !
                if (!dep_est_vivant) {
                    vivants[nbr_vivants++] = etat_dep;
                    nouveau_trouve = 1;
                }
            }
        }
    }
    
    // 3. Reconstruire l'automate en ne gardant QUE les états et transitions vivants
    
    // a. Filtrer les états
    int nouv_nbr_etat = 0;
    for (int i = 0; i < A->nbr_etat; i++) {
        int est_vivant = 0;
        for (int j = 0; j < nbr_vivants; j++) {
            if (A->etats[i] == vivants[j]) est_vivant = 1;
        }
        if (est_vivant) {
            A->etats[nouv_nbr_etat++] = A->etats[i];
        }
    }
    A->nbr_etat = nouv_nbr_etat;
    
    // b. Filtrer les états initiaux (au cas où un init est mort, ce qui veut dire langage vide)
    int nouv_inic = 0;
    for (int i = 0; i < A->inic; i++) {
        int est_vivant = 0;
        for (int j = 0; j < nbr_vivants; j++) {
            if (A->etat_initiaux[i] == vivants[j]) est_vivant = 1;
        }
        if (est_vivant) {
            A->etat_initiaux[nouv_inic++] = A->etat_initiaux[i];
        }
    }
    A->inic = nouv_inic;
    
    // c. Filtrer les transitions
    int nouv_nbr_trans = 0;
    for (int i = 0; i < A->nbr_trans; i++) {
        int dep_est_vivant = 0;
        int arr_est_vivant = 0;
        
        for (int j = 0; j < nbr_vivants; j++) {
            if (A->transitions[i].etat_dep == vivants[j]) dep_est_vivant = 1;
            if (A->transitions[i].etat_arriv == vivants[j]) arr_est_vivant = 1;
        }
        
        // On garde la transition uniquement si le départ ET l'arrivée sont vivants
        if (dep_est_vivant && arr_est_vivant) {
            A->transitions[nouv_nbr_trans++] = A->transitions[i];
        }
    }
    A->nbr_trans = nouv_nbr_trans;
    
    // Pas besoin de filtrer les états finaux, ils sont vivants par définition !
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
    sauvgarder(*a,"automateInit.dot");
    Automate* aDet = malloc(sizeof(Automate));
    Automate* aMin = malloc(sizeof(Automate));
    initAutomate(aDet);
    initAutomate(aMin);
    supprimerEpsilons(a);
    supprimerEtatsInaccessibles(a);
    determiniserAutomate(a,aDet);
    sauvgarder(*aDet,"automateDeterministe.dot");
    printf("Deterministe");
    *aMin = minimiserMoore(aDet);
    sauvgarder(*aMin,"automateMinimaliste.dot"); 
    printf("les fichiers sont enregsitres avec succes");
    free(aDet);
    free(aMin);

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
    recopieAlphabet(&M,A->Alphabet,A->nbr_alph);
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


//ihssan
void produitAutomates(Automate *A1, Automate *A2, Automate *C) {
    int i, j;

    // 1. Initialisation
    initAutomate(C);

    // 2. Déterminer l'alphabet commun (Intersection des alphabets)
    for(i = 0; i < A1->nbr_alph; i++) {
        if(rechercherAlphabet(A2, A1->Alphabet[i])) {
            C->Alphabet[C->nbr_alph++] = A1->Alphabet[i];
        }
    }

    // 3. File BFS (Agrandie pour supporter les gros produits)
    Couple file[400];  
    int tete = 0, queue = 0;

    // 4. Ajouter les couples initiaux
    for(i = 0; i < A1->inic; i++) {
        for(j = 0; j < A2->inic; j++) {
            file[queue].e1 = A1->etat_initiaux[i];
            file[queue].e2 = A2->etat_initiaux[j];

            C->etats[C->nbr_etat++] = queue;        // L'indice 'queue' devient l'ID du nouvel état
            C->etat_initiaux[C->inic++] = queue;    // Enregistrer comme état initial
            queue++;
        }
    }

    // 5. BFS pour générer les transitions
    while(tete < queue) {
        Couple dep = file[tete];
        int dep_index = tete; // l'ID du couple de départ dans l'automate C
        tete++;

        // parcourir toutes les transitions de A1 depuis dep.e1
        for(i = 0; i < A1->nbr_trans; i++) {
            if(A1->transitions[i].etat_dep != dep.e1) continue;
            
            char symbole = A1->transitions[i].lettre[0]; // Sécurisé (pas de pointeur complexe)
            int arrivee1 = A1->transitions[i].etat_arriv;

            // parcourir toutes les transitions de A2 depuis dep.e2
            for(j = 0; j < A2->nbr_trans; j++) {
                if(A2->transitions[j].etat_dep != dep.e2) continue;
                
                // Comparaison sécurisée du premier caractère
                if(A2->transitions[j].lettre[0] != symbole) continue;

                int arrivee2 = A2->transitions[j].etat_arriv;

                // Chercher si le couple d'arrivée existe déjà dans notre file
                int arr_index = -1;
                for(int k = 0; k < queue; k++) {
                    if(file[k].e1 == arrivee1 && file[k].e2 == arrivee2) {
                        arr_index = k; // Trouvé !
                        break;
                    }
                }

                // S'il n'existe pas, on l'ajoute à la file
                if(arr_index == -1) {
                    file[queue].e1 = arrivee1;
                    file[queue].e2 = arrivee2;
                    C->etats[C->nbr_etat++] = queue;
                    arr_index = queue;
                    queue++;
                }

                // Ajouter la transition dans C
                C->transitions[C->nbr_trans].etat_dep = dep_index;
                C->transitions[C->nbr_trans].etat_arriv = arr_index;
                C->transitions[C->nbr_trans].lettre[0] = symbole;
                C->transitions[C->nbr_trans].lettre[1] = '\0'; // Fermeture de la chaîne
                C->nbr_trans++;
            }
        }
    }

    // 6. Définir les états finaux : intersection (e1 est final ET e2 est final)
    for(i = 0; i < queue; i++) {
        int e1 = file[i].e1;
        int e2 = file[i].e2;

        int est_final_A1 = estFinal(A1, e1); // Utilise ta propre fonction !
        int est_final_A2 = estFinal(A2, e2);

        if(est_final_A1 && est_final_A2) {
            C->etat_finaux[C->finc++] = i;
        }
    }
}