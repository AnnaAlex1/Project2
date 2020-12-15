#ifndef __READX__
#define __READX__

#include "hashTable.h"
//#include "tfidf.h"
#include <stdio.h>
#include <stdbool.h>


struct WordList{
  char *word;
  struct WordList* next;
};



struct Pair {
  struct WordList* category;
  struct WordList* details;
};


struct Spec {
  char *spec_id;
  int num_of_fields;
  struct Pair fields;
  int num_of_words;     //number of words found in the spec (without stopwords)
  //double *tfidf;
  struct Tfidf *tfidf;
  int *bow;
  int tf_size;

  //struct Pair* words;
  //struct WordList* words;
  //char *vector;
};


void set_pair(struct Spec *spec, char *first_part, char *second_part);
void set_spec(struct Spec *spec, FILE *spec_file, char* dir_name, char* file_name);
void print_spec(struct Spec spec);
char *extract_id(char * dir_name, char *file_name);
int read_datasetX(int HashtableNumOfEntries, struct Entry* HashTable, int bucketSize);
void free_spec(struct Spec *spec);
int endofword(char current, bool flag);
void set_flags(char prev_ch, char ch, bool* help_flag, bool* changeword);


void add_string_beg(struct WordList**, char*);
void add_string_end(struct WordList**, char*);
//when it adds in vocabulary, then it returns the position so vector knows where to be 1



#endif
