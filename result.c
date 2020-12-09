#include <stdio.h>

#include "result.h"
#include "ids_list.h"
#include "readW.h"


int print_results( struct AccessCliques *start ){

    FILE *output_file;
    struct Clique *node1, *node2;

    output_file = fopen("./output.csv", "w");
    if ( output_file == NULL ){
        perror("Unsuccesful creation of file 'output.csv' \n ");
        return 1;
    }


    fprintf(output_file, "left_spec_id, right_Spec_id\n");

    //for each start of a clique
   
    while( start != NULL){                                      //take start of clique

        

        node1 = start->clique;

    
        if (node1 == NULL) {printf("No elements in this clique!\n"); start = start->next_id; continue;}                            //case: empty list
        if ( node1->next_element == NULL){ /*printf("Only one spec for this item!\n"); */ start = start->next_id; continue; }             //case: only one spec for this item
        
        //for each element (except last one)
        while ( node1->next_element != NULL) {                     //case: 2 or more specs referring to the same item              //if the current node is atmost the second from the end
            
            node2 = node1->next_element; 

            //for element+1 to last one
            while ( node2 != NULL){
                
                fprintf(output_file, "%s, %s\n", node1->id, node2->id);
                
                node2 = node2->next_element;

            }

            node1 = node1->next_element;
            
        }

        start = start->next_id;                          //next clique
    }



    fclose(output_file);
    return 0;

}
