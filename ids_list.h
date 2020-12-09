#ifndef __IDSLIST__
#define __IDSLIST__

//node of a list representing a clique of matching specs <--- LIST
struct Clique{
  char* id; //spec_id
  struct Clique* next_element;  //pointer to next node
  struct BucketSpec* mat_spec;  //pointer to BucketSpec (spec + its matching clique)
};

//node of a list containing pointers to the start of every clique <--- LIST 2
struct AccessCliques{
  struct Clique* clique;  //pointer to first element of clique
  struct NegCorrel *list_neg_cor; //pointer to the list of negative correlations
  struct AccessCliques* next_id;  //pointer to next node
};

//node of a list containing pointers to cliques that our clique correlates negatively with
struct NegCorrel{
  struct AccessCliques* aclist; //pointer to the head (AccessCliques node) of the clique
  struct NegCorrel* next_negcor; //pointer to next node
};

void addIdInList(struct Clique*,struct Clique*);          //adds node in the clique matching spec_ids (LIST)
void addIdInList2(struct AccessCliques*,struct AccessCliques**);       //adds node in the list of pointers to cliques' heads (LIST 2)
void deleteIdFromList2(struct Clique* ,struct AccessCliques**); //removes a pointer to clique's head from the list (LIST 2)
void release_Clique(struct AccessCliques* );              //release memory of cliques (LIST)
void release_aclist(struct AccessCliques**);              //release memory of list of pointers to cliques (LIST2)
void addNegInList(struct NegCorrel**, struct AccessCliques*);

#endif
