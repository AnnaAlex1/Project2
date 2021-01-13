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
int finalVectorsSize;

struct AccessCliques* aclist = NULL;         //a list holding the start of each clique
//global variable for vocabulary?

int num_of_specs=0;




void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv, int* tfidf_mean);





int main(int argc, char** argv){


  //CHECKING ARGUMENTS
  if (strcmp(argv[argc-1],"tf-idf") && strcmp(argv[argc-1],"bow")) {
    perror("Wrong parameter");
    return -1;
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

  //printf
  /*for (int i=0; i<totalVocabularyWords; i++){
    printf("idf[%d]: %f\n", i, idf[i]);
  }*/
  //printf("%d\n",num_of_specs);


  //CALCULATING VECTORS
  printf("Size of vocabulary: before = %d\n", totalVocabularyWords);

  finalVectorsSize = totalVocabularyWords; //initialize with initial vocabulary size

  float* tfidf_mean;
  tfidf_mean = calculate_tfidf_mean(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, idf); //calculating tfidf values and average tfidf values

  freeVocabulary(vocabulary,VOC_ENTRIES,VOC_BUCK_SIZE);

  //FIND 1000 WORDS TO KEEP
  int *positions = clip_vectorsize(tfidf_mean);
  


  setSpecsVectors(hashTable,NUM_OF_ENTRIES,BUCKET_SIZE,argv[argc-1],positions);
  printf("Vectors for every spec initialized!\n");

  printf("Size of vocabulary: after = %d\n", finalVectorsSize);




  //READING DATASET W
  FILE* datasetw = fopen("./sigmod_medium_labelled_dataset.csv","r");
  char left_spec_id[70];
  char right_spec_id[70];
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

  char curbuf[70];
  fseek(datasetw,0,SEEK_SET);
  fgets(curbuf,BSIZE,datasetw);

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

  printf("Matching specs is successfully completed!\n");

  //checkHashTable(vocabulary, VOC_ENTRIES, VOC_BUCK_SIZE);



  //PRINT OF RESULTS
  if ( print_results_all( aclist ) == 1) {return 1;}



  //MACHINE LEARNING
  logistic_regression((float)0.8,hashTable,argv[argc-1]);



  printf("Release of memory!\n");


  fclose(stop_file);
  fclose(datasetw);

  //RELEASE OF MEMORY
  release_Clique(aclist);             //release memory of cliques
  release_aclist(&aclist);           //release memory of list with the beginnings of cliques

  freeHashTable(hashTable, NUM_OF_ENTRIES,BUCKET_SIZE);
  free(vocabulary);
  free(hashTable);
  free(tfidf_mean);
  free(positions);
  free(idf);


  return 0;

}



void setSpecsVectors(struct Entry* hashTable,int numOfEntries,int bucketSize,char* argv, int* positions)
{
  struct Bucket* currentBucket;
  int numOfBucket;

  for(int i = 0;i < numOfEntries;i++){
    numOfBucket = 1;
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){
        //printf("Entry: %d, Bucket: %d, place in bucket: %d/%d id: %s\n", i, numOfBucket, j, bucketSize, currentBucket->bucket_specs[j].spec->spec_id);
        final_tfidf(currentBucket->bucket_specs[j].spec, positions);
        if (strcmp(argv,"bow")==0) {

          if ( setVector(currentBucket->bucket_specs[j].spec) == FAIL) return;

        }
      }
      currentBucket = currentBucket->nextBucket;
      numOfBucket++;
    }
  }
}
