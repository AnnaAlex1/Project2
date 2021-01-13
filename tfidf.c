//tfidf.c

#include "tfidf.h"
#include "readX.h"
#include "BOW.h"
#include "quicksort.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

extern int totalVocabularyWords;
extern int finalVectorsSize;
extern int num_of_specs;

//1. Calculation of tf value for each spec
//Calculates the tf values for every word in the spec
void init_tf(struct Spec* spec, struct VocabEntry *general_voc){

    //allocation of spec's tfidf table and initialization to 0
    //printf("Sizeof Vocabulary: %d\n", totalVocabularyWords);
    spec->vector = calloc(totalVocabularyWords, sizeof(float));
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
                spec->vector[pos] = currentBucket->words[j].freq / (float)spec->num_of_words;
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

                    idf[currentBucket->words[j].num] = log10((float)num_of_specs/(float)currentBucket->words[j].freq );
                    count++;
                    //printf("idf[i]: %f, i: %d\n", idf[currentBucket->words[j].num], currentBucket->words[j].num);
            }

            currentBucket = currentBucket->nextBucket;
        }
    }

    //printf("Count: %d + totalvocabularywords: %d\n", count, totalVocabularyWords);

   return idf;
}


/*

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

                for(int pos=0; pos<currentBucket->bucket_specs[j].spec->tf_size; pos++){
                    
                    currentBucket->bucket_specs[j].spec->vector[pos] = currentBucket->bucket_specs[j].spec->vector[pos] * idf[pos];
                    tfidf_mean[pos] += currentBucket->bucket_specs[j].spec->vector[pos];

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



//3. Calculates final form of tfidf vectors for each spec (removing words with low tf-idf average)
void final_tfidf(struct Spec* spec, float *tfidf_mean){        //takes the tf table and computes the tfidf value

    finalVectorsSize = 1000;
    
    float* tf_idf = calloc(finalVectorsSize, sizeof(float));        //final tfidf
    //printf("finalvoca: %d\n", finalVectorsSize);

    float* to_free = spec->vector;


    int i=0, j=0;
    


    for (i=0; i<totalVocabularyWords; i++){         //for every idf value in idf table
        //printf("tfidf mean: %f\n", tfidf_mean[i]);
        if ( (tfidf_mean[i]*1000)  > (float)0.19){   //keeping this word
            

            if ( i < spec->tf_size){                //if we did not overpass the size of the spec's tf table

                tf_idf[j] = spec->vector[i];
                j++;


            }  //rest: overpassed the size of the spec->tfidf table, meaning no other word in idf was in this spec, remain 0

            if (j == finalVectorsSize-1){      //if  1000 words were accepted, stop
                break;
            }

        }


    }


    spec->tf_size = finalVectorsSize;

    //printf("spec->tf_size: %d & tf_idf size: %d\n", spec->tf_size, finalVectorsSize);

    //printf("We kept %d words after clipping idf\n", j);
    free(to_free);
    spec->vector = tf_idf;


}

*/



//Calculate average tf-idf values
float* calculate_tfidf_mean(struct Entry* hashTable, int numOfEntries, int bucketSize, float* idf){

    float *tfidf_mean = calloc(finalVectorsSize, sizeof(float));    //allocate array of mean values


    struct Bucket* currentBucket;
    int numOfBucket;

    for(int i = 0;i < numOfEntries;i++){            //for every spec in hashtable
        numOfBucket = 1;
        currentBucket = hashTable[i].bucket;
        
        while(currentBucket != NULL){
            for(int j = 0;j < currentBucket->isFull;j++){

                for(int pos=0; pos<currentBucket->bucket_specs[j].spec->tf_size; pos++){    //for all values of the vector
                    
                    //calculate tfidf values
                    currentBucket->bucket_specs[j].spec->vector[pos] = currentBucket->bucket_specs[j].spec->vector[pos] * idf[pos];
                    
                    //keep addition for every position
                    tfidf_mean[pos] += currentBucket->bucket_specs[j].spec->vector[pos];

                }

            }
            currentBucket = currentBucket->nextBucket;
            numOfBucket++;
        }
    }

    
    for(int pos=0; pos<finalVectorsSize; pos++){    //divide by the  number of the specs
        tfidf_mean[pos] = tfidf_mean[pos]/num_of_specs;
    }
    
    return tfidf_mean;

}


int* clip_vectorsize(float* tfidf_mean){  //return an array with the positions of the words to keep

    finalVectorsSize = 1000;        //number of words to keep

    int pos_array[totalVocabularyWords];  //array with all positions

/*    printf("Initial Array: \n");
    for (int i=0; i<totalVocabularyWords; i++){
        printf("%f  ", array[i]);
    }
    printf("\n\n");
*/

    //enter part of array to be sorted, position of 1st element, position of last element
    quicksort(tfidf_mean, pos_array, 0, totalVocabularyWords-1);     //sorting the whole array


    //print final array
/*    printf("Final Array:\n");
    for (int i=0; i<totalVocabularyWords; i++){
        printf("%f  ", tfidf_mean[i]);
    }
    printf("\n");


    printf("\nPosition Array:\n");
    for (int i=0; i<totalVocabularyWords; i++){
        printf("%d  ", pos_array[i]);
    }
    printf("\n");
*/

    //quicksort again for positions
    int *final_pos = malloc(sizeof(int)*finalVectorsSize);        //array of the final size
    memcpy(final_pos, pos_array, sizeof(int)*finalVectorsSize);

/*    printf("\nAfter memcpy:\n");
    for (int i=0; i<finalVectorsSize; i++){
        printf("%d  ", final_pos[i]);
    }
    printf("\n");
*/


 //   quicksort2(final_pos, 0, finalVectorsSize-1);

    
/*
    printf("\nFinal Position Array:\n");
    for (int i=0; i<finalVectorsSize; i++){
        printf("%d  ", final_pos[i]);
    }
    printf("\n");
*/

    return final_pos;

}





//3. Calculates final form of tfidf vectors for each spec (keeping the 1000 words with the highest average tfidf)
void final_tfidf(struct Spec* spec, int *positions){        //takes the tf table and computes the tfidf value

    //final tf_idf vector for this spec
    float* tf_idf = calloc(finalVectorsSize, sizeof(float)); 


    int i, j=0;
    int pos;

    for (i=0; i<finalVectorsSize; i++){         //for every word to keep
           
        pos = positions[i];
        
        if ( pos < spec->tf_size){  //if the word was found in this spec (otherwise it stays zero anyway)

            tf_idf[j] = spec->vector[pos];
            j++;

        }


    }


    spec->tf_size = finalVectorsSize;

    //printf("spec->tf_size: %d & tf_idf size: %d\n", spec->tf_size, finalVectorsSize);

    //printf("We kept %d words after clipping idf\n", j);
    free(spec->vector);
    spec->vector = tf_idf;


}
