//tfidf.c

#include "tfidf.h"
#include "readX.h"
#include "BOW.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int totalVocabularyWords;
extern int finalVectorsSize;
extern int num_of_specs;

//1. Calculation of tf value for each spec
//Calculates the tf values for every word in the spec
void init_tf(struct Spec* spec, struct VocabEntry *general_voc){

    //allocation of spec's tfidf table and initialization to 0
    //printf("Sizeof Vocabulary: %d\n", totalVocabularyWords);
    spec->tfidf = calloc(totalVocabularyWords, sizeof(float));
    spec->tf_size = totalVocabularyWords;

    struct VocabBucket* currentBucket;

    int pos=0;

    //for every word found in the spec
    for(int i = 0;i < LOC_ENTRIES;i++){
    	currentBucket = spec->local_vocab[i].bucket;
    	while(currentBucket != NULL){

      		for(int j = 0;j < currentBucket->isFull;j++){ //for every word

                //tf value = (freq of word in spec) / (num of words in spec)
                //find general position of word from global vocabulary

                pos = getWordPosition(general_voc, VOC_ENTRIES, VOC_BUCK_SIZE, currentBucket->words[j].word);
                //printf("Pos: %d\n", pos);
                if (pos == -1){
                    perror("Word not found in Global Vocabulary!\n");
                    return;
                }

                //store tf value in the corresponding position
                spec->tfidf[pos] = currentBucket->words[j].freq / (float)spec->num_of_words;
                //printf("spec->tfidf[pos]: %f, pos: %d\n", spec->tfidf[pos], pos);

            }

            currentBucket = currentBucket->nextBucket;
        }
    }


}






//2. Initialization of idf table
//creates the idf table, calculates idf value for each word
float *init_idf(int num_of_specs, struct VocabEntry* vocabulary,int numOfEntries){


    float *idf = calloc( totalVocabularyWords, sizeof(float));

    int count = 0;

    struct VocabBucket* currentBucket;

    //checkvoc(vocabulary);
    for(int i = 0;i < numOfEntries;i++){
    	currentBucket = vocabulary[i].bucket;
    	while(currentBucket != NULL){

                for(int j = 0;j < currentBucket->isFull;j++){

                    //printf("Entry: %d, place in bucket: %d/%d word: %s, pos: %d, freq: %d\n", i, j, VOC_BUCK_SIZE, currentBucket->words[j].word, currentBucket->words[j].num, currentBucket->words[j].freq);
                    //idf value = log( num of specs / num of specs containing the word )
                    //printf("num_of_specs: %d, freq: %d, res: %f\n", num_of_specs, currentBucket->words[j].freq, log10((float)num_of_specs/(float)currentBucket->words[j].freq ));

                    idf[currentBucket->words[j].num] = log10((float)num_of_specs/(float)currentBucket->words[j].freq );
                    count++;
                    //printf("Num: %d\n", currentBucket->words[j].num);
                    //printf("Entry: %d, place in bucket: %d/%d word: %s, pos: %d, freq: %d\n\n", i, j, VOC_BUCK_SIZE, currentBucket->words[j].word, currentBucket->words[j].num, currentBucket->words[j].freq);

                    //printf("idf[i]: %f, i: %d\n", idf[currentBucket->words[j].num], currentBucket->words[j].num);
            }

            currentBucket = currentBucket->nextBucket;
        }
    }

    //printf("Count: %d + totalvocabularywords: %d\n", count, totalVocabularyWords);

   return idf;
}




//Calculate average tf-idf values
float* calculate_tfidf_mean(struct Entry* hashTable, int numOfEntries, int bucketSize, float* idf){

    float *tfidf_mean = calloc(finalVectorsSize, sizeof(float));


    struct Bucket* currentBucket;
    int numOfBucket;
    for(int i = 0;i < numOfEntries;i++){
        numOfBucket = 1;
        currentBucket = hashTable[i].bucket;
        
        while(currentBucket != NULL){
            for(int j = 0;j < currentBucket->isFull;j++){
                printf("Entry: %d, Bucket: %d, place in bucket: %d/%d id: %s\n", i, numOfBucket, j, bucketSize, currentBucket->bucket_specs[j].spec->spec_id);


                for(int pos=0; pos<finalVectorsSize; pos++){
                    
                    if ( i < currentBucket->bucket_specs[j].spec->tf_size){
                        currentBucket->bucket_specs[j].spec->tfidf[pos] = currentBucket->bucket_specs[j].spec->tfidf[pos] * idf[pos];
                        tfidf_mean[pos] += currentBucket->bucket_specs[j].spec->tfidf[pos];
                    } else {
                        break;
                    }
                    

                }

            }
            currentBucket = currentBucket->nextBucket;
            numOfBucket++;
        }
    }

    
    for(int pos=0; pos<finalVectorsSize; pos++){
        tfidf_mean[pos] = tfidf_mean[pos]/num_of_specs;
    }
    
    return tfidf_mean;

}












/*
//3. Calculates final form of tfidf values for each spec (removing words with low tfidf)
void calculate_tfidf(struct Spec* spec, float* idf){        //takes the tf table and computes the tfidf value


    for (int i=0; i<totalVocabularyWords; i++){         //for every idf value in idf table
        
        if ( i < spec->tf_size){
            spec->tfidf[i] = spec->tfidf[i] * idf[i];
        } else {
            break;
        }
        
    }

}*/



















//3. Calculates final form of tfidf values for each spec (removing words with low idf)
void final_tfidf(struct Spec* spec, float *tfidf_mean){        //takes the tf table and computes the tfidf value

    float* tf_idf = calloc(finalVectorsSize, sizeof(float));        //final tfidf
    //printf("finalvoca: %d\n", finalVectorsSize);

    float* to_free = spec->tfidf;


    int i=0, j=0;
    int new_size = 0;


    for (i=0; i<totalVocabularyWords; i++){         //for every idf value in idf table
        //printf("idf[%d]: %f\n", 0, idf[i]);
        if (tfidf_mean[i]  > 1.3){   //keeping this word

            //if (i==14331) printf("spec->tidf[j]: %f, idf: %f\n", spec->tfidf[i], idf[i]);

            //printf("spec_>tf_size : %d\n", spec->tf_size);
            //tf_idf value = tf value * idf value
            if ( i < spec->tf_size){                //if we did not overpass the size of the spec's tf table

                tf_idf[j] = spec->tfidf[i];
                j++;
            }  //rest: overpassed the size of the spec->tfidf table, meaning no other word in idf was in this spec, remain 0

            new_size++;

        }


    }


/*

    if (finalVectorsSize == totalVocabularyWords){  //case: first time, we don't have yet the final size, malloc-ed with total size

        //clip extra positions

        tf_idf = realloc(tf_idf, new_size);    //keep first new_size positions
        if (tf_idf == NULL){
            perror("Request for reallocation failed\n");
            return;
        }


    }

*/

    spec->tf_size = new_size;

    //printf("spec->tf_size: %d & tf_idf size: %d\n", spec->tf_size, finalVectorsSize);

    //printf("We kept %d words after clipping idf\n", j);
    free(to_free);
    spec->tfidf = tf_idf;
    finalVectorsSize = new_size;

/*
    for (int m=0; m<finalVectorsSize; m++){
        printf("tf_idf: %f m:%d\n", tf_idf[m], m);
    }

    for (int m=0; m<totalVocabularyWords; m++){
        printf("idf: %f m:%d\n", idf[m], m);
    }

    for (int m=0; m<spec->tf_size; m++){
        printf("spec->tfidf: %f m:%d\n", spec->tfidf[m], m);
    }
  */

}




///////////////////////////////////////////////////////////////
