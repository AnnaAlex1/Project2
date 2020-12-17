#ifndef __VECTOR__
#define __VECTOR__

#include "readX.h"
#include "BOW.h"


#define SUCCESS 1
#define FAIL 0


int createVector(struct Spec* spec, int vsize);
int setVector(struct Spec* spec);
void removeWordsFromVector(struct Spec* spec, int* words_rm);


#endif
