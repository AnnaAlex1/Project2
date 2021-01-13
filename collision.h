#ifndef __COLLIS__
#define __COLLIS__

#include <stdbool.h>

#include "hashTable.h"
#include "ids_list.h"


struct PredStruct{
    char* specid1;                       //first specid of the prediction
    char* specid2;                       //second specid of the prediction   
    struct AccessCliques *head1;         //head pointing to the specid1's clique
    struct AccessCliques *head2;         //head pointing to the specid2's clique
    float prediction;                    //prediction for the specs
};

//list that holds predictions to keep
struct PredList{
    struct PredStruct* pred_Struct;
    struct PredList* next;
};


struct PredStruct *set_PredStruct(struct Entry* hashTable, char* specid1, char* specid2, float y);
bool collisions(struct PredStruct pred_struct, struct PredList *predictions);
bool col_in_dataset(struct PredStruct pred_struct);
bool col_in_predictions(struct PredStruct pred_struct, struct PredList *predictions);
void addInPredictions(struct PredList **predictions, struct PredStruct *pred_struct);
void removeFromPredictions(struct PredList **predictions, struct PredList *pred_node);
char findgreater(float ac_diff, float ab_diff, float bc_diff);
int print_predictions( struct PredList *predictions);
void free_PredList(struct PredList **predictions);




#endif
