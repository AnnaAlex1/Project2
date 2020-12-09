#ifndef __WORDS__
#define __WORDS__



struct WordList{
  char *word;
  struct WordList* next;
};


void add_string_beg(struct WordList**, char*);
void add_string_end(struct WordList**, char*);
//when it adds in vocabulary, then it returns the position so vector knows where to be 1


#endif