#ifndef __BOW__
#define __BOW__

struct RareWord{
  int delNum;
  struct RareWord *nextWord;
};

struct VocabWord{
  char *word;   //the actual word
  int num;      //its corresponding position in the vectors
  int freq;     //its frequency
};

//bucket
struct VocabBucket{
  struct VocabBucket* nextBucket;  //pointer to next bucket (in case of overflow)
  int isFull;   //current number of BucketSpecs inside bucket => place in bucket to add a new spec
  struct VocabWord *words;
};

// hash table entry
struct VocabEntry{
  int key;
  struct VocabBucket* bucket;  // pointer to bucket
};


int addWordInVocabulary(char* , int , struct VocabEntry* , int );
int addWordInLocalVocabulary(char* , int , struct VocabEntry* , int );
int searchWordInVocabulary(struct VocabEntry*, int, int, char*, int);
int getWordPosition(struct VocabEntry* hashTable, int HashtableNumOfEntries, int bucketSize, char* word);
void initialiseVocabBucket(struct VocabBucket* ,int );
void initializeVocabHashTable(struct VocabEntry* ,int ,int );
void fixVectors(int*);
void freeVocabulary(struct VocabEntry* hashTable,int numOfEntries,int bucketSize);
void checkvoc(struct VocabEntry* vocabulary);


int hashFunction2(int HashtableNumOfEntries,char* hashName);



#endif
