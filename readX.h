#ifndef __READX__
#define __READX__

#include "hashTable.h"
#include "BOW.h"
#include <stdio.h>
#include <stdbool.h>

#define VOC_ENTRIES 200
#define VOC_BUCK_SIZE  1000

#define LOC_ENTRIES 10
#define LOC_BUCKETSIZE  100


struct WordList{      //list of words
  char *word;
  struct WordList* next;
};


struct Pair {       //pair of lists of words containing all the information found in the file
  struct WordList* category;
  struct WordList* details;
};


struct Spec {
  char *spec_id;          //spec's id
  int num_of_fields;      //number of category-details pairs
  struct Pair fields;     //lists of categories with their details
  int num_of_words;       //number of words found in the spec (without stopwords)

  int tf_size;            //size of the list tf

  float *vector;           //TF-IDF vector to be sent in model
  //char *vector;       //BOW vector to be sent in model

  struct VocabEntry * local_vocab;  //a list that holds all the important words(corresponding to bow & tfidf) and their frequency


};
//////////////////



void set_pair(struct Spec *spec, char *first_part, char *second_part);
void set_spec(struct Spec *spec, FILE *spec_file, char* dir_name, char* file_name, struct VocabEntry* vocabulary, FILE* stop_file,int HashtableNumOfEntries,int bucketSize);
void print_spec(struct Spec spec);
char *extract_id(char * dir_name, char *file_name);
int read_datasetX(int HashtableNumOfEntries, struct Entry* HashTable, int bucketSize, struct VocabEntry* vocabulary, FILE* stop_file);
void free_spec(struct Spec *spec);
int endofword(char current, bool flag);
void set_flags(char prev_ch, char ch, bool* help_flag, bool* changeword);

void procedure(struct Spec* spec, char* word, struct VocabEntry*,FILE* stop_file);
void add_string_end(struct WordList**, char*);



#endif
