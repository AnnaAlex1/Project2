#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "readX.h"
#include "collision.h"
#include "ids_list.h"
#include "hashTable.h"




// 1. For every prediction, get heads of specids' cliques, store them along with the prediction
struct PredStruct *set_PredStruct(struct Entry* hashTable, char* specid1, char* specid2, float y){

    struct PredStruct *pred_struct;


    pred_struct = malloc(sizeof( struct PredStruct));
    pred_struct->specid1 = malloc(sizeof(char) * (strlen(specid1)+1) );
    strcpy(pred_struct->specid1, specid1);

    pred_struct->specid2 = malloc(sizeof(char) * (strlen(specid2)+1) );
    strcpy(pred_struct->specid2, specid2);


    pred_struct->head1 = get_head(hashTable, specid1);//search for specid1 and return head1 address
    pred_struct->head2 = get_head(hashTable, specid2);//search for specid2 and return head2 address

    pred_struct->prediction = y;

    return pred_struct;
}




// 2. For evey prediction, check if there is a collision, return true if there is
bool collisions(struct PredStruct pred_struct, struct PredList *predictions){

    bool result = false, result2 = false;    //true if there is a collision, false if it's okay to keep


    //check if the prediction agrees with the formed cliques & negative correlations
    result = col_in_dataset(pred_struct);
    if(result){
      return true;
    }

    //Check for collission between the prediction and the former predictions
    result2 = col_in_predictions(pred_struct, predictions);

   return result2;
}



// 3a. Check for collision between the prediction and the formed cliques & neg_cor (return true for collision)
bool col_in_dataset(struct PredStruct pred_struct){

    if ( pred_struct.prediction >= 0.5 ){  //case: predicts class 1
        //if there is a negative correllation between the cliques: collision
       if ( in_neglist(pred_struct.head1->list_neg_cor, pred_struct.head2) ) return true;

    } else {    //case: predicts class 0 (not the same item/not in the same clique)
        if ( pred_struct.head1 == pred_struct.head2 ) return true;  //if we know they are in the same clique, collision

    }

    return false;

}



// 3b. Check for collission between the prediction and the former predictions
bool col_in_predictions(struct PredStruct pred_struct, struct PredList *predictions){

    if(predictions == NULL){
      return false;
    }

    float ac_prediction = pred_struct.prediction;

    float ab_prediction, bc_prediction;


    char a[50];
    strcpy(a, pred_struct.specid1);
    char b[50];
    char c[50];
    strcpy(c, pred_struct.specid2);


    bool b_found = false;


    // Search in list of predictions for a,b or b,a (b some specid, basically search for any prediction for a)
    struct PredList* temp_a = predictions;
    struct PredList* temp_c = predictions;
    struct PredList* temp_next_a;
    struct PredList* temp_next_c;

    while (temp_a != NULL){

        if ( strcmp(a, temp_a->pred_Struct->specid1) == 0 ){    //if we find a prediction for a, b
            strcpy(b,temp_a->pred_Struct->specid2);
            ab_prediction = temp_a->pred_Struct->prediction;
        } else if ( strcmp(a, temp_a->pred_Struct->specid2) == 0 ){ //if we find a prediction for b, a
            strcpy(b,temp_a->pred_Struct->specid1);
            ab_prediction = temp_a->pred_Struct->prediction;
        } else {                                               //a not found: check next prediction on list
            temp_a = temp_a->next;
            continue;
        }



        // A prediction for a was found
        temp_c = predictions;
        b_found = false;

        //Check for a prediction for c and b
        while (temp_c != NULL){

            //if left spec is c and right spec is b

            if ( (strcmp(temp_c->pred_Struct->specid1, c) == 0) && (strcmp(temp_c->pred_Struct->specid2, b) == 0) ){
                //prediction for c and b found
                b_found = true;
                bc_prediction = temp_c->pred_Struct->prediction;
                break;
            }

            //or if left spec is b and right spec is c
            if ( (strcmp(temp_c->pred_Struct->specid1, b) == 0) && (strcmp(temp_c->pred_Struct->specid2, c) == 0) ){
                //prediction for c and b found
                b_found = true;
                bc_prediction = temp_c->pred_Struct->prediction;
                break;
            }
            temp_c = temp_c->next;
        }



        float ab_diff, bc_diff, ac_diff;
        char max;


        //A prediction for c and b was found
        if ( b_found ){
            if ( ac_prediction >= 0.5 ){    //case: a,c to be classified as the same item


                    ac_diff = 1.0 - ac_prediction;  //difference from prediction 1

                    //must be a,b = 0  c,b = 0 or a,b = 1 c,b = 1
                    if ( (ab_prediction < 0.5) && (bc_prediction >= 0.5) ){
                        ab_diff = ab_prediction;
                        bc_diff = 1.0 - bc_prediction ;
                        //printf("Collision found\n");
                    } else if ( (ab_prediction >= 0.5) && (bc_prediction < 0.5) ){
                        //printf("Collision found\n");
                        ab_diff = 1.0 - ab_prediction;
                        bc_diff = bc_prediction ;

                    } else {
                        //printf("No collision!\n");
                        temp_a = temp_a->next;
                        continue;   //no collision here!
                    }

            } else {         //case: a,c to be classified as different items

                ac_diff = ac_prediction;

                //must be a,b = 1  c,b = 0   or   a,b = 0 c,b = 1   or   a,b = 0 c,b = 0
                if ( (ab_prediction >= 0.5) && (bc_prediction >= 0.5) ){

                    ab_diff = 1.0 - ab_prediction;
                    bc_diff = 1.0 - bc_prediction;

                } else {
                    //printf("No collision!\n");
                    temp_a = temp_a->next;
                    continue;   //no collision here!
                }

            }


            max = findgreater(ac_diff, ab_diff, bc_diff);


            if ( max == '1' ){
                //don't put current prediction in list
                return true;    //collision!
            } else if (max == '2') {
                temp_next_a = temp_a->next;
                removeFromPredictions(&predictions, temp_a);
                temp_a = temp_next_a;
                continue;
            } else {
                temp_next_c = temp_c->next;
                removeFromPredictions(&predictions, temp_c);
                temp_c = temp_next_c;
            }

            //return false; //no collision

        }



        temp_a = temp_a->next;
    }

    return false; //no collison was found

}



// 4a. Add in list of predictions if there was no collision
void addInPredictions(struct PredList **predictions, struct PredStruct *pred_struct){

    struct PredList *new_node;
    new_node = malloc(sizeof(struct PredList));
    new_node->pred_Struct = pred_struct;


    if(predictions == NULL){    //list is empty - add first node
        *predictions = new_node;

    } else { //add node at the beginning of the list
        new_node->next = *predictions;
        *predictions = new_node;
    }


}


// 4b. Remove from list of predictions if there is a collision (and it's the weakest prediction)
void removeFromPredictions(struct PredList **predictions, struct PredList *pred_node){

    struct PredList *temp = *predictions;
    struct PredList *prev = NULL;

    if (temp == NULL){
      printf("null\n");
        return;
    }

    if ( temp == pred_node )  //remove first node of list
    {
        *predictions = temp->next;   //set 2nd node as first
        free(temp);   //free temp --> first node
        return;
    }


    while ( (temp != NULL) && ( temp != pred_node ))  //loop until finding node to delete
    {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {  //didn't find the node
      printf("null 2\n");
        return;
    }

    prev->next = temp->next;  //skip node that you will delete
    free(temp); //free that node

    return;


}




char findgreater(float ac_diff, float ab_diff, float bc_diff){

    if (ac_diff >= ab_diff && ac_diff >= bc_diff) return ac_diff;
    if (ab_diff >= ac_diff && ab_diff >= bc_diff) return ab_diff;

    return bc_diff;

    //if they have the same value, return the first you find
}




// 5. Print list
int print_predictions( struct PredList *predictions ){

    FILE* output_file;
    output_file = fopen("./predictions.csv", "w");
    if ( output_file == NULL ){
        perror("Unsuccesful creation of file 'predictions.csv' \n ");
        return 1;
    }


    while(predictions != NULL){

        if (predictions->pred_Struct->prediction >= 0.5){
            fprintf(output_file, "%s, %s, %d\n", predictions->pred_Struct->specid1, predictions->pred_Struct->specid2, 1);
        }else {
            fprintf(output_file, "%s, %s, %d\n", predictions->pred_Struct->specid1, predictions->pred_Struct->specid2, 0);
        }

        predictions = predictions->next;
    }

    fclose(output_file);
    return 0;

}




// 6. Free list
void free_PredList(struct PredList **predictions){

    struct PredList *temp, *to_free;
    temp = *predictions;

    while ( temp != NULL){

        to_free = temp;
        temp = temp->next;

        free(to_free->pred_Struct->specid1);
        free(to_free->pred_Struct->specid2);
        free(to_free->pred_Struct);
        free(to_free);

    }


}
