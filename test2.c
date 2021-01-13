//test2.c

#include "acutest.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"
#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"
#include "BOW.h"
#include "words.h"
#include "tfidf.h"
#include "bowvector.h"
#include "logistic.h"


int last_update ;
extern int epochs;
extern float b;
extern float a;

#define LOC_ENTRIES 10
#define ENTRIES 20
#define BUCKETSIZE 100

extern int totalVocabularyWords;
int finalVectorsSize = 3;
int num_of_specs = 0;
struct AccessCliques* aclist = NULL;

//#define NUM_OF_ENTRIES 5
//#define BUCKET_SIZE 100

//int finalVectorsSize;


void test_setVector()		//checking if it sets vector of spec correctly
{
	FILE *file, *stop_file;
    file = fopen("./1111.json", "r");
    stop_file = fopen("./common-english-words.txt","r");

    char dir_name[20] = "www.nelly.com";
    char file_name[20] = "1111.json";


    finalVectorsSize = 4;

    struct VocabEntry *voc = malloc(sizeof(struct VocabEntry)*VOC_ENTRIES);
    initializeVocabHashTable(voc,VOC_ENTRIES,VOC_BUCK_SIZE);

    struct Spec spec;
    set_spec(&spec,file,dir_name,file_name, voc, stop_file,VOC_ENTRIES,VOC_BUCK_SIZE);

    setVector(&spec);

		for(int i=0;i<finalVectorsSize;i++){
			TEST_ASSERT( spec.vector[i] == (float)1.0 );
		}

    free_spec(&spec);
		freeVocabulary(voc,VOC_ENTRIES,VOC_BUCK_SIZE);
		free(voc);
    fclose(file);
    fclose(stop_file);
}


void test_hashFunction2(void){

    char word1[7] = "camera";

    int hash1 =  hashFunction2(LOC_ENTRIES, word1);

    TEST_ASSERT(hash1 == (((99+97)*99)%LOC_ENTRIES));

    char word2[2] = "k";

    int hash2 =  hashFunction2(LOC_ENTRIES, word2);

    TEST_ASSERT(hash2 == (10%LOC_ENTRIES));

    char word3[2] = "2";

    int hash3 =  hashFunction2(LOC_ENTRIES, word3);

    TEST_ASSERT(hash3 == (2%LOC_ENTRIES));

}



void test_searchSimilar(void){
    struct Entry* HashTable;
    HashTable = malloc( sizeof(struct Entry) * ENTRIES);
    initializeHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1

    //FIRST ELEMENT
    struct Spec spec;

    spec.spec_id = malloc( strlen("www.maria.com//1234") + 1 );
    strcpy(spec.spec_id,"www.maria.com//1234" );

    int pos = hashFunction(ENTRIES, spec.spec_id);
    insertInHashTable(&spec, ENTRIES, HashTable, 1);     //inserting first element

    //SECOND ELEMENT ( create chain of buckets (overflow of a bucket) )
    struct Spec spec2;
    spec2.spec_id = malloc( strlen("www.maria.com//1333") + 1 );
    strcpy(spec2.spec_id,"www.maria.com//1333" );

    int pos2 = hashFunction(ENTRIES, spec2.spec_id);
    insertInHashTable(&spec2, ENTRIES, HashTable, 1);     //inserting element
    searchSimilar(HashTable, ENTRIES, 1, spec.spec_id,spec2.spec_id);

    TEST_ASSERT( HashTable[pos].bucket->bucket_specs[0].head->clique == HashTable[pos2].bucket->nextBucket->bucket_specs[0].head->clique ); //checking that they are in the same clique



    //FREE MEMORY

    struct Bucket* currentBucket;
    struct Bucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
          nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

          for(int j = 0;j < currentBucket->isFull; j++){
              free(currentBucket->bucket_specs[j].spec->spec_id);
              free(currentBucket->bucket_specs[j].spec);
          }
          free(currentBucket->bucket_specs);
          free(currentBucket);
          currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
    }

//		freeHashTable(HashTable,ENTRIES,1);
    free(HashTable);
		free(nextTempBucket);
    free(spec.spec_id);
    free(spec2.spec_id);
}



void test_searchDifferent(void){
    struct Entry* HashTable;
    HashTable = malloc( sizeof(struct Entry) * ENTRIES);
    initializeHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1

    //FIRST ELEMENT
    struct Spec spec;

    spec.spec_id = malloc( strlen("www.maria.com//1234") + 1 );
    strcpy(spec.spec_id,"www.maria.com//1234" );

    int pos = hashFunction(ENTRIES, spec.spec_id);
    insertInHashTable(&spec, ENTRIES, HashTable, 1);     //inserting first element

    //SECOND ELEMENT ( create chain of buckets (overflow of a bucket) )
    struct Spec spec2;
    spec2.spec_id = malloc( strlen("www.maria.com//1333") + 1 );
    strcpy(spec2.spec_id,"www.maria.com//1333" );

    int pos2 = hashFunction(ENTRIES, spec2.spec_id);
    insertInHashTable(&spec2, ENTRIES, HashTable, 1);     //inserting element
    searchDifferent(HashTable, ENTRIES, 1, spec.spec_id,spec2.spec_id);


    TEST_ASSERT( HashTable[pos].bucket->bucket_specs[0].head->list_neg_cor->aclist == HashTable[pos2].bucket->nextBucket->bucket_specs[0].head);
    TEST_ASSERT( HashTable[pos2].bucket->bucket_specs[0].head->list_neg_cor->aclist == HashTable[pos].bucket->nextBucket->bucket_specs[0].head);



    //FREE MEMORY
    struct Bucket* currentBucket;
    struct Bucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->bucket_specs[j].spec->spec_id);
            free(currentBucket->bucket_specs[j].spec);
        }
        free(currentBucket->bucket_specs);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
    }

    free(HashTable);
		free(nextTempBucket);
    free(spec.spec_id);
    free(spec2.spec_id);
}


void test_addWordInVocabulary(void){
    struct VocabEntry* HashTable;
    HashTable = malloc( sizeof(struct VocabEntry) * ENTRIES);

    //FIRST ELEMENT
    char word[7] = "camera";

    int pos = hashFunction2(ENTRIES, word);

    initializeVocabHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1
    addWordInVocabulary(word, ENTRIES, HashTable, 1);     //inserting first element


    TEST_ASSERT(  strcmp(HashTable[pos].bucket->words[0].word, word) == 0 );


    //SECOND ELEMENT ( create chain of buckets (overflow of a bucket) )
    char word2[6] = "camel";

    pos = hashFunction2(ENTRIES, word2);

    addWordInVocabulary(word2, ENTRIES, HashTable, 1);     //inserting element

    TEST_ASSERT(strcmp(HashTable[pos].bucket->nextBucket->words[0].word, word2) == 0 );


    //FREE MEMORY
    struct VocabBucket* currentBucket;
    struct VocabBucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->words[j].word);
        }
        free(currentBucket->words);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
  }
    free(nextTempBucket);
    free(HashTable);

}


void test_searchWordInVocabulary(void){
    struct VocabEntry* HashTable;
    int found;

    HashTable = malloc( sizeof(struct VocabEntry) * ENTRIES);
    initializeVocabHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1

    char word[7] = "camera";

    int pos = hashFunction2(ENTRIES, word);
    addWordInVocabulary(word, ENTRIES, HashTable, 1);
    found = searchWordInVocabulary(HashTable, ENTRIES, 1, word, pos);

    TEST_ASSERT( found == HashTable[pos].bucket->words[0].num ); //checking that they are in the same clique
    TEST_ASSERT( found == 0 ); //checking that they are in the same clique



    //FREE MEMORY
    struct VocabBucket* currentBucket;
    struct VocabBucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->words[j].word);
        }
        free(currentBucket->words);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
    }
    free(nextTempBucket);
    free(HashTable);
}

void test_getWordPosition(void){
    struct VocabEntry* HashTable;
    int found;

    HashTable = malloc( sizeof(struct VocabEntry) * ENTRIES);
    initializeVocabHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1

    char word[7] = "camera";

    int pos = hashFunction2(ENTRIES, word);
    addWordInVocabulary(word, ENTRIES, HashTable, 1);
    found = getWordPosition(HashTable, ENTRIES, 1, word);

    TEST_ASSERT( found == HashTable[pos].bucket->words[0].num ); //checking that they are in the same clique
    TEST_ASSERT( found == 0 ); //checking that they are in the same clique



    //FREE MEMORY
    struct VocabBucket* currentBucket;
    struct VocabBucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->words[j].word);
        }
        free(currentBucket->words);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
    }
    free(nextTempBucket);
    free(HashTable);
}


void test_findSpecInHashTable(void){
    struct Entry* HashTable;
    HashTable = malloc( sizeof(struct Entry) * ENTRIES);

    //FIRST ELEMENT
    struct Spec spec;
    struct Spec *new_spec;

    spec.spec_id = malloc( strlen("www.nelly.com//1234") + 1 );
    strcpy(spec.spec_id,"www.nelly.com//1234" );
    int pos = hashFunction(ENTRIES, spec.spec_id);

    initializeHashTable(HashTable, ENTRIES, 1);  //bucketsize = 1
    insertInHashTable(&spec, ENTRIES, HashTable, 1);     //inserting first element
    new_spec = findSpecInHashTable(HashTable, ENTRIES, 1, spec.spec_id);

    TEST_ASSERT(  strcmp(HashTable[pos].bucket->bucket_specs[0].spec->spec_id, new_spec->spec_id) == 0 );


    //FREE MEMORY
    struct Bucket* currentBucket;
    struct Bucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = HashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->bucket_specs[j].spec->spec_id);
            free(currentBucket->bucket_specs[j].spec);
        }
        free(currentBucket->bucket_specs);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = HashTable[i].bucket;
  }


    free(nextTempBucket);
    free(HashTable);
    free(spec.spec_id);
}


void test_f(void){
  float *weight;
  float *x;
  float result;

	b = (float)1.0;

  weight = malloc(sizeof(float)*3);
  weight[0] = (float)1.0;
  weight[1] = (float)2.0;
  weight[2] = (float)3.0;

  x = malloc(sizeof(float)*3);
  x[0] = (float)1.0;
  x[1] = (float)2.0;
  x[2] = (float)3.0;

  result = f(weight,x);
  TEST_ASSERT( result == (float)15.0 );

  free(weight);
  free(x);

}
void test_sigmoid(void){
  float f = (float)0.001;
  float pred;

  pred = sigmoid(f);
  TEST_ASSERT( pred == (float)0.50025 );

}




void test_init_tf(void){

    //vocabulary initialization
    struct VocabEntry *vocabulary;
    vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
    initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);


    //local vocabulary initialization
    struct Spec spec;
    spec.local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec.local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec.num_of_words = 0;

    char words[5][10] = {"the", "quick", "fox", "jumped", "the"};

    for (int i=0; i<5; i++){
        spec.num_of_words++;
        int in_local = addWordInLocalVocabulary(words[i], LOC_ENTRIES, spec.local_vocab, LOC_BUCKETSIZE);  //add word in local vocabulary, while also checking if it's already in

        if ( in_local == -1 ){ //wasn't found in this spec before
            addWordInVocabulary(words[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE); //try to add in voacbulary only if its not already in local vocabulary
        }
    }


    init_tf(&spec, vocabulary);

    freeVocabulary(spec.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);


    TEST_ASSERT( spec.vector[0] == (float)0.4 );
    TEST_ASSERT( spec.vector[1] == (float)0.2 );
    TEST_ASSERT( spec.vector[2] == (float)0.2 );
    TEST_ASSERT( spec.vector[3] == (float)0.2 );



}




void test_init_idf(void){

    //vocabulary initialization
    struct VocabEntry *vocabulary;
    vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
    initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);


    //SPEC1: local vocabulary initialization
    struct Spec spec1;
    spec1.local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec1.local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec1.num_of_words = 0;

    char words1[3][10] = {"the", "quick", "the"};

    num_of_specs++;

    for (int i=0; i<3; i++){
        spec1.num_of_words++;
        int in_local = addWordInLocalVocabulary(words1[i], LOC_ENTRIES, spec1.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words1[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }





    //SPEC2: local vocabulary initialization
    struct Spec spec2;
    spec2.local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec2.local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec2.num_of_words = 0;

    char words2[4][10] = {"the", "dog", "runs", "away"};

    num_of_specs++;


    for (int i=0; i<4; i++){
        spec2.num_of_words++;
        int in_local = addWordInLocalVocabulary(words2[i], LOC_ENTRIES, spec2.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words2[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }


    float* idf;
    idf = init_idf(num_of_specs, vocabulary, VOC_ENTRIES);


    TEST_ASSERT( idf[0] == (float)( log10((float)2.0/(float)2.0) ) );
    TEST_ASSERT( idf[1] == (float)( log10((float)2.0/(float)1.0) ) );
    TEST_ASSERT( idf[2] == (float)( log10((float)2.0/(float)1.0) ) );
    TEST_ASSERT( idf[3] == (float)( log10((float)2.0/(float)1.0) ) );
    TEST_ASSERT( idf[4] == (float)( log10((float)2.0/(float)1.0) ) );


    freeVocabulary(spec1.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(spec2.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);

}





void test_calculate_tfidf_mean(void){

    //vocabulary initialization
    struct VocabEntry *vocabulary;
    vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
    initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);

    //hashtable initialization
    struct Entry* hashTable;
    hashTable = malloc( sizeof(struct Entry) * NUM_OF_ENTRIES);
    initializeHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE);



    //SPEC1: local vocabulary initialization
    struct Spec *spec1 = malloc(sizeof(struct Spec));
    spec1->spec_id = malloc(sizeof(char*) * 20);
    strcpy(spec1->spec_id, "www.nelly.com//1333");
    spec1->local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec1->local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec1->num_of_words = 0;

    char words1[3][10] = {"the", "quick", "the"};

    num_of_specs++;

    for (int i=0; i<3; i++){
        spec1->num_of_words++;
        int in_local = addWordInLocalVocabulary(words1[i], LOC_ENTRIES, spec1->local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words1[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }

    //calculate tf value for first spec
    init_tf(spec1, vocabulary);


    //insert spec in hashtable
    insertInHashTable( spec1, NUM_OF_ENTRIES, hashTable, BUCKET_SIZE);



    //SPEC2: local vocabulary initialization
    struct Spec *spec2 = malloc(sizeof(struct Spec));
    spec2->local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    spec2->spec_id = malloc(sizeof(char*) * 20);
    strcpy(spec2->spec_id, "www.maria.com//1333");
    initializeVocabHashTable(spec2->local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec2->num_of_words = 0;

    char words2[4][10] = {"the", "dog", "runs", "away"};

    num_of_specs++;


    for (int i=0; i<4; i++){
        spec2->num_of_words++;
        int in_local = addWordInLocalVocabulary(words2[i], LOC_ENTRIES, spec2->local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words2[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }

    //calculate tf value for second spec
    init_tf(spec2, vocabulary);


    //insert spec in hashtable
    insertInHashTable( spec2, NUM_OF_ENTRIES, hashTable, BUCKET_SIZE);


    finalVectorsSize = 5;

    //set num_of_specs large enough so idf[i] > 2.3 to check that case
    num_of_specs = 30000;

    //calculate idf table
    float* idf;
    idf = init_idf(num_of_specs, vocabulary, VOC_ENTRIES);

    //Calculate tfidf values for each spec and array of mean of tfidf values
    float* tfidf_mean;
    tfidf_mean = calculate_tfidf_mean(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, idf); //calculating tfidf values and average tfidf values



    //for first spec:

    TEST_ASSERT( spec1->vector[0] == (float)( (float)(2.0)/ (float)3.0 * log10((float)30000.0/(float)2.0) ) );
    TEST_ASSERT( spec1->vector[1] == (float)( (float)(1.0)/ (float)3.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec1->vector[2] == (float)0.0 );
    TEST_ASSERT( spec1->vector[3] == (float)0.0 );
    TEST_ASSERT( spec1->vector[4] == (float)0.0 );


    //for second spec:

    TEST_ASSERT( spec2->vector[0] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)2.0) ) );
    TEST_ASSERT( spec2->vector[1] == (float)0.0 );
    TEST_ASSERT( spec2->vector[2] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec2->vector[3] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec2->vector[4] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );


    //Checking correctness of tfidf mean values:
    TEST_ASSERT( tfidf_mean[0] == (float)( (spec1->vector[0] + spec2->vector[0]) / (float)num_of_specs ) );
    TEST_ASSERT( tfidf_mean[1] == (float)( (spec1->vector[1] + spec2->vector[1]) / (float)num_of_specs ) );
    TEST_ASSERT( tfidf_mean[2] == (float)( (spec1->vector[2] + spec2->vector[2]) / (float)num_of_specs ) );
    TEST_ASSERT( tfidf_mean[3] == (float)( (spec1->vector[3] + spec2->vector[3]) / (float)num_of_specs ) );
    TEST_ASSERT( tfidf_mean[4] == (float)( (spec1->vector[4] + spec2->vector[4]) / (float)num_of_specs ) );






    freeVocabulary(spec1->local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(spec2->local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);
    freeHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE);

    free(spec1->local_vocab);
    free(spec2->local_vocab);
    free(vocabulary);
    free(hashTable);

    free(idf);
    free(tfidf_mean);

    free(spec1);
    free(spec2);


}




void test_final_tfidf(void){

    //vocabulary initialization
    struct VocabEntry *vocabulary;
    vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
    initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);

    //hashtable initialization
    struct Entry* hashTable;
    hashTable = malloc( sizeof(struct Entry) * NUM_OF_ENTRIES);
    initializeHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE);



    //SPEC1: local vocabulary initialization
    struct Spec *spec1 = malloc(sizeof(struct Spec));
    spec1->spec_id = malloc(sizeof(char*) * 20);
    strcpy(spec1->spec_id, "www.nelly.com//1333");
    spec1->local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec1->local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec1->num_of_words = 0;

    char words1[3][10] = {"the", "quick", "the"};

    num_of_specs++;

    for (int i=0; i<3; i++){
        spec1->num_of_words++;
        int in_local = addWordInLocalVocabulary(words1[i], LOC_ENTRIES, spec1->local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words1[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }

    //calculate tf value for first spec
    init_tf(spec1, vocabulary);


    //insert spec in hashtable
    insertInHashTable( spec1, NUM_OF_ENTRIES, hashTable, BUCKET_SIZE);



    //SPEC2: local vocabulary initialization
    struct Spec *spec2 = malloc(sizeof(struct Spec));
    spec2->local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    spec2->spec_id = malloc(sizeof(char*) * 20);
    strcpy(spec2->spec_id, "www.maria.com//1333");
    initializeVocabHashTable(spec2->local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec2->num_of_words = 0;

    char words2[4][10] = {"the", "dog", "runs", "away"};

    num_of_specs++;


    for (int i=0; i<4; i++){
        spec2->num_of_words++;
        int in_local = addWordInLocalVocabulary(words2[i], LOC_ENTRIES, spec2->local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){
            addWordInVocabulary(words2[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
    }

    //calculate tf value for second spec
    init_tf(spec2, vocabulary);


    //insert spec in hashtable
    insertInHashTable( spec2, NUM_OF_ENTRIES, hashTable, BUCKET_SIZE);


    finalVectorsSize = totalVocabularyWords;

    //set num_of_specs large enough so idf[i] > 2.3 to check that case
    num_of_specs = 1700000;

    //calculate idf table
    float* idf;
    idf = init_idf(num_of_specs, vocabulary, VOC_ENTRIES);

    //Calculate tfidf values for each spec and array of mean of tfidf values
    float* tfidf_mean;
    tfidf_mean = calculate_tfidf_mean(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, idf); //calculating tfidf values and average tfidf values




    //for first spec:

    //Calculate final tfidf values (final size of vector) for spec1
    final_tfidf(spec1, tfidf_mean);


    TEST_ASSERT( spec1->vector[0] == (float)( (float)(2.0)/ (float)3.0 * log10((float)num_of_specs/(float)2.0) ) );
    TEST_ASSERT( spec1->vector[1] == (float)( (float)(1.0)/ (float)3.0 * log10((float)num_of_specs/(float)1.0) ) );
    TEST_ASSERT( spec1->vector[2] == (float)0.0 );
    TEST_ASSERT( spec1->vector[3] == (float)0.0 );
    TEST_ASSERT( spec1->vector[4] == (float)0.0 );

    //Words left after clipping
    TEST_ASSERT(finalVectorsSize == 2);


    freeVocabulary(spec1->local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    free(spec1->local_vocab);
    freeVocabulary(spec2->local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    free(spec2->local_vocab);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);
    free(vocabulary);


    //FREE HASHTABLE
    struct Bucket* currentBucket;
    struct Bucket* nextTempBucket;
    for(int i = 0;i < ENTRIES;i++){                //for every entry in the hashtable
        currentBucket = hashTable[i].bucket;
        while(currentBucket != NULL){
        nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs

        for(int j = 0;j < currentBucket->isFull; j++){
            free(currentBucket->bucket_specs[j].spec->spec_id);
            free(currentBucket->bucket_specs[j].spec);
        }
        free(currentBucket->bucket_specs);
        free(currentBucket);
        currentBucket = nextTempBucket;
        }
        currentBucket = hashTable[i].bucket;
    }

    free(nextTempBucket);
    free(hashTable);


    free(idf);
    free(tfidf_mean);

    free(spec1);
    free(spec2);
}





void test_endofword(void){


    char word[3] = "1.5";

    char cur;
    int res;

    for( int i=0; i<3; i++){    //for flag = false

        cur = word[i];

        res = endofword(cur, false);

        if(i==0) TEST_ASSERT( res == 0);
        if(i==1) TEST_ASSERT( res == 1);
        if(i==2) TEST_ASSERT( res == 0);

    }


    res = endofword(word[0], true);
    TEST_ASSERT( res == 0);

    res = endofword(word[1], true);
    TEST_ASSERT( res == 0);

    res = endofword(word[2], true);
    TEST_ASSERT( res == 0);


}



void test_update_w(void){
	float *weight;
  float *gradient;

	a = 0.1;

  weight = malloc(sizeof(float)*3);
  weight[0] = 0.0;
  weight[1] = 1.0;
  weight[2] = 3.0;

  gradient = malloc(sizeof(float)*3);
  gradient[0] = 1.0;
  gradient[1] = 2.0;
  gradient[2] = 3.0;

	weight = update_w(weight,gradient);
	TEST_ASSERT( weight[0] == (float)-0.1);
	TEST_ASSERT( weight[1] == (float)0.8 );
	TEST_ASSERT( weight[2] == (float)2.7 );
	TEST_ASSERT( last_update == 0 );

	epochs = 1;
	weight = update_w(weight,gradient);
	TEST_ASSERT( weight[0] == (float)-0.2);
	TEST_ASSERT( weight[1] == (float)0.6);
	TEST_ASSERT( weight[2] == (float)2.4 );
	TEST_ASSERT( last_update == 1 );

	free(weight);
	free(gradient);
}





TEST_LIST = {

    { "setVector", test_setVector},
	{"hashFunction2", test_hashFunction2},
    {"searchSimilar",test_searchSimilar},
    {"searchDifferent",test_searchDifferent},
    {"addWordInVocabulary",test_addWordInVocabulary},
    {"searchWordInVocabulary",test_searchWordInVocabulary},
    {"getWordPosition",test_getWordPosition},
    {"findSpecInHashTable",test_findSpecInHashTable},
	{"sigmoid",test_sigmoid},
    {"f",test_f},
    { "init_tf", test_init_tf},
    { "init_idf", test_init_idf},
    { "calculate_tfidf_mean", test_calculate_tfidf_mean},
    { "final_tfidf", test_final_tfidf},
    { "endofword", test_endofword },
		{"update_w",test_update_w},
    {NULL,NULL}

};
