
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