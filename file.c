#include "abstract.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>







int main(){
    TOF* P;
    header h;
    BUFFER buf;
    TOF_OPEN(&P,"test.bin",'n');
    h = P->h;
    printf("le nombre de blocs est : %d \nle nombre d'enregistrements est : %d\nle nombre d'enregistrements supprimes est : %d",h.nBlock,h.nRec,h.nDelRec);
    TOF_close(P);
    return 0;
}

