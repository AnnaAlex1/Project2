#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "ids_list.h"

void addIdInList(struct Clique* origin_matching_list,struct Clique* dest_matching_list){

  struct Clique* dest_pointer;
  dest_pointer = dest_matching_list;

  //search last node - end of list
  while(dest_pointer->next_element != NULL){
    dest_pointer = dest_pointer->next_element;
  }

  //add new node at the end of the list
  dest_pointer->next_element = origin_matching_list;

  return;
}

void addIdInList2(struct AccessCliques* head,struct AccessCliques** accesscliques_list){

  if(accesscliques_list == NULL){    //list is empty - add first node
    *accesscliques_list = head;
  }
  else{   //add node at the beginning of the list
    head->next_id = *accesscliques_list;
    *accesscliques_list = head;
  }
}

void addNegInList(struct NegCorrel** list_neg_cor,struct AccessCliques* head){
  struct NegCorrel* new_node;

  new_node = malloc(sizeof(struct NegCorrel));
  new_node->aclist = head;
  new_node->done=false;

  if(list_neg_cor == NULL){    //list is empty - add first node
    new_node->next_negcor = NULL;
    *list_neg_cor = new_node;
  }
  else{   //add node at the beginning of the list
    new_node->next_negcor = *list_neg_cor;
    *list_neg_cor = new_node;
    //printf("After: %s\n", (*list_neg_cor)->aclist->clique->id);
  }


}


int in_neglist(struct NegCorrel* list_neg_cor,struct AccessCliques* head){

  while (list_neg_cor != NULL){
    if (list_neg_cor->aclist == head){
      return 1; //head of list found in negative correlations list
    }

    list_neg_cor = list_neg_cor->next_negcor;
  }

  return 0; //not in list
}



void deleteIdFromList2(struct Clique* removing_id,struct AccessCliques** aclist){


    struct AccessCliques* temp = *aclist;  //temporary pointer to the first node
    struct AccessCliques* prev = NULL;      //temporary pointer to the previous of current node

    if (temp != NULL && (strcmp( temp->clique->id,removing_id->id) == 0) )  //remove first node of list
    {
        *aclist = temp->next_id;   //set 2nd node as first
        free(temp);   //free temp --> first node
        return;
    }


    while (temp != NULL && (strcmp( temp->clique->id,removing_id->id) != 0))  //loop until finding node to delete
    {
        prev = temp;
        temp = temp->next_id;
    }


    if (temp == NULL)   //list is empty - cannot delete any node
        return;

    prev->next_id = temp->next_id;  //skip node that you will delete
    free(temp); //free that node

    return;

}


void release_Clique(struct AccessCliques* accesscliques_list){ //release memory of cliques

  struct Clique* temp, *to_free;

  while( accesscliques_list != NULL){

    temp = accesscliques_list->clique;
    while( temp != NULL){
      free(temp->id);
      to_free = temp;
      temp = temp->next_element;
      free(to_free);

    }

    accesscliques_list = accesscliques_list->next_id;
  }


}

void releaseNegCor(struct AccessCliques* head){
  struct NegCorrel* temp;
  struct NegCorrel* to_free;

  temp = head->list_neg_cor;

  while( temp != NULL){
    to_free = temp;
    temp = temp->next_negcor;
    free(to_free);
  }

}





void release_aclist(struct AccessCliques** accesscliques_list){

  struct AccessCliques* temp, *to_free;
  temp = *accesscliques_list;

  while( temp != NULL){

    to_free = temp;
    temp = temp->next_id;
    releaseNegCor(to_free);
    free(to_free);

  }
}
