#include <ctype.h>
// Structure pour une entrée dans la table des symboles
typedef struct {
    char mot[100];
    int numero_ligne;
} EntreeTableSymboles;
// Structure pour la table des symboles
typedef struct {
    EntreeTableSymboles entrees[500];
    int nbr_entrees;
} TableSymboles;
// Crée et remplit la table des symboles
TableSymboles creerTableSymboles(Automate *A, const char *nom_fichier) {
    TableSymboles table;
    table.nbr_entrees = 0;

    FILE *f = fopen(nom_fichier, "r");
    if (!f) {
        printf("Erreur : impossible d'ouvrir le fichier %s\n", nom_fichier);
        return table;
    }

    char ligne[1000];
    int numero_ligne = 1;

    while (fgets(ligne, sizeof(ligne), f)) {
        char mot[100];
        int i = 0;

        for (int j = 0; ligne[j] != '\0'; j++) {
            char c = ligne[j];

            if (!isalnum(c)) {
                if (i > 0) {
                    mot[i] = '\0';

                    if (testerMot(A, mot)&&table.nbr_entrees < 500) {
                        strcpy(table.entrees[table.nbr_entrees].mot, mot);
                        table.entrees[table.nbr_entrees].numero_ligne = numero_ligne;
                        table.nbr_entrees++;
                    }

                    i = 0;
                }
            } else if (i < 99) {
                 mot[i++] = c;
                } 
        }

        // Traiter le dernier mot de la ligne
        if (i > 0) {
            mot[i] = '\0';
            if (testerMot(A, mot)&&table.nbr_entrees < 500) {
                strcpy(table.entrees[table.nbr_entrees].mot, mot);
                table.entrees[table.nbr_entrees].numero_ligne = numero_ligne;
                table.nbr_entrees++;
            }
        }

        numero_ligne++;
    }

    fclose(f);
    return table;
}

// Afficher la table des symboles
void afficherTableSymboles(TableSymboles *table) {
    printf("\n TABLE DES SYMBOLES \n");
    printf("%-20s | %s\n", "MOT", "LIGNE");
    for (int i = 0; i < table->nbr_entrees; i++) {
        printf("%-20s | %d\n",
               table->entrees[i].mot,
               table->entrees[i].numero_ligne);
    }
    printf("Total : %d mot(s) reconnu(s)\n", table->nbr_entrees);
}