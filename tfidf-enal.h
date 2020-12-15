//tfidf-enal.h


#ifndef __TFIDF__
#define __TFIDF__

#include "readX.h"


/*
spec{
    struct Tfidf *tfidf;
    int size_of_tf;
}
*/

struct Tfidf{
    double value;
    struct Tfidf next;
}


void init_tf(struct Spec* spec, double *idf, int vocab_size);


#endif