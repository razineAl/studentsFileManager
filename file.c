#include "abstract.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
// #include "TOF_operations.h"


long block_in_MM = -1;

void binsearch(TOF* P, int val, int *found, long *i, int *j );

int main(){
    TOF* P;
    tRec r = {10000,"razine","aliouche","26/10/2005","medea",{0,1,0,0,0}};
    tRec r2 = {1600,"rachem","thabatarth","11/02/2006","azeffoun",{0,1,0,1,0}};
    header h;
    BUFFER buff;
    int found;
    long i;
    int j;
    TOF_OPEN(&P,"test.bin",'n');
    h = P->h;
    buff.NB = 0;
    buff.tab[buff.NB] = r2;
    buff.NB++;
    buff.tab[buff.NB] = r;
    buff.NB++;
    writeBlock(P,1,&buff);
    setHeader(P,1,1);
    setHeader(P,2,2);
    readBlock(P,1,&buff);
    tRec student = buff.tab[0];
    readStudent(student);
    student = buff.tab[1];
    readStudent(student);
    printf("le nombre de blocs est : %d \nle nombre d'enregistrements est : %d\nle nombre d'enregistrements supprimes est : %d\n",P->h.nBlock,P->h.nRec,P->h.nDelRec);
    binsearch(P, 11600, &found, &i, &j );
    if (found == 1)
    {
        printf("the record is found in the Block N : %d and it's the record N %d\n",i,j+1);
    }
    else
    {
        printf("the record is not found and it must be inserted in the Block N : %d in the position : %d\n",i,j+1);
    }
    TOF_close(P);
    return 0;
}


void binsearch(TOF* P, int val, int *found, long *i, int *j ){
   long low, up, counter;
   int stop, inf, sup;
   BUFFER buf;
   long block_in_MM = -1;

   low = 1;
   up = getHeader( P, 1 );
   *found = 0;
   stop = 0;
   counter = 0; // 'readBlock' counter

   // external search ...
   while ( low <= up && !*found && !stop ) {
	*i = (low + up) / 2;     // the middle between low and up
	readBlock( P, *i, &buf ); counter++; block_in_MM = *i; 	
	if ( val < buf.tab[0].ID )
	   up = *i - 1;		// search continues in the first half
	else
	   if ( val > buf.tab[buf.NB-1].ID && buf.NB >= b*ld )
	      	low = *i + 1;	// search continues in the last half
	   else {
		stop = 1;	 
	      	inf = 0;
		sup = buf.NB-1; 
                // internal search (inside buf) ...
		while ( inf <= sup && !*found ) {
		   *j = (inf + sup) / 2;
		   if ( val == buf.tab[*j].ID ) *found = 1;
		   else
			if ( val < buf.tab[*j].ID )
			   sup = *j - 1;
			else
			   inf = *j + 1;
	   	} // end while -- internal search
		if ( inf > sup ) *j = inf;
	   }
   } // end while -- external search
   if ( low > up ) {
	*i = low;
	*j = 0;
   }

   return ;

} // binsearch




