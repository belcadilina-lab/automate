#include <stdlib.h>
#include <stdio.h>
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
