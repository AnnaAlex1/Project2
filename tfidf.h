//tfidf.h


#ifndef __TFIDF__
#define __TFIDF__

#include "readX.h"
#include "BOW.h"



struct Rem_struct{
    int pos;
    struct Rem_struct* next;
};



void init_tf(struct Spec* spec, struct VocabEntry *general_voc);
float *init_idf(int num_of_specs, struct VocabEntry* vocabulary,int numOfEntries);
float* calculate_tfidf_mean(struct Entry* hashTable, int numOfEntries, int bucketSize, float* idf);
int* clip_vectorsize(float* tfidf_mean);
void final_tfidf(struct Spec* spec, int *positions);


#endif