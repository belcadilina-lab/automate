#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "dependencies/operations.h"
#include "dependencies/utilities.h"

//structure pour représenter les nouvelles etats
typedef struct {
    int nfa_etats[20]; 
    int nbr;          
    int dfa_id; 
} EnsembleEtats;

//ajoute un etat a un ensemble sans créer de doublon
void ajouterEtatSansDoublon(EnsembleEtats *ens, int etat) {
    for (int i = 0; i < ens->nbr; i++) {
        if (ens->nfa_etats[i] == etat) return;
    }
    ens->nfa_etats[ens->nbr] = etat;
    ens->nbr++;
}

//tester si deux ensembles contiennent les memes etats
bool ensemblesEgaux(EnsembleEtats *e1, EnsembleEtats *e2){
    if(e1->nbr != e2->nbr) return false;
    
    for(int i = 0; i < e1->nbr; i++){
        bool trouve = false;
        for(int j = 0; j < e2->nbr; j++){
            if (e1->nfa_etats[i] == e2->nfa_etats[j]){
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
            if (ens->nfa_etats[i] == AFN->etat_finaux[j]) return true;
        }
    }
    return false;
}
void determiniserAutomate(Automate *AFN, Automate *AFD) {
    initAutomate(AFD);
    AFD->nbr_alph = AFN->nbr_alph;
    recopieAlphabet(AFD,AFN->Alphabet,AFN->nbr_alph);
    // Tableau pour stocker tous les nouveaux états (sous-ensembles) découverts
    EnsembleEtats file_attente[50];
    int total_ensembles = 0;
    int ensembles_traites = 0;

    // ÉTAPE 1 : Créer l'état initial de l'AFD (regroupant tous les états initiaux de l'AFN)
    EnsembleEtats etat_init;
    etat_init.nbr = 0;
    etat_init.dfa_id = 0;
    for (int i = 0; i < AFN->inic; i++) {
        etat_init.nfa_etats[etat_init.nbr] = AFN->etat_initiaux[i];
        etat_init.nbr++;
    }
    //////////////////////////////////////////////////////
    file_attente[total_ensembles++] = etat_init;
    AFD->etats[AFD->nbr_etat++] = etat_init.dfa_id;
    AFD->etat_initiaux[AFD->inic++] = etat_init.dfa_id;

    // ÉTAPE 2 : Traiter chaque nouvel ensemble découvert
    while (ensembles_traites < total_ensembles) {
        EnsembleEtats *courant = &file_attente[ensembles_traites];

        // Pour chaque lettre de l'alphabet
        for (int a = 0; a < AFN->nbr_alph; a++) {
            char lettre_str[2] = { AFN->Alphabet[a], '\0' };
            EnsembleEtats nouveau_groupe = { .nbr = 0, .dfa_id = -1 };

            // Chercher toutes les destinations possibles depuis les états de 'courant'
            for (int i = 0; i < courant->nbr; i++) {
                int etat_dep = courant->nfa_etats[i];

                for (int t = 0; t < AFN->nbr_trans; t++) {
                    // On utilise strcmp car 'lettre' est un tableau de char[200]
                    if (AFN->transitions[t].etat_dep == etat_dep && 
                        strcmp(AFN->transitions[t].lettre, lettre_str) == 0) {
                        ajouterEtatSansDoublon(&nouveau_groupe, AFN->transitions[t].etat_arriv);
                    }
                }
            }

            // Si le nouveau groupe n'est pas vide (il y a des transitions)
            if (nouveau_groupe.nbr > 0) {
                // Vérifier si ce groupe existe déjà dans notre file d'attente
                int id_existant = -1;
                for (int k = 0; k < total_ensembles; k++) {
                    if (ensemblesEgaux(&nouveau_groupe, &file_attente[k])) {
                        id_existant = file_attente[k].dfa_id;
                        break;
                    }
                }

                // S'il n'existe pas, on le crée
                if (id_existant == -1) {
                    nouveau_groupe.dfa_id = total_ensembles; // Nouvel ID unique
                    file_attente[total_ensembles] = nouveau_groupe;
                    id_existant = nouveau_groupe.dfa_id;
                    
                    AFD->etats[AFD->nbr_etat++] = id_existant;
                    total_ensembles++;
                }

                // Ajouter la transition dans l'AFD
                AFD->transitions[AFD->nbr_trans].etat_dep = courant->dfa_id;
                AFD->transitions[AFD->nbr_trans].etat_arriv = id_existant;
                strcpy(AFD->transitions[AFD->nbr_trans].lettre, lettre_str);
                AFD->nbr_trans++;
            }
        }
        ensembles_traites++;
    }

    // ÉTAPE 3 : Définir les états finaux de l'AFD
    for (int i = 0; i < total_ensembles; i++) {
        if (contientEtatFinal(&file_attente[i], AFN)) {
            AFD->etat_finaux[AFD->finc++] = file_attente[i].dfa_id;
        }
    }
}