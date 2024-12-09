#include "abstract.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define b 100

long block_in_MM = -1;

void init_student(tRec* student);
// void init_Buffer(BUFFER* buf);
void del(TOF* P, int val,int factor, long* SearchCost);
tRec extractStudent(char* line);
void ins(TOF* P,tRec student, int factor, long *SearchCost, long *InsCost);
int main(){

   float ld = 0.7;
   int factor = (int)(ld*b + 0.5);
    FILE* f = fopen("students_data_1a.csv","r");    
    FILE* f2 = fopen("TOFInsertionReadWrites.txt","w+");    
    FILE* f3 = fopen("TOFDeletionReadWrites.txt","w+");    
    FILE* file = fopen("delete_students.csv","r");
    TOF* P;
    BUFFER buff,buff1;
    long i;
    long TOFReads,TOFWrites;
    int j;
    int cpt1=0,cpt2=0;
    char line[255],lineID[255], delimiters[] = ",";
    tRec student;

    TOF_OPEN(&P,"students_TOF.bin",'n');
    
    
    
    if (f == NULL)
    {
        printf("error while opening the first file :((");
        return 0;
    }
    if (file == NULL)
    {
        printf("error while opening the \"IDs to delete\" file :((");
        return 0;
    }
    if (file == NULL)
    {
        printf("error while opening the \"reads and writes\" file :((");
        return 0;
    }
    
   fprintf(f2,"-------------  TOF INSERTIONS  -------------\n");
   while(fgets(line,255,f) != NULL){
      student = extractStudent(line);
      ins(P,student,factor,&TOFReads,&TOFWrites);
      fprintf(f2,"--- %ld READS          |          %ld WRITES \n",TOFReads,TOFWrites);
        
   }
   fprintf(f3,"-------------  TOF DELETIONS  -------------\n");
   while(fgets(lineID,255,file) != NULL){
      int id = atoi(lineID);
      del(P,id,factor,&TOFReads);
      fprintf(f3,"--- %ld READS          |          1 WRITE \n",TOFReads);
   }

   

   // readBlock(P,143,&buff);
   // for (int i = 0; i < buff.NB; i++)
   // {
   //   readStudent(buff.tab[i]);
   // }
   

   printf("the number of blocks is %d and the number of records is %d and the number of deleted records is %d",getHeader(P,1),getHeader(P,2),getHeader(P,3));
   
   fclosef(f);
   fclosef(f2);
   fclosef(f3);
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
long binsearch(TOF* P, int val, int *found, long *i, int *j, int factor ){
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

   return counter;

} // binsearch


void ins(TOF* P, tRec E, int factor, long* SearchCost, long* InsCost)
{
   tRec sauv;
   long i;
   int found, stop, slotReuse, j;
   BUFFER buf;
   int val = E.ID;

   // locate the address (i,j) where to insert the new record...
   *SearchCost = binsearch( P,val, &found, &i, &j,factor );

   // read block i (if necessary)
   readBlock( P , i , &buf );

//    printf("the search costs %ld reads.\n", SearchCost);
   if (found && buf.del[j] == ' ') {
      // if the given record exists and is not logically deleted, do nothing
      printf("Insertion refused because the value already exists in block %ld at position %d\n",\
	      i, j);
      return;
   }

   // if the given record does not exist or is logically deleted, then insertion continues ... 
   fflush(stdout);

   *InsCost = 0;	// read/write counter in the inter-block shifts phase 

   slotReuse = 0; // indicator of reuse of a slot containing a logically deleted record

   if ( i > getHeader( P, 1 ) ) {
      // special case of insertion at end of file ...
      buf.tab[0] = E;
      buf.del[0] = ' ';
      buf.NB = 1;
      writeBlock( P, i, &buf ); (*InsCost)++;
      setHeader( P, 1, getHeader( P,1 ) + 1 );
   }
   else {
      // general case ...
      stop = 0;
      while ( !stop ) {	// main loop for inter-block shifts ...
	while ( !stop && j < buf.NB ) {	// internal loop for intra-block shifts ...
	   sauv = buf.tab[j];
	   buf.tab[j] = E;
	   if ( buf.del[j] == '*' ) {   // if slot j is unused (contains a delete record) 
		stop = 1;               // we use it and stop shift processing
		buf.del[j] = ' ';
	   }	
	   else {                       // else, shifts continues to next slot (j+1)
	   	j++;
		E = sauv;
	   }
	} // end loop -- intra-block shifts

	if ( j == buf.NB ) 	// or stop == 0
	   if ( buf.NB < factor ){
		buf.NB++;
		buf.tab[j] = E;  buf.del[j] = ' ';
		writeBlock( P, i, &buf ); (*InsCost)++;
		stop = 1;
	   }
	   else {
		writeBlock( P, i, &buf ); (*InsCost)++;
		i++;  j = 0; 
		if ( i <= getHeader( P, 1 ) ) {
		   readBlock( P, i, &buf ); (*InsCost)++; block_in_MM = i;
		}
		else {
      	      	   // we reach the end-of-file, so we append a new block ...
      	      	   buf.tab[0] = E;
      	      	   buf.del[0] = ' ';
      	      	   buf.NB = 1;
      	      	   writeBlock( P, i, &buf ); (*InsCost)++;
                   setHeader( P, 1, getHeader( P, 1 )+1 );
	      	   stop = 1; // to exit the main loop
		}
	   }
	else {
	   // The case where shifts processing has stopped on an unused slot (a logically deleted record)
	   slotReuse = 1; 
        setHeader( P, 3, getHeader(P, 3)-1 );  // because we reused the logically deleted slot
	    writeBlock( P, i, &buf ); (*InsCost)++;
	}

      } // end of while (the main loop)

   } // end of else (general case)

   if ( !slotReuse )		// if we have not reused the slot of a deleted record,
      setHeader( P, 2, getHeader(P, 2)+1 ); // then nIns++

   // printf("\tCosts in read/write counts (search:%ld + shifts:%ld) = %ld\n", \
	// 	SearchCost, InsCost, SearchCost+InsCost);
   printf("End of insertion \n");

} // ins

void del(TOF* P, int val,int factor, long* SearchCost)
{
   long i;
   int found, j;
   BUFFER buf;

   *SearchCost = binsearch( P, val, &found, &i, &j ,factor);

   printf("The cost of the search phase %ld reads.\n", SearchCost);
   readBlock(P,i,&buf);
   if (!found || buf.del[j] == '*') {
      printf("Deletion aborted because the record in not found\n");
      return;
   }

   printf("The record to be deleted is in block %ld at position %d\n", i, j);

   buf.del[j] = '*';		// mark the slot j as 'deleted'
   writeBlock( P, i, &buf );

   setHeader( P, 3, getHeader(P, 3)+1 );


} // del


// void init_Buffer(BUFFER* buf){
//    tRec r = {-1,"","","","",{0,0,0,0,0}};
//    for (int i = 0; i < b; i++)
//    {
//       (*buf).tab[i] = r;
//    }
//    return;
// }