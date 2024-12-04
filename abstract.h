#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define b 10
typedef struct tRec{
    int ID;
    char firstName[20];
    char familyName[20];
    char dateBirth[10];
    char cityBirth[15];
    int missingValues[5];
}tRec;

typedef struct tBlock{
    tRec tab[b];
    int NB;
}tBlock,BUFFER;

typedef struct header{
    int nBlock;
    int nRec;
    int nDelRec;
}header;

typedef struct TOF{
    header h;
    FILE* f;
}TOF;




void TOF_OPEN(TOF** file,char* fileName, char mode){
    *file = malloc(sizeof(TOF));
    header h = {0,0,0};
    mode = tolower(mode);
    if (mode == 'n' )
    {
        (*file)->f = fopen(fileName,"wb+");
        if ((*file)->f == NULL)
        {
            printf("error while opening the file !");
            return;
        }
    
        (*file)->h = h;
        fwrite(&h,sizeof(header),1,(*file)->f);
    }
    else if(mode == 'e')
    {
        (*file)->f = fopen(fileName,"rb+");
        if ((*file)->f == NULL)
        {
            printf("error while opening the file !");
            return;
        }
        fread(&h,sizeof(header),1,(*file)->f);
        (*file)->h = h;
    }
    else {
        printf("Incorrect Mode ! please select either \"E\" or \"N\" ");
    }
    
}


void TOF_close( TOF *file ){
   fseek(file->f, 0, SEEK_SET);
   fwrite(&file->h, sizeof(header), 1, file->f);
   fclose(file->f);
   free(file);
}


void readBlock(TOF* file,int i,tBlock* buf){
    fseek(file->f,sizeof(header) + (i-1)*sizeof(tBlock),SEEK_SET);
    fread(buf,sizeof(tBlock),1,file->f);
}


void writeBlock(TOF* file,int i,tBlock* buf){
    fseek(file->f,sizeof(header) + (i-1)*sizeof(tBlock),SEEK_SET);
    fwrite(buf,sizeof(tBlock),1,file->f);
}


void setHeader(TOF* file,int i,int j){
    switch (i)
    {
    case 1:
        file->h.nBlock = j;
        break;
    case 2:
        file->h.nRec = j;
        break;
    case 3:
        file->h.nDelRec = j;
        break;
    
    default:
    printf("the second parameter must be in the range [1-3]");
        break;
    }
}


int getHeader(TOF* file,int i){
    switch (i)
    {
    case 1:
        return file->h.nBlock ;
        break;
    case 2:
        return file->h.nRec;
        break;
    case 3:
        return file->h.nDelRec;
        break;
    
    default:
        printf("the second parameter must be in the range [1-3]");
        return -1;  
        break;
    }
}


char* partitions( char* phrase, int part, char delimiter) {
    if (phrase == NULL || part < 1) {
        return NULL;
    }
    
    int i = 1; // Track the current partition
    char* start = phrase; // Pointer to the start of a part
    char* end = NULL; // Pointer to the end of a part

    while (*phrase) {
        if (*phrase == delimiter || *phrase == '\0') {
            if (i == part) {
                end = phrase; // Mark the end of the desired part
                break;
            }
            i++;
            start = phrase + 1; // Move start to the next part
        }
        phrase++;
    }

    // Handle the case where the desired part is at the end
    if (i == part && end == NULL) {
        end = phrase;
    }

    if (i != part || start == end) {
        return NULL; // Part not found or empty part
    }

    // Allocate memory for the result
    size_t length = end - start;
    char* result = malloc(length + 1);
    if (result == NULL) {
        return NULL; // Memory allocation failure
    }

    // Copy the part into the result
    strncpy(result, start, length);
    result[length] = '\0'; // Null-terminate the string

    return result;
}

// long binsearch( t_rec val, int *found, long *i, int *j )
// {
//    long low, up, counter;
//    int stop, inf, sup;

//    low = 1;
//    up = getHeader( f, "nBlock" );
//    *found = 0;
//    stop = 0;
//    counter = 0; // 'readBlock' counter

//    // external search ...
//    while ( low <= up && !*found && !stop ) {
// 	*i = (low + up) / 2;     // the middle between low and up
// 	TOF_readBlock( f, *i, &buf ); counter++; block_in_MM = *i; 	
// 	if ( val < buf.tab[0] )
// 	   up = *i - 1;		// search continues in the first half
// 	else
// 	   if ( val > buf.tab[ buf.nb-1 ] )
// 	      	low = *i + 1;	// search continues in the last half
// 	   else {
// 		stop = 1;	 
// 	      	inf = 0;
// 		sup = buf.nb-1; 
//                 // internal search (inside buf) ...
// 		while ( inf <= sup && !*found ) {
// 		   *j = (inf + sup) / 2;
// 		   if ( val == buf.tab[*j] ) *found = 1;
// 		   else
// 			if ( val < buf.tab[*j] )
// 			   sup = *j - 1;
// 			else
// 			   inf = *j + 1;
// 	   	} // end while -- internal search
// 		if ( inf > sup ) *j = inf;
// 	   }
//    } // end while -- external search
//    if ( low > up ) {
// 	*i = low;
// 	*j = 0;
//    }

//    return counter;

// } // binsearch