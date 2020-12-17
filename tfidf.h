//tfidf.h


#ifndef __TFIDF__
#define __TFIDF__

#include "readX.h"
#include "BOW.h"



struct Rem_struct{
    int pos;
    struct Rem_struct* next;
};



void init_tf(struct Spec* spec, struct VocabEntry *general_voc,int numOfEntries);
float *init_idf(int num_of_specs, struct VocabEntry* vocabulary,int numOfEntries);
void calculate_tfidf(struct Spec* spec, float* idf);

#endif
