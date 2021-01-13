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

#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

extern int totalVocabularyWords;
extern int sizeOfDatasetW;
int finalVectorsSize;
float b;

struct AccessCliques* aclist = NULL;         //a list holding the start of each clique

int num_of_specs=0;



void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv,float* tfidf_mean);




int main (int argc, char** argv) {

    //CHECKING ARGUMENTS
    if (strcmp(argv[argc-1],"tf-idf") && strcmp(argv[argc-1],"bow")) {
        perror("Wrong parameter");
        return -1;
    }

    char answer = 'n';
    char *path;

    // COMMUNICATE WITH USER
    printf("\nModel uses by default a portion (0.2) of datasetW to perform testing.\n\n");
    printf("Do you want to enter a new dataset for testing? (y/n) \t");
    answer = fgetc(stdin);
    if (answer=='y') {

        printf("\nGive model a new dataset (in csv format) by inserting its full path below:\n");

        path = malloc(100*sizeof(char));
        scanf("%s",path);
        printf("\nThank you! Let testing begin...\n");

    } else {

        printf("\nIt's ok! Let default testing begin...\n\n");
    }


    //VOCABULARY
    struct VocabEntry *vocabulary;
    vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
    initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);
    printf("Vocab initialised\n");


    //HASHTABLE OF SPECS
    struct Entry* hashTable;
    hashTable = malloc( sizeof(struct Entry) * NUM_OF_ENTRIES);
    initializeHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE);
    printf("Hash initialised\n");



    //OPENING FILE WITH STOPWORDS
    FILE* stop_file = fopen("./common-english-words.txt","r");
    if (stop_file==NULL)
    {
        perror("Cannot open file of stopwords.\n");
        return -1;
    }


    //READING DATASET_X
    if ( read_datasetX(NUM_OF_ENTRIES, hashTable, BUCKET_SIZE, vocabulary,  stop_file) == 1 ) { return 1; }
    printf("Specs added to the hashtable successfully!\n");


    //checkvoc(vocabulary);

    //CALCULATION OF IDF VALUES
    float* idf;
    idf = init_idf(num_of_specs,vocabulary, VOC_ENTRIES);



    //CALCULATING VECTORS
    printf("Size of vocabulary: before = %d\n", totalVocabularyWords);

    finalVectorsSize = totalVocabularyWords; //initialize with initial vocabulary size

    float* tfidf_mean;
    tfidf_mean = calculate_tfidf_mean(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, idf); //calculating tfidf values and average tfidf values

    freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);

    setSpecsVectors(hashTable,NUM_OF_ENTRIES,BUCKET_SIZE,argv[argc-1],tfidf_mean);
    printf("Vectors for every spec initialized!\n");

    printf("Size of vocabulary: after = %d\n", finalVectorsSize);



    //RUN TEST

    //get weights in order to run the trained model
    FILE *fweights = fopen("./best_weights.txt","r");
    float weights[finalVectorsSize];
    
    for (int i=0; i<finalVectorsSize; ++i) {
       fscanf(fweights,"%f\n",&(weights[i]));
    }
    fscanf(fweights,"%f\n",&(weights[finalVectorsSize]));
    b = weights[finalVectorsSize];


    FILE *datasetw;
    if (answer=='y') {  //user inserts dataset for testing

        datasetw = fopen(path,"r");
        free(path);
        test2(datasetw, hashTable, argv[argc-1], b, weights,(float)0.8); //run test


    } else {    //default dataset for testing (20% of datasetW)
        //datasetw = fopen("./sigmod_medium_labelled_dataset.csv","r");
        datasetw = fopen("./sigmod_large_labelled_dataset.csv","r");
        testDefault (datasetw, hashTable, argv[argc-1], b, weights,(float)0.8);
        
    }
    
    printf("\nOpen test_predictions.csv to check model's predictions!\n");
    printf("Release of memory!\n");
    
    fclose(stop_file);

    //RELEASE OF MEMORY
    release_Clique(aclist);             //release memory of cliques
    release_aclist(&aclist);           //release memory of list with the beginnings of cliques

    freeHashTable(hashTable, NUM_OF_ENTRIES,BUCKET_SIZE);
    free(vocabulary);
    free(hashTable);
    free(tfidf_mean);
    free(idf);



    return 0;



}



void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv,float* tfidf_mean)
{
  struct Bucket* currentBucket;
  int numOfBucket;

  for(int i = 0;i < numOfEntries;i++){
    numOfBucket = 1;
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){
        //printf("Entry: %d, Bucket: %d, place in bucket: %d/%d id: %s\n", i, numOfBucket, j, bucketSize, currentBucket->bucket_specs[j].spec->spec_id);
        final_tfidf(currentBucket->bucket_specs[j].spec, tfidf_mean);
        if (strcmp(argv,"bow")==0) {

          if ( setVector(currentBucket->bucket_specs[j].spec) == FAIL) return;

        }
      }
      currentBucket = currentBucket->nextBucket;
      numOfBucket++;
    }
  }
}