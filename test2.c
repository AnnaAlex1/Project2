//test2.c

#include <stdio.h>
#include "acutest.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "readX.h"
#include "ids_list.h"
#include "hashTable.h"
#include "readW.h"
#include "result.h"
#include "BOW.h"
#include "words.h"
#include "tfidf.h"
#include "bowvector.h"


int finalVectorsSize;
struct AccessCliques* aclist = NULL;         //a list holding the start of each clique
int num_of_specs=0;
extern int totalVocabularyWords;


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
        printf("word: %s\n", words[i]);
        int in_local = addWordInLocalVocabulary(words[i], LOC_ENTRIES, spec.local_vocab, LOC_BUCKETSIZE);  //add word in local vocabulary, while also checking if it's already in
        
        if ( in_local == -1 ){ //wasn't found in this spec before
            printf("%d\n", addWordInVocabulary(words[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE));//try to add in voacbulary only if its not already in local vocabulary
        }
        printf("totalvocabwords: %d\n", totalVocabularyWords);
    }

    printf("\nLOCAL: \n");
    //checkvoc(spec.local_vocab);

    printf("\nGLOBAL: \n");
    //checkvoc(vocabulary);

   
    init_tf(&spec, vocabulary);


/*    for (int i=0; i<totalVocabularyWords; i++){
        printf("spec.tfidf[%d]: %f\n",i, spec.tfidf[i]);
    }
*/

    freeVocabulary(spec.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);
   
    
    TEST_ASSERT( spec.tfidf[0] == (float)0.4 );
    TEST_ASSERT( spec.tfidf[1] == (float)0.2 );
    TEST_ASSERT( spec.tfidf[2] == (float)0.2 );
    TEST_ASSERT( spec.tfidf[3] == (float)0.2 );



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
        printf("word1: %s\n", words1[i]);
        int in_local = addWordInLocalVocabulary(words1[i], LOC_ENTRIES, spec1.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){ 
            addWordInVocabulary(words1[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
        printf("totalvocabwords: %d\n", totalVocabularyWords);
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
        printf("word2: %s\n", words2[i]);
        int in_local = addWordInLocalVocabulary(words2[i], LOC_ENTRIES, spec2.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){ 
            addWordInVocabulary(words2[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
        printf("totalvocabwords: %d\n", totalVocabularyWords);
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

 

void test_calculate_tfidf(void){

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
        printf("word1: %s\n", words1[i]);
        int in_local = addWordInLocalVocabulary(words1[i], LOC_ENTRIES, spec1.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){ 
            addWordInVocabulary(words1[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
        printf("totalvocabwords: %d\n", totalVocabularyWords);
    }

    //calculate tf value for first spec
    init_tf(&spec1, vocabulary);





    //SPEC2: local vocabulary initialization
    struct Spec spec2;
    spec2.local_vocab = malloc( sizeof(struct VocabEntry) * LOC_ENTRIES);
    initializeVocabHashTable(spec2.local_vocab, LOC_ENTRIES , LOC_BUCKETSIZE);
    spec2.num_of_words = 0;

    char words2[4][10] = {"the", "dog", "runs", "away"}; 

    num_of_specs++;


    for (int i=0; i<4; i++){
        spec2.num_of_words++;
        printf("word2: %s\n", words2[i]);
        int in_local = addWordInLocalVocabulary(words2[i], LOC_ENTRIES, spec2.local_vocab, LOC_BUCKETSIZE);  //add in local

        if ( in_local == -1 ){ 
            addWordInVocabulary(words2[i], VOC_ENTRIES, vocabulary, VOC_BUCK_SIZE);     //add in global
        }
        printf("totalvocabwords: %d\n", totalVocabularyWords);
    }

    //calculate tf value for second spec
    init_tf(&spec2, vocabulary);

    //set num_of_specs large enough so idf[i] > 2.3 to check that case
    num_of_specs = 30000;

    //calculate idf table
    float* idf;
    idf = init_idf(num_of_specs, vocabulary, VOC_ENTRIES);

    

    //for first spec:
    calculate_tfidf(&spec1, idf);

    TEST_ASSERT( spec1.tfidf[0] == (float)( (float)(2.0)/ (float)3.0 * log10((float)30000.0/(float)2.0) ) );
    TEST_ASSERT( spec1.tfidf[1] == (float)( (float)(1.0)/ (float)3.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec1.tfidf[2] == (float)0.0 );
    TEST_ASSERT( spec1.tfidf[3] == (float)0.0 );
    TEST_ASSERT( spec1.tfidf[4] == (float)0.0 );
    

    //for second spec:
    calculate_tfidf(&spec2, idf);

    TEST_ASSERT( spec2.tfidf[0] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)2.0) ) );
    TEST_ASSERT( spec2.tfidf[1] == (float)0.0 );
    TEST_ASSERT( spec2.tfidf[2] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec2.tfidf[3] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );
    TEST_ASSERT( spec2.tfidf[4] == (float)( (float)(1.0)/ (float)4.0 * log10((float)30000.0/(float)1.0) ) );


    freeVocabulary(spec1.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(spec2.local_vocab, LOC_ENTRIES, LOC_BUCKETSIZE);
    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);


}




TEST_LIST = {

    { "init_tf", test_init_tf},
    { "init_idf", test_init_idf},
    { "calculate_tfidf", test_calculate_tfidf},
    {NULL,NULL}

};