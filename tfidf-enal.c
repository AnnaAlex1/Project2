//tfidf.c enallaktiko


#include "tfidf-enal.h"
#include "readX.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//1. Calculation of tf value for each spec
//takes the BOW of the spec and calculates the tf values for every word in the spec
void init_tf(struct Spec* spec, double *idf, int vocab_size){

    spec->tfidf = malloc( sizeof(double) * vocab_size);

    for (int i=0; i<vocab_size; i++){

        if (spec->bow[i] != 0){     //if the word exists in the spec
            spec->tfidf[i] = spec->bow[i] / (double)spec->num_of_words;             //NOT BAG OF WORDS
        } else {
            spec->tfidf[i] = 0;
        }
        


    }


}


//2. Initialization of idf table
//creates the idf table, calculates idf value for each word
double *init_idf(int num_of_specs, struct Vocabulary vocabulary, int vocab_size){

    double *idf = malloc( sizeof(double) * vocab_size);

    for (int i=0; i<vocab_size; i++){

        idf[i] = log( num_of_specs/(double)vocabulary[i].frequency );

    }

    return idf;
}




//3. Calculates final form of tfidf values for each spec
void calculate_tfidf(struct Spec spec,double* idf, int vocab_size){        //takes the tf table and computes the tfidf value

    double *new_tfidf = malloc(sizeof(double)*vocab_size);

    cur_vocab_size = spec.tf_size;


    for (int i=0; i<vocab_size; i++){           //for every word in vocabulary

        if (i < spec.tf_size){                  //if we are inside spec's tf boundaries
                
            new_tfidf[j] = new_tfidf[j] * idf[j];

        } else {            //otherwise: now the vocabulary's words are not in this spec -> rest set to zeros
            row[j] = 0; 
        }


    }

    free(spec.tfidf);
    spec.tfidf = new_tfidf;

} 


//4. Check idf values, Create a table with the words to remove



//5.Remove the words with low idf from tfidf 
