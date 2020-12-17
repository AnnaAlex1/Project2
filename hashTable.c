#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ids_list.h"
#include "readX.h"
#include "readW.h"
#include "hashTable.h"

extern struct AccessCliques* aclist;  //list of pointers to the first element of every spec_clique <--- LIST 2

void initialiseBucket(struct Bucket* currentBucket,int bucketSize){   //initialises a bucket & allocates memory for its content
  currentBucket->isFull = 0;
  currentBucket->nextBucket = NULL;
  currentBucket->bucket_specs = malloc(sizeof(struct BucketSpec)*bucketSize);

}


void initializeHashTable(struct Entry* hashTable,int numOfEntries,int bucketSize){
  for(int i=0;i<numOfEntries;i++){
    hashTable[i].key = i; //set key to a bucket
    hashTable[i].bucket = malloc(sizeof(struct Bucket));  //allocate bucket
    initialiseBucket(hashTable[i].bucket,bucketSize); //initialise bucket
  }
}

int sumDigits(char* numStr) //calculates sum of digits of a given number
{
  int sum = 0;
  int num = atoi(numStr); //convert string to int

  while (num)
  {
    sum += num%10;
    num = num/10;
  }

  return sum;

}

int hashFunction(int HashtableNumOfEntries,char* hashName)  //hashing method: isolate numeric part of spec_id -> calculate sum of its digits -> mod HashtableNumOfEntries
{
  char* hashName_parts[NUM_OF_SPLITS];
  split_specid(hashName_parts,hashName);

  int hashNum = sumDigits(hashName_parts[NUM_OF_SPLITS-1]);
  int entryNum = hashNum % HashtableNumOfEntries;

  free_specid_parts(hashName_parts);
  return entryNum;

}

void insertInHashTable(struct Spec* spec, int HashtableNumOfEntries, struct Entry* HashTable, int bucketSize){
  int entryNum; //key
  int length; //length of spec_id
  struct Bucket* currentBucket;
  char* hashName; //string given for hashing <- spec_id
  int bucket_place;

  hashName = spec->spec_id;


  entryNum = hashFunction(HashtableNumOfEntries,hashName);                        // find the correct entry for the new spec
  length = strlen(hashName) + 1;
  currentBucket = HashTable[entryNum].bucket;                                     //use entry and point to proper bucket

  while(currentBucket->nextBucket != NULL){                                       //loop chain of buckets
    currentBucket = currentBucket->nextBucket;                                      //find the first free bucket to put the id
  }
  if(currentBucket->isFull == bucketSize){                                        //if the first free bucket is full,
    currentBucket->nextBucket = malloc(sizeof(struct Bucket));                      //next bucket needs to be allocated,
    currentBucket = currentBucket->nextBucket;                                      //pointed and
    initialiseBucket(currentBucket,bucketSize);                                     //initialised
    bucket_place = 0;                                                               //hold place of spec in bucket (first spec in bucket => place = 0)
  }
  else if(currentBucket->isFull < bucketSize){                                    //next free bucket already allocated
    bucket_place = currentBucket->isFull;                                           //hold place of spec in bucket
  }

  //create & insert BucketSpec
  currentBucket->bucket_specs[bucket_place].spec = malloc(sizeof(struct Spec)); //allocate memory for spec
  memcpy( currentBucket->bucket_specs[bucket_place].spec, spec, sizeof(struct Spec)); //copy spec

  currentBucket->bucket_specs[bucket_place].head = malloc(sizeof(struct AccessCliques));
  currentBucket->bucket_specs[bucket_place].head->clique = malloc(sizeof(struct Clique));
  currentBucket->bucket_specs[bucket_place].head->clique->id = malloc(length);
  memcpy(currentBucket->bucket_specs[bucket_place].head->clique->id,hashName,length); //copy spec_id to matching list
  currentBucket->bucket_specs[bucket_place].head->clique->next_element = NULL;
  currentBucket->bucket_specs[bucket_place].head->clique->mat_spec = &(currentBucket->bucket_specs[bucket_place]);  //clique points back to inserted BucketSpec (1-1 relation)
  currentBucket->bucket_specs[bucket_place].head->list_neg_cor = NULL;
  currentBucket->bucket_specs[bucket_place].head->next_id = NULL;
  addIdInList2(currentBucket->bucket_specs[bucket_place].head, &aclist); //update list of cliques - add new one

  /*currentBucket->bucket_specs[bucket_place].matching_ids = malloc(sizeof(struct Clique));   //create list for matching ids (clique)
  currentBucket->bucket_specs[bucket_place].matching_ids->id = malloc(length);
  memcpy(currentBucket->bucket_specs[bucket_place].matching_ids->id,hashName,length); //copy spec_id to matching list
  currentBucket->bucket_specs[bucket_place].matching_ids->next_element = NULL;
  currentBucket->bucket_specs[bucket_place].matching_ids->mat_spec = &(currentBucket->bucket_specs[bucket_place]);  //clique points back to inserted BucketSpec (1-1 relation)
*/
//addIdInList2(currentBucket->bucket_specs[bucket_place].matching_ids, &aclist); //update list of cliques - add new one

  currentBucket->isFull++; //adjust place for incoming BucketSpec
}


void searchSimilar(struct Entry* hashTable, int HashtableNumOfEntries, int bucketSize, char* id,char* dest_id){
  struct Bucket* currentBucket;
  int found_dest = 0; //boolean flag
  struct Clique* dest_matching_list = NULL;
  struct Clique* origin_matching_list = NULL;
  int entryNum; //key
  struct AccessCliques* dest_head = NULL;

  //searching for destination's clique
  entryNum = hashFunction(HashtableNumOfEntries, dest_id);

  currentBucket = hashTable[entryNum].bucket;
  while(currentBucket != NULL && found_dest == 0){
    for(int j = 0;j < currentBucket->isFull ;j++){               // search for destination's bucket
      if( strcmp(currentBucket->bucket_specs[j].spec->spec_id,dest_id) == 0){   //found destination spec's place in bucket
        found_dest = 1; //raise flag
        dest_matching_list = currentBucket->bucket_specs[j].head->clique;       //dest_matching_list points to the beginning of destination clique
        dest_head = currentBucket->bucket_specs[j].head;
        //printf("bucket:%d, pos:%d, spec:%s beginning of dest clique: %s\n",entryNum, j, dest_id, dest_matching_list->id );
        break;
      }
    }
    currentBucket = currentBucket->nextBucket;
  }


  if(found_dest == 1){

    int entryNum = hashFunction(HashtableNumOfEntries, id);

    currentBucket = hashTable[entryNum].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){   //search for origin spec's bucket

        if(strcmp(currentBucket->bucket_specs[j].spec->spec_id,id) == 0){ //found origin spec's place in bucket
          origin_matching_list = currentBucket->bucket_specs[j].head->clique;         //origin_matching_list pointing to the beginning of origin clique
          if(origin_matching_list == dest_matching_list){
            return;
          }
          else{
            //printf("\n bucket:%d, pos:%d, spec:%s, origin_matching_list: %s\n\n",entryNum, j, id, origin_matching_list->id );
            addIdInList(origin_matching_list, dest_matching_list);                     // add origin_matching_list at the end of dest_matching_list
            deleteIdFromList2(currentBucket->bucket_specs[j].head->clique, &aclist);     // delete origin's clique from list of cliques
            //currentBucket->bucket_specs[j].matching_ids = dest_matching_list;
            fixing_pointers(origin_matching_list, dest_head);           // make all pointers of origin clique to point to destination's clique
            return;
          }
        }
      }
      currentBucket = currentBucket->nextBucket;
    }
  }
  else{
    printf("destination id %s not found\n",dest_id);
  }
}



void searchDifferent(struct Entry* hashTable, int HashtableNumOfEntries, int bucketSize, char* id,char* dest_id){
  struct Bucket* currentBucket;
  int found_dest = 0; //boolean flag
  struct AccessCliques* dest_head = NULL;
  struct AccessCliques* origin_head = NULL;
  int entryNum; //key

  //searching for destination's clique
  entryNum = hashFunction(HashtableNumOfEntries, dest_id);

  currentBucket = hashTable[entryNum].bucket;
  while(currentBucket != NULL && found_dest == 0){
    for(int j = 0;j < currentBucket->isFull ;j++){               // search for destination's bucket
      if( strcmp(currentBucket->bucket_specs[j].spec->spec_id,dest_id) == 0){   //found destination spec's place in bucket
        found_dest = 1; //raise flag
        dest_head = currentBucket->bucket_specs[j].head;       //dest_head points to the beginning of destination clique
        //printf("bucket:%d, pos:%d, spec:%s beginning of dest clique: %s\n",entryNum, j, dest_id, dest_head->id );
        break;
      }
    }
    currentBucket = currentBucket->nextBucket;
  }


  if(found_dest == 1){

    int entryNum = hashFunction(HashtableNumOfEntries, id);

    currentBucket = hashTable[entryNum].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){   //search for origin spec's bucket

        if(strcmp(currentBucket->bucket_specs[j].spec->spec_id,id) == 0){ //found origin spec's place in bucket
          origin_head = currentBucket->bucket_specs[j].head;         //origin_head pointing to the beginning of origin clique
          if(origin_head == dest_head){
            printf("Wrong data !!!\n");
            return;
          }
          else{
            //printf("\n bucket:%d, pos:%d, spec:%s, origin_head: %s\n\n",entryNum, j, id, origin_head->id );

            addNegInList(&(origin_head->list_neg_cor), dest_head);                     // add dest head to list of negative correlations of origin head
            addNegInList(&(dest_head->list_neg_cor), origin_head);                     // add origin head to list of negative correlations of dest head

            return;
          }
        }
      }
      currentBucket = currentBucket->nextBucket;
    }
  }
  else{
    printf("destination id %s not found\n",dest_id);
  }
}



void fixing_pointers(struct Clique * origin_clique, struct AccessCliques* dest_head){
// make all pointers from origin _clique point to dest_head
    struct Clique* temp;

    temp = origin_clique;
    while (temp!=NULL){
      temp->mat_spec->head = dest_head;
      temp = temp->next_element;

    }



}




void freeHashTable(struct Entry* hashTable,int numOfEntries,int bucketSize){
  struct Bucket* currentBucket;
  struct Bucket* nextTempBucket;

  for(int i = 0;i < numOfEntries;i++){                //for every entry in the hashtable
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      nextTempBucket = currentBucket->nextBucket;                               // remove data from bucket_specs
      for(int j = 0;j < currentBucket->isFull; j++){
        free_spec(currentBucket->bucket_specs[j].spec);
        free(currentBucket->bucket_specs[j].spec);
      }
      free(currentBucket->bucket_specs);
      free(currentBucket);
      currentBucket = nextTempBucket;
    }
    currentBucket = hashTable[i].bucket;
  }

}



void checkHashTable(struct Entry* hashTable,int numOfEntries,int bucketSize){
  struct Bucket* currentBucket;
  int numOfBucket;
  for(int i = 0;i < numOfEntries;i++){
    numOfBucket = 1;
    currentBucket = hashTable[i].bucket;
    while(currentBucket != NULL){
      for(int j = 0;j < currentBucket->isFull;j++){
        printf("Entry: %d, Bucket: %d, place in bucket: %d/%d id: %s\n", i, numOfBucket, j, bucketSize, currentBucket->bucket_specs[j].spec->spec_id);
      }
      currentBucket = currentBucket->nextBucket;
      numOfBucket++;
    }
  }
}




