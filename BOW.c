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


int hashFunction1(int HashtableNumOfEntries,int hashNum){
  int entryNum;

  //printf("name: %d\n",hashNum);
  entryNum = hashNum%HashtableNumOfEntries;
  return entryNum;
}


int hashFunction2(int HashtableNumOfEntries,char* hashName){
  int entryNum;
  int hashNum;
  char first2Letters[2];

  //printf("name: %s\n",hashName);
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
  //printf("1st: %c, 2nd: %c\n",first2Letters[0],first2Letters[1]);
  if((first2Letters[0] >= '0' && first2Letters[0] <= '9') && (first2Letters[1] >= '0' && first2Letters[1] <= '9') ){        // case: 2 first chars = 2 numbers
    hashNum = first2Letters[0]-'0' + first2Letters[1]-'0';     // get a number according to first letters
    //printf("2 numbers\n");
  }
  else if((first2Letters[0] >= '0' && first2Letters[0] <= '9') && (!(first2Letters[1] >= '0' && first2Letters[1] <= '9'))){        // case: 2 first chars = 1 number , 1 letter
    hashNum = first2Letters[0]-'0' + first2Letters[1]-'a';     // get a number according to first letters
    //printf("1 number, 1 letter\n");
  }
  else if((!(first2Letters[0] >= '0' && first2Letters[0] <= '9')) && ((first2Letters[1] >= '0' && first2Letters[1] <= '9'))){        // case: 2 first chars = 1 letter , 1 number
    hashNum = first2Letters[0]-'a' + first2Letters[1]-'0';     // get a number according to first letters
    //printf("1 letter, 1 number\n");
  }
  else{                       // case: 2 first chars = 2 letters
    hashNum = first2Letters[0]-'a' + first2Letters[1]-'a';     // get a number according to first letters
    //printf("1: %d , 2: %d\n", first2Letters[0]-'a' , first2Letters[1]-'a');
    //printf("2 letters\n");
  }
  entryNum = hashNum % HashtableNumOfEntries;
  //printf("Word %s and Entrynum: %d\n", hashName, entryNum);
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
  //printf("Found word: %d\n", found_word);
  return found_word;
}






int seachWordInVocabulary(struct VocabEntry* hashTable, int HashtableNumOfEntries, int bucketSize, char* word,int entryNum, int delete){
  struct VocabBucket* currentBucket;
  int found_word = -1; //boolean flag

  //check if word alreaduy exists in vocabulary
  currentBucket = hashTable[entryNum].bucket;
  while(currentBucket != NULL && found_word == -1){
    for(int j = 0;j < currentBucket->isFull ;j++){               // search for word's bucket
      if(strcmp(currentBucket->words[j].word,word) == 0){   //found word's place in bucket
        found_word = currentBucket->words[j].num; //raise flag
        /*if(delete == 1){
          free(currentBucket->words[j].word);
          currentBucket->words[j].word = NULL;
          currentBucket->isFull--;
          totalVocabularyWords--;
          found_word = currentBucket->words[j].num;
        }
        else{
          currentBucket->words[j].freq++;
        }*/
        currentBucket->words[j].freq++;
        break;
      }
    }
    currentBucket = currentBucket->nextBucket;
  }
  //printf("Found word: %d\n", found_word);
  return found_word;
}







int addWordInVocabulary(char* word, int HashtableNumOfEntries, struct VocabEntry* HashTable, int bucketSize){
  int entryNum; //key
  int length; //length of spec_id
  struct VocabBucket* currentBucket;
  char* hashName; //string given for hashing <- spec_id
  int bucket_place;
  int found_word;

  //printf("HERE\n");

  hashName = word;
//  entryNum = hashFunction1(HashtableNumOfEntries,totalVocabularyWords);                        // find correct entry for new word
  entryNum = hashFunction2(HashtableNumOfEntries,hashName);                        // find correct entry for new word
  length = strlen(hashName) + 1;

  found_word = seachWordInVocabulary(HashTable, HashtableNumOfEntries, bucketSize, word, entryNum,0);   // check if word already exists in vocabulary

  if(found_word > -1){
    return found_word;
  }

  //printf("EntryNum: %d\n", entryNum);
  currentBucket = HashTable[entryNum].bucket;                                     //use entry and point to proper bucket
  while(currentBucket->nextBucket != NULL){                                       //loop chain of buckets
    currentBucket = currentBucket->nextBucket;                                      //find the first free bucket to put the word
  }
  if(currentBucket->isFull == bucketSize){                                        //if the first free bucket is full,
    currentBucket->nextBucket = malloc(sizeof(struct Bucket));                      //next bucket needs to be allocated,
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
  //printf("Word: %s\n", currentBucket->words[bucket_place].word);
}


/*
void deleteWordFromVocabulary(char* word, int HashtableNumOfEntries, struct VocabEntry* HashTable, int bucketSize){
  int numOfWord;
  struct VocabBucket* currentBucket;
  struct RareWord* newRareWord;

  //numOfWord = seachWordInVocabulary(HashTable, HashtableNumOfEntries, bucketSize, word, entryNum,1);   // find word in vocabulary
  if(numOfWord == -1){
    printf("Can't find word %s in vocabulary\n",word);
    return;
  }
  newRareWord = malloc(sizeof(struct RareWord));
  newRareWord->delNum = numOfWord;
  newRareWord->nextWord = rareWordsList;
  rareWordsList = newRareWord;
}
*/
