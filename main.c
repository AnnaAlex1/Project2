#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"

#include <stdlib.h>
#include <errno.h>
#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"
#include "BOW.h"
#include "words.h"
#include "tfidf.h"
#include "bowvector.h"

#include <stdlib.h>
#include <errno.h>

#include <string.h>

#define NUM_OF_ENTRIES 300
#define BUCKET_SIZE 1000

extern int totalVocabularyWords;
int finalVectorsSize;

struct AccessCliques* aclist = NULL;         //a list holding the start of each clique
//global variable for vocabulary?

int num_of_specs=0;

void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv,float* idf);

int main(int argc, char** argv){

  //struct Tfidf *tfidf = NULL;
  //double *tfidf_column_sum;
  

  if (strcmp(argv[argc-1],"tf-idf") && strcmp(argv[argc-1],"bow")) {
    perror("Wrong parameter");
    return -1;
  }

  struct VocabEntry *vocabulary;
  vocabulary = malloc( sizeof(struct VocabEntry) * VOC_ENTRIES);
  initializeVocabHashTable(vocabulary,VOC_ENTRIES ,VOC_BUCK_SIZE);


  struct Entry* hashTable;
  hashTable = malloc( sizeof(struct Entry) * NUM_OF_ENTRIES);

  initializeHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE);

  FILE* stop_file = fopen("./common-english-words.txt","r");
  if (stop_file==NULL)
    {
      perror("Cannot open file of stopwords.\n");
      return -1;
    }


  float* idf;
  idf = init_idf(num_of_specs,vocabulary, VOC_ENTRIES);
  

//algorithm:

  //Reading dataset X
  if ( read_datasetX(NUM_OF_ENTRIES, hashTable, BUCKET_SIZE, vocabulary,  stop_file) == 1 ) { return 1; }

  printf("Specs added to the hashtable successfully!\n");


  finalVectorsSize = totalVocabularyWords; //initialize with initial vocabulary size
  
  setSpecsVectors(hashTable,NUM_OF_ENTRIES,BUCKET_SIZE,argv[argc-1],idf);
  printf("Vectors for every spec initialized!\n");
  


  //Reading dataset W

  FILE* datasetw = fopen("./sigmod_large_labelled_dataset.csv","r");
  char left_spec_id[50];
  char right_spec_id[50];
  int wstatus;



  while ((wstatus=readDatasetW_1(datasetw,left_spec_id,right_spec_id))!=EOF)
  {

      if (wstatus==ENOENT) return wstatus;

      //in merge of matching lists move spec with greater id value
      if (greaterSpecId (left_spec_id, right_spec_id) == RIGHT){
        searchSimilar(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, right_spec_id, left_spec_id);
      }
      else{
        searchSimilar(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, left_spec_id, right_spec_id);

      }

  }


  while ((wstatus=readDatasetW_0(datasetw,left_spec_id,right_spec_id))!=EOF)
  {

      if (wstatus==ENOENT) return wstatus;

      //in merge of matching lists move spec with greater id value
      if (greaterSpecId (left_spec_id, right_spec_id) == RIGHT){
        searchDifferent(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, right_spec_id, left_spec_id);
      }
      else{
        searchDifferent(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, left_spec_id, right_spec_id);

      }

  }


  //checkHashTable(vocabulary, VOC_ENTRIES, VOC_BUCK_SIZE);

  fclose(stop_file);
  fclose(datasetw);
  printf("Matching specs is successfully completed!\n");


  //Results - create output file
  if ( print_results( aclist ) == 1) {return 1;}

  release_Clique(aclist);             //release memory of cliques
  release_aclist(&aclist);           //release memory of list with the beginnings of cliques

  freeHashTable(hashTable, NUM_OF_ENTRIES,BUCKET_SIZE);
  free(hashTable);

  return 0;

}



void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv,float* idf)
{
  struct Bucket* currentBucket;
  int numOfBucket;
  for(int i = 0;i < numOfEntries;i++){
    numOfBucket = 1;
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){
        //printf("Entry: %d, Bucket: %d, place in bucket: %d/%d id: %s\n", i, numOfBucket, j, bucketSize, currentBucket->bucket_specs[j].spec->spec_id);
        calculate_tfidf(currentBucket->bucket_specs[j].spec, idf);
        //clip_tfidf();
        if (strcmp(argv,"bow")==0) {

          if ( setVector(currentBucket->bucket_specs[j].spec) == FAIL) return;
          //if ( setVector( currentBucket->bucket_specs[j].spec ) == FAIL) return;

        } 
      }
      currentBucket = currentBucket->nextBucket;
      numOfBucket++;
    }
  }
}
