#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "dependencies/operations.h"
#include "dependencies/utilities.h"

//structure pour représenter les nouvelles etats
typedef struct {
    int etats[20]; 
    int nbr;          
    int id; 
} EnsembleEtats;

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
                    if(nonD->transitions[k].etat_dep == etat_dep && strcmp(nonD->transitions[k].lettre[0], lettre) == 0){
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
                strcpy(D->transitions[D->nbr_trans].lettre[0], lettre);
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