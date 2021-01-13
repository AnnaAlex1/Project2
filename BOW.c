#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ids_list.h"
#include "readX.h"
#include "readW.h"
#include "hashTable.h"
#include "BOW.h"

int totalVocabularyWords = 0;
struct RareWord *rareWordsList = NULL;
extern int num_of_specs;

void initialiseVocabBucket(struct VocabBucket* currentBucket,int bucketSize){   //initialises a vocabulary bucket
  currentBucket->isFull = 0;
  currentBucket->nextBucket = NULL;
  currentBucket->words = malloc(sizeof(struct VocabWord)*bucketSize);
}


void initializeVocabHashTable(struct VocabEntry* hashTable,int numOfEntries,int bucketSize){
  for(int i=0;i<numOfEntries;i++){
    hashTable[i].key = i; //set key to a bucket
    hashTable[i].bucket = malloc(sizeof(struct VocabBucket));  //allocate bucket
    initialiseVocabBucket(hashTable[i].bucket,bucketSize); //initialise bucket
  }
}

int hashFunction2(int HashtableNumOfEntries,char* hashName){
  int entryNum;
  int hashNum;
  char first2Letters[2];

  if (strlen(hashName) == 1){
    if (hashName[0] >= '0' && hashName[0] <= '9'){
      hashNum = hashName[0] - '0';
    } else {
        hashNum = hashName[0] - 'a';
    }
    entryNum = hashNum % HashtableNumOfEntries;
    return entryNum;
  }
  memcpy(first2Letters,hashName,2*sizeof(char));             // get the first 2 chars of the word
  hashNum = (first2Letters[0] + first2Letters[1])*first2Letters[0];


  entryNum = hashNum % HashtableNumOfEntries;
  return entryNum;
}



int getWordPosition(struct VocabEntry* hashTable, int HashtableNumOfEntries, int bucketSize, char* word){
  struct VocabBucket* currentBucket;
  int found_word = -1; //boolean flag

  int entryNum = hashFunction2(HashtableNumOfEntries,word);                        // find correct entry for new word

  //check if word alreaduy exists in vocabulary
  currentBucket = hashTable[entryNum].bucket;
  while(currentBucket != NULL && found_word == -1){
    for(int j = 0;j < currentBucket->isFull ;j++){               // search for word's bucket
      if(strcmp(currentBucket->words[j].word,word) == 0){   //found word's place in bucket
        found_word = currentBucket->words[j].num; //raise flag
        break;
      }
    }
    currentBucket = currentBucket->nextBucket;
  }
  return found_word;
}






int searchWordInVocabulary(struct VocabEntry* hashTable, int HashtableNumOfEntries, int bucketSize, char* word,int entryNum){
  struct VocabBucket* currentBucket;
  int found_word = -1; //boolean flag

  //check if word alreaduy exists in vocabulary
  currentBucket = hashTable[entryNum].bucket;
  while(currentBucket != NULL && found_word == -1){
    for(int j = 0;j < currentBucket->isFull ;j++){               // search for word's bucket
      if(strcmp(currentBucket->words[j].word,word) == 0){   //found word's place in bucket
        found_word = currentBucket->words[j].num; //raise flag
        currentBucket->words[j].freq++;
        break;
      }
    }
    currentBucket = currentBucket->nextBucket;
  }
  return found_word;
}




int addWordInVocabulary(char* word, int HashtableNumOfEntries, struct VocabEntry* HashTable, int bucketSize){
  int entryNum; //key
  int length; //length of spec_id
  struct VocabBucket* currentBucket;
  char* hashName; //string given for hashing <- spec_id
  int bucket_place;
  int found_word;

  hashName = word;
  entryNum = hashFunction2(HashtableNumOfEntries,hashName);                        // find correct entry for new word
  length = strlen(hashName) + 1;

  found_word = searchWordInVocabulary(HashTable, HashtableNumOfEntries, bucketSize, word, entryNum);   // check if word already exists in vocabulary

  if(found_word > -1){
    return found_word;
  }
  if (found_word < -1){
    return -999;
  }

  currentBucket = HashTable[entryNum].bucket;                                     //use entry and point to proper bucket
  while(currentBucket->nextBucket != NULL){                                       //loop chain of buckets
    currentBucket = currentBucket->nextBucket;                                      //find the first free bucket to put the word
  }

  if(currentBucket->isFull == bucketSize){                                        //if the first free bucket is full,
    currentBucket->nextBucket = malloc(sizeof(struct VocabBucket));                      //next bucket needs to be allocated,
    currentBucket = currentBucket->nextBucket;                                      //pointed and
    initialiseVocabBucket(currentBucket,bucketSize);                                //initialised
    bucket_place = 0;                                                               //hold place of spec in bucket (first spec in bucket => place = 0)
  }
  else if(currentBucket->isFull < bucketSize){                                    //next free bucket already allocated
    bucket_place = currentBucket->isFull;                                           //hold place of spec in bucket
  }



  currentBucket->words[bucket_place].word = malloc(length); //allocate memory for word
  memcpy(currentBucket->words[bucket_place].word, word, length); //copy word
  currentBucket->words[bucket_place].num = totalVocabularyWords;
  currentBucket->words[bucket_place].freq = 1;
  currentBucket->isFull++; //adjust place for incoming word
  totalVocabularyWords++;

  return found_word;

}


int addWordInLocalVocabulary(char* word, int HashtableNumOfEntries, struct VocabEntry* HashTable, int bucketSize){
  int entryNum; //key
  int length; //length of spec_id
  struct VocabBucket* currentBucket;
  char* hashName; //string given for hashing <- spec_id
  int bucket_place;
  int found_word;

  hashName = word;
  entryNum = hashFunction2(HashtableNumOfEntries,hashName);                        // find correct entry for new word
  length = strlen(hashName) + 1;

  found_word = searchWordInVocabulary(HashTable, HashtableNumOfEntries, bucketSize, word, entryNum);   // check if word already exists in vocabulary

  if(found_word > -1){
    return found_word;
  }
  if (found_word < -1){
    return -999;
  }

  currentBucket = HashTable[entryNum].bucket;                                     //use entry and point to proper bucket
  while(currentBucket->nextBucket != NULL){                                       //loop chain of buckets
    currentBucket = currentBucket->nextBucket;                                      //find the first free bucket to put the word
  }

  if(currentBucket->isFull == bucketSize){                                        //if the first free bucket is full,
    currentBucket->nextBucket = malloc(sizeof(struct VocabBucket));                      //next bucket needs to be allocated,
    currentBucket = currentBucket->nextBucket;                                      //pointed and
    initialiseVocabBucket(currentBucket,bucketSize);                                //initialised
    bucket_place = 0;                                                               //hold place of spec in bucket (first spec in bucket => place = 0)
  }
  else if(currentBucket->isFull < bucketSize){                                    //next free bucket already allocated
    bucket_place = currentBucket->isFull;                                           //hold place of spec in bucket
  }

  currentBucket->words[bucket_place].word = malloc(length); //allocate memory for word
  memcpy(currentBucket->words[bucket_place].word, word, length); //copy word
  currentBucket->words[bucket_place].num = totalVocabularyWords;
  currentBucket->words[bucket_place].freq = 1;
  currentBucket->isFull++; //adjust place for incoming word
  return found_word;
}



void freeVocabulary(struct VocabEntry* hashTable,int numOfEntries,int bucketSize){
  struct VocabBucket* currentBucket;
  struct VocabBucket* nextTempBucket;

  for(int i = 0;i < numOfEntries;i++){                //for every entry in thehashtable
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      nextTempBucket = currentBucket->nextBucket;                               //remove data from bucket_specs
      for(int j = 0;j < currentBucket->isFull; j++){
        free(currentBucket->words[j].word);
      }
      free(currentBucket->words);
      free(currentBucket);
      currentBucket = nextTempBucket;
    }
    currentBucket = hashTable[i].bucket;
  }
}

/*

void checkvoc(struct VocabEntry* vocabulary){
  struct VocabBucket* currentBucket;
  int count=0;
  int numOfBucket;
  for(int i = 0;i < VOC_ENTRIES;i++){
    numOfBucket = 1;
    currentBucket = vocabulary[i].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){
        printf("Entry: %d, Bucket: %d, place in bucket: %d/%d word: %s, pos: %d, freq: %d\n", i, numOfBucket, j, VOC_BUCK_SIZE, currentBucket->words[j].word, currentBucket->words[j].num, currentBucket->words[j].freq);
        count++;
      }
      currentBucket = currentBucket->nextBucket;
      numOfBucket++;
    }
  }
  printf("----------------------------count: %d\n", count);
}*/
