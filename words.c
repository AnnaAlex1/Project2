#include <stdio.h>
#include <string.h>

#include "words.h"


void lower (char* word)
{
    for (int i=0; word[i]!='\0'; ++i)
    {
        if (word[i]>='A' && word[i]<='Z')
        {
            word[i] = word[i] + 32;
        }
    }
    return;
}

int isStopword (char* word, FILE* file)
{
    
    char c; int i=0;
    char stopword[20];
    while(1) 
    {
        c = fgetc(file);
            
        if ((c==',') || (c==EOF)) //end of stopword or file --> we have just collected a stopword
        {
            stopword[i] = '\0'; i=0;
            if (strcmp(word,stopword)==0) return TRUE;  //word matches to stopword
            else if (c==EOF) return FALSE; //end of file - no matching found

        } else stopword[i++] = c;

    
    }

    
}