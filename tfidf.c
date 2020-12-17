//tfidf.c

#include "tfidf.h"
#include "readX.h"
#include "BOW.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int totalVocabularyWords;
extern int finalVectorsSize;

//1. Calculation of tf value for each spec
//Calculates the tf values for every word in the spec
void init_tf(struct Spec* spec, struct VocabEntry *general_voc,int numOfEntries){

    //allocation of spec's tfidf table and initialization to 0
    spec->tfidf = calloc(totalVocabularyWords, sizeof(float));

    struct VocabBucket* currentBucket;

    int pos=0;

    //for every word found in the spec
    for(int i = 0;i < numOfEntries;i++){
    	currentBucket = spec->local_vocab[i].bucket;
    	while(currentBucket != NULL){
      		for(int j = 0;j < currentBucket->isFull;j++){ //for every word

                //tf value = (freq of word in spec) / (num of words in spec)

                //find general position of word from global vocabulary
                pos = getWordPosition(general_voc, numOfEntries, VOC_BUCK_SIZE, currentBucket->words[j].word);
                if (pos == -1){
                    perror("Word not found in Global Vocabulary!\n");
                    return;
                }

                //store tf value in the corresponding position
                spec->tfidf[pos] = currentBucket->words[j].freq / (float)spec->num_of_words; 

            }
            currentBucket = currentBucket->nextBucket;
        }
    }


}






//2. Initialization of idf table
//creates the idf table, calculates idf value for each word
float *init_idf(int num_of_specs, struct VocabEntry* vocabulary,int numOfEntries){

    float *idf = malloc( sizeof(float) * totalVocabularyWords);

    struct VocabBucket* currentBucket;

    for(int i = 0;i < numOfEntries;i++){
    	currentBucket = vocabulary[i].bucket;
    	while(currentBucket != NULL){
      		for(int j = 0;j < currentBucket->isFull;j++){
                //idf value = log( num of specs / num of specs containing the word )
			    idf[currentBucket->words[j].num] = log((float)num_of_specs/(float)currentBucket->words[j].freq );
            }
            currentBucket = currentBucket->nextBucket;
        }
    }
   return idf;
}



//3. Calculates final form of tfidf values for each spec (removing words with low idf)
void calculate_tfidf(struct Spec* spec, float* idf){        //takes the tf table and computes the tfidf value

    float* tf_idf = calloc(finalVectorsSize, sizeof(float));        //final tfidf
          

    float* to_free = spec->tfidf;

    int i=0, j=0;
    
    for (i=0; i<totalVocabularyWords; i++){         //for every idf value in idf table

        if (idf[i]  > 0.005){   //keeping this word


            //tf_idf value = tf value * idf value
            if ( i < spec->tf_size){                //if we did not overpass the size of the spec's tf table

                tf_idf[j] = spec->tfidf[i] * idf[i];

            } //rest: overpassed the size of the spec->tfidf table, meaning no other word in idf was in this spec, remain 0

            j++;

        } 


    }

    j--;

    if (finalVectorsSize == totalVocabularyWords){  //case: first time, we don't have yet the final size, malloc-ed with total size
        
        //clip extra positions
        tf_idf = realloc(tf_idf, j);    //keep first j positions
        if (tf_idf == NULL){
            perror("Request for reallocation failed\n");
            return;
        }

    }

   


    printf("We kept %d words after clipping idf\n", j);
    free(to_free);
    spec->tfidf = tf_idf;

    spec->tf_size = j;
    finalVectorsSize = j;


} 




///////////////////////////////////////////////////////////////

