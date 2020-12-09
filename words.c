#include "words.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void add_string_beg(struct WordList** list, char* word ){

    struct WordList *new;
    new = malloc(sizeof(struct WordList));
    new->word = malloc(sizeof(char) * (strlen(word)+1) );
    new->next = NULL;

    if(list == NULL){    //list is empty - add first node
        *list = new;
    }
    else{   //add node at the beginning of the list
        new->next = *list;
        *list = new;
    }

}



void add_string_end(struct WordList** list, char* word ){

    struct WordList *new;
    new = malloc(sizeof(struct WordList));
    new->word = malloc(sizeof(char) * (strlen(word)+1) );
    strcpy(new->word, word);
    new->next = NULL;

    //printf("%s\n", new->word);

    if(*list == NULL){    //list is empty - add first node
        *list = new;
        return;
    }

    struct WordList *temp = *list;
    while (temp->next!=NULL){
        temp = temp->next;
    }
    
    temp->next = new;
    
    return;

}
