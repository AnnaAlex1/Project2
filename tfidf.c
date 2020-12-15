//tfidf.c

#include "tfidf.h"
#include "readX.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


//takes the BOW of the spec and calculates the tf values for every word in the spec
void spec_tf(struct Tfidf **tfidf, struct Spec* spec, double *idf, int cur_vocab_size, double *tfidf_column_sum){  //create tfidf row for spec

    double *spec_tf = malloc( sizeof(double) * cur_vocab_size);     //table of tfidf numbers


    for (int i=0; i<cur_vocab_size; i++){       //for every word in the vocabulary

        if (spec->bow[i] != 0){     //if the word exists in the spec
            spec_tf[i] = spec->bow[i] / (double)spec->num_of_words;
            //spec_tf[i] = spec_tf[i] * idf[i];                         //OXI EDW GIATI XREIAZETAI NA EXEI HDH DIAVASEI OLA TA SPEC
        } else {
            spec_tf[i] = 0;
        }
        

    }

    tfidf_add( tfidf, spec_tf, cur_vocab_size);

}




//adds the table of tf values of the spec to the general list of tf values
void tfidf_add(struct Tfidf **tfidf, double *spec_tf, int cur_vocab_size){          //adding a table of tfidf values of spec to the list

    struct Tfidf *new = malloc( sizeof(struct Tfidf)); 
    new->spec_tfidf = spec_tf;
    new->sizeoftable = cur_vocab_size;


    if (tfidf == NULL){  //list is empty - add first node
        *list = new;
    } else {             //add node at the beginning of the list
        new->next_spec = *list;
        *list = new;
    }


}




//creates the idf table, calculates idf value for each word
double *init_idf(int num_of_specs, struct Vocabulary vocabulary, int vocab_size){

    double *idf = malloc( sizeof(double) * vocab_size);

    
    for (int i=0; i<vocab_size; i++){

        idf[i] = log( num_of_specs/(double)vocabulary[i].frequency );


    }

    return idf;

}



//calculates final form of tfidf table
double* calculate_tfidf(struct Tfidf *tfidf, double *idf, int vocab_size){
                                                //cur_voacb_size == idf size
   
    double *column_sum;
    column_sum = malloc( sizeof(double) * vocab_size);      //maybe calloc
    for (int i = 0; i < vocab_size; i++){
        column_sum[i] = 0;
    }
    

    int sizeoftable;
    double *row;

    int i = 0;

    while (tfidf != NULL){  //for each row of the tfidf table

        row = tfidf->spec_tfidf;
        sizeoftable = tfidf->sizeoftable;

        for (int j=0; j < vocab_size; j++){         //size of the final vocabulary

            if ( j < sizeoftable){          //if there are still values in the tf table of size sizeoftable
                
                row[j] = row[j] * idf[j];                   //EXITS TABLE'S MEMORY BOUNDARIES
                sum[j] = sum[j] + row[j];

            } else {
                row[j] = 0; 
            }


        }

        tfidf = tfidf->next_spec;

    }


}



void words_to_remove(double* sum, int vocab_size){

    for (int i=0; i<vocab_size; i++){
        if (sum<=0.005){
            //call funtion for removing
        }
    }

    //maybe sorting and then take first 1000

}



//releases tfidf table
void release_tfidf(struct Tfidf **tfidf){

    struct Tfidf *to_free;

    while (tfidf != NULL){

        to_free = *tfidf;
        free(to_free->spec_tfidf);
        *tfidf = (*tfidf)->next_id;
        free(to_free);

    }


}




//eimaste stin tfidf
//for all tfidf
    //for every first element of every row (hold them in a table)
        //sum
    //average (sum/num_of_specs)

