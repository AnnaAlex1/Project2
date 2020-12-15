//tfidf.h

#ifndef __TFIDF__
#define __TFIDF__

#include "readX.h"




struct Tfidf{
    double *spec_tfidf;     //one line of the tfidf table (each spec's tfidf)
    int sizeoftable;
    struct Tfidf* next_spec;
};


void tfidf_add(struct Tfidf **tfidf, double *spec_tf);
void spec_tf(struct Tfidf **tfidf, struct Spec* spec, double *idf, int vocab_size);     //calculates every tf value of each word
double *init_idf(int num_of_specs, struct Vocabulary vocabulary, int vocab_size);

void release_tfidf(struct Tfidf **tfidf);


#endif