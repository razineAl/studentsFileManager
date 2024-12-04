#include "abstract.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#define b 10

int main(){
    // TOF** P;
    // TOF_OPEN(P,"students_TOF",'n');
    FILE* f = fopen("students_data_1a.csv","r");
    char line[255], delimiters[] = ",";
    tRec student;
    
    
    if (f == NULL)
    {
        printf("error while opening the file :((");
        return 0;
    }
    
        
        
        
    while(fgets(line,255,f) != NULL){
    int j = 0;
    char* token = partitions(line,j+1,',');
    for (int i = 0; i < sizeof(student.missingValues)/sizeof(int); i++)
    {
        student.missingValues[i] = 0;
    }
    while(token != NULL || j < 5){
        switch (j)
        {
        case 0:
            if (token == NULL) student.missingValues[j] = 1;
            else
            {
                student.ID = atoi(token);
            }
                                
            break;
        case 1:
            if (token == NULL) student.missingValues[j] = 1;
            else
            {
                strcpy(student.firstName,token);
            }
            
            break;
        case 2:
            if (token == NULL) student.missingValues[j] = 1;
            else
            {
                strcpy(student.familyName,token);
            }
                      
            break;
        case 3:
            if (token == NULL) student.missingValues[j] = 1;
            else
            {
                strcpy(student.dateBirth,token);
            }
            
            break;
        case 4:
            if (token == NULL) student.missingValues[j] = 1;
            else
            {
                strcpy(student.cityBirth,token);
            }

            break;
        }
        j = j+1;
        token = partitions(line,j+1,',');      
    }
        if (student.missingValues[1] == 0)
        {
            printf("%s\n",student.firstName);
        } else {
            printf("missing value !!\n");
        }
        
        
    }
    
    return 0;
}





