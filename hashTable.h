#ifndef __HASHTABLE__
#define __HASHTABLE__

#include "readX.h"
#include "ids_list.h"

//type of bucket's content
struct BucketSpec{
  struct Spec* spec;  //pointer to a spec
  //struct Clique* matching_ids;  //list of matching specs = clique of spec
  struct AccessCliques* head;              // ------------------------------------------------------------------------------------------ (αντι για Clique)
};

//bucket
struct Bucket{
  struct BucketSpec* bucket_specs;  //content of bucket: bucketsize*bucket_specs
  struct Bucket* nextBucket;  //pointer to next bucket (in case of overflow)
  int isFull;   //current number of BucketSpecs inside bucket => place in bucket to add a new spec
};

// hash table entry
struct Entry{
  int key;
  struct Bucket* bucket;  // pointer to bucket
};

void initializeHashTable(struct Entry*, int, int);  //initialises hashtable
int hashFunction(int, char*); //function to hash spec_ids for placing spec to bucket
void insertInHashTable(struct Spec*, int, struct Entry*, int);  //insert a spec in hashtable
void searchSimilar(struct Entry*, int, int, char*, char*);   // search position of 2 spec_ids inside hash table in order to merge their matching lists (cliques)
void searchDifferent(struct Entry* hashTable, int HashtableNumOfEntries, int bucketSize, char* id,char* dest_id);
void fixing_pointers(struct Clique*, struct AccessCliques*);  // when merging 2 lists: make all pointers of origin list point to destination list
void freeHashTable(struct Entry* hashTable,int numOfEntries,int bucketSize);
struct Spec* findSpecInHashTable(struct Entry* hashTable, int HashtableNumOfEntries, int bucketSize, char* id);

void checkHashTable(struct Entry* hashTable,int numOfEntries,int bucketSize);

#endif
