#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<stdbool.h>
#define b 10
typedef struct tRec{
    int ID;
    char firstName[20];
    char familyName[20];
    char dateBirth[15];
    char cityBirth[15];
    int missingValues[5];
}tRec;

typedef struct tBlock{
    tRec tab[b];
    int del[b];
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




char* partitions(char* phrase, int part, char delimiter) {
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

    // Trim leading and trailing spaces
    while (start < end && isspace((unsigned char)*start)) {
        start++;
    }
    while (end > start && isspace((unsigned char)*(end - 1))) {
        end--;
    }

    // Check if the trimmed part is empty
    if (start == end) {
        return NULL; // Part is only spaces or empty
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



void readStudent(tRec student){
    char fields[5][15] = {"ID","firstname","lastName","date of birth","city of birth"};
    bool missing = false;
    for (int i = 0; i < 5; i++)
    {
        if (student.missingValues[i] == 1)
        {
            missing = true;
            printf("- The %s is missing\n",fields[i]);
        } else {
            switch (i)
            {
            case 0:
                printf("- The ID is : %d\n",student.ID);
                break;
            case 1:
                printf("- The firstname is : %s\n",student.firstName);
                break;
            case 2:
                printf("- The lastname is : %s\n",student.familyName);
                break;
            case 3:
                printf("- The date of birth is : %s\n",student.dateBirth);
                break;
            case 4:
                printf("- The city of birth is : %s\n",student.cityBirth);
                break;
            }
        }
        
    }
    if (!missing)
    {
        printf("--- There is no missing values --- \n");
    }
    
    
}

