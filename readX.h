#ifndef __READX__
#define __READX__

#include "hashTable.h"
#include "words.h"
#include <stdio.h>
#include <stdbool.h>

struct Pair {
  struct WordList* category;
  struct WordList* details;
};


struct Spec {
  char *spec_id;
  int num_of_fields;
  struct Pair fields;
  //struct Pair* words;
  //struct WordList* words;
  //int *vector;
};


void set_pair(struct Spec *spec, char *first_part, char *second_part);
void set_spec(struct Spec *spec, FILE *spec_file, char* dir_name, char* file_name);
void print_spec(struct Spec spec);
char *extract_id(char * dir_name, char *file_name);
int read_datasetX(int HashtableNumOfEntries, struct Entry* HashTable, int bucketSize);
void free_spec(struct Spec *spec);
int endofword(char current, bool flag);

#endif
