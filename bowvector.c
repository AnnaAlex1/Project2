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
    //if (createVector(spec,finalVectorsSize)==FAIL) return FAIL;

    for (int i=0; i<finalVectorsSize; ++i) {
        //printf("spec->tfidf[i]: %f, pos: %d\n", spec->tfidf[i], i);

        if (spec->vector[i] != (float) 0.0)
            spec->vector[i] = (float)1.0; // set indexed cell of vector to 1

    }

    return SUCCESS;


}
