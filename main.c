#include "abstract.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define b 100

long block_in_MM = -1;

void init_student(tRec* student);
void init_Buffer(BUFFER* buf);
tRec extractStudent(char* line);
void ins(TOF* P,tRec student, int factor);
int main(){

   float ld = 0.7;
   int factor = (int)(ld*b);
    FILE* f = fopen("students_data_1a.csv","r");
    TOF* P;
    BUFFER buff,buff1;
    long i;
    int j;
    char line[255], delimiters[] = ",";
    tRec student;

    TOF_OPEN(&P,"students_TOF.bin",'n');
    
    
    
    if (f == NULL)
    {
        printf("error while opening the file :((");
        return 0;
    }
    
  
   while(fgets(line,255,f) != NULL){
      student = extractStudent(line);
      ins(P,student,factor);
        
   }

   
   

   readBlock(P,2,&buff);
   for (int i = 0; i < buff.NB; i++)
   {
     readStudent(buff.tab[i]);
   }
   
    
   TOF_close(P);    
   return 0;
}



void init_student(tRec* student){
    for (int i = 0; i < sizeof((*student).missingValues)/sizeof(int); i++)
    {
        (*student).missingValues[i] = 0;
    }
    return;
}


tRec extractStudent(char* line){
   tRec student;
   int k = 0;
   char* token = partitions(line,k+1,',');
    init_student(&student);
    while(token != NULL || k < 5){
        switch (k)
        {
        case 0:
            if (token == NULL) student.missingValues[k] = 1;
            else
            {
                student.ID = atoi(token);
            }
                                
            break;
        case 1:
            if (token == NULL) student.missingValues[k] = 1;
            else
            {
                strcpy(student.firstName,token);
            }
            
            break;
        case 2:
            if (token == NULL) student.missingValues[k] = 1;
            else
            {
                strcpy(student.familyName,token);
            }
                      
            break;
        case 3:
            if (token == NULL) student.missingValues[k] = 1;
            else
            {
                strcpy(student.dateBirth,token);
            }    

            break;
        case 4:
            if (token == NULL) student.missingValues[k] = 1;
            else
            {
                strcpy(student.cityBirth,token);
            }

            break;
        }
        k = k+1;
        token = partitions(line,k+1,',');   
         
    }
    return student;
}
void binsearch(TOF* P, int val, int *found, long *i, int *j, int factor ){
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
	   if ( val > buf.tab[buf.NB-1].ID && buf.NB >= factor )
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


void ins(TOF* P, tRec E, int factor)
{
   tRec sauv;
   long i, SearchCost, InsCost;
   int found, stop, slotReuse, j;
   BUFFER buf;
   int val = E.ID;

   // locate the address (i,j) where to insert the new record...
   binsearch( P,val, &found, &i, &j,factor );

   // read block i (if necessary)
   readBlock( P , i , &buf );

//    printf("the search costs %ld reads.\n", SearchCost);
   if (found && buf.del[j] == 0) {
      // if the given record exists and is not logically deleted, do nothing
      printf("Insertion refused because the value already exists in block %ld at position %d\n",\
	      i, j);
      return;
   }

   // if the given record does not exist or is logically deleted, then insertion continues ... 
   fflush(stdout);

   InsCost = 0;	// read/write counter in the inter-block shifts phase 

   slotReuse = 0; // indicator of reuse of a slot containing a logically deleted record

   if ( i > getHeader( P, 1 ) ) {
      // special case of insertion at end of file ...
      init_Buffer(&buf);
      buf.tab[0] = E;
      buf.del[0] = 0;
      buf.NB = 1;
      writeBlock( P, i, &buf ); InsCost++;
      setHeader( P, 1, getHeader( P,1 ) + 1 );
   }
   else {
      // general case ...
      stop = 0;
      while ( !stop ) {	// main loop for inter-block shifts ...
	while ( !stop && j < buf.NB ) {	// internal loop for intra-block shifts ...
	   sauv = buf.tab[j];
	   buf.tab[j] = E;
	   if ( buf.del[j] != 0 ) {   // if slot j is unused (contains a delete record) 
		stop = 1;               // we use it and stop shift processing
		buf.del[j] = 0;
	   }	
	   else {                       // else, shifts continues to next slot (j+1)
	   	j++;
		E = sauv;
	   }
	} // end loop -- intra-block shifts

	if ( j == buf.NB ) 	// or stop == 0
	   if ( buf.NB < factor ) {
		buf.NB++;
		buf.tab[j] = E;  buf.del[j] = 0;
		writeBlock( P, i, &buf ); InsCost++;
		stop = 1;
	   }
	   else {
		writeBlock( P, i, &buf ); InsCost++;
		i++;  j = 0; 
		if ( i <= getHeader( P, 1 ) ) {
		   readBlock( P, i, &buf ); InsCost++; block_in_MM = i;
		}
		else {
      	      	   // we reach the end-of-file, so we append a new block ...
                     init_Buffer(&buf);
      	      	   buf.tab[0] = E;
      	      	   buf.del[0] = 0;
      	      	   buf.NB = 1;
      	      	   writeBlock( P, i, &buf ); InsCost++;
                   setHeader( P, 1, getHeader( P, 1 )+1 );
	      	   stop = 1; // to exit the main loop
		}
	   }
	else {
	   // The case where shifts processing has stopped on an unused slot (a logically deleted record)
	   slotReuse = 1; 
        setHeader( P, 3, getHeader(P, 3)-1 );  // because we reused the logically deleted slot
	    writeBlock( P, i, &buf ); InsCost++;
	}

      } // end of while (the main loop)

   } // end of else (general case)

   if ( !slotReuse )		// if we have not reused the slot of a deleted record,
      setHeader( P, 2, getHeader(P, 2)+1 ); // then nIns++

   // printf("\tCosts in read/write counts (search:%ld + shifts:%ld) = %ld\n", \
	// 	SearchCost, InsCost, SearchCost+InsCost);
   printf("End of insertion\n");

} // ins


void init_Buffer(BUFFER* buf){
   tRec r = {-1,"","","","",{0,0,0,0,0}};
   for (int i = 0; i < b; i++)
   {
      (*buf).tab[i] = r;
   }
   return;
}