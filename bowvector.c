#include <stdlib.h>

#include "bowvector.h"

extern int totalVocabularyWords;
extern int finalVectorsSize;

int createVector(struct Spec* spec, int vsize)
{
    if (spec==NULL) return FAIL;

    // create vector - set all cells to 0
    spec->vector = calloc(vsize,sizeof(char));

    return SUCCESS;
}

int setVector(struct Spec* spec)
{
    // create vector
    if (createVector(spec,finalVectorsSize)==FAIL) return FAIL;

    int index = 0;
    int freq = 0;
    char word[30];
    
    for (int i=0; i<finalVectorsSize; ++i) {

        if (spec->tfidf[i] == (float) 1) 
            spec->vector[index] = (char) 1; // set indexed cell of vector to 1
    }

    return SUCCESS;


}


void removeWordsFromVector(struct Spec* spec, int* words_rm)
{
    int num_of_rm = sizeof(words_rm)/sizeof(int);
    int index = 0;

    // remove words from index by seting their value in vector to -1
    for (int i=0; i<num_of_rm; ++i)
    {
        index = words_rm[i];
        spec->vector[index] = (char) 2;
    }

    // create a new vector with size: prev_size-num_of_rm
    char* newv = calloc(totalVocabularyWords-num_of_rm,sizeof(char));

    for (int j=0,k=0; j<totalVocabularyWords; ++j)
    {
        if (spec->vector[j]==1) newv[++k] = (char) 1; //copy frequency and move to next

        else if (spec->vector[j]==0) ++k;   //move to next
    }

    free(spec->vector);     //delete old vector
    spec->vector = newv;    //and replace it with the new one

}







