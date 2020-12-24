#include <stdio.h>
#include <stdbool.h>

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


int print_results_all(struct AccessCliques *start){

    FILE *output_file, *output_file0;
    struct Clique *node1, *node2, *node3;
    struct NegCorrel *nlist, *back;

    output_file = fopen("./output1.csv", "w");
    if ( output_file == NULL ){
        perror("Unsuccesful creation of file 'output.csv' \n ");
        return 1;
    }

    output_file0 = fopen("./output0.csv", "w");
    if ( output_file0 == NULL ){
        perror("Unsuccesful creation of file 'output.csv' \n ");
        return 1;
    }

    fprintf(output_file, "left_spec_id, right_Spec_id\n");
    fprintf(output_file0, "left_spec_id, right_Spec_id\n");



//for each start of a clique
    while( start != NULL){                                      //take start of clique

        
        node1 = start->clique;
        nlist = start->list_neg_cor;


        if (nlist == NULL){/*printf("No negative corellations with this clique!"); start = start->next_id; continue;*/}
        if (node1 == NULL) {printf("No elements in this clique!\n"); start = start->next_id; continue;}                            //case: empty list
        if ( node1->next_element == NULL){ //printf("Only one spec for this item!\n"); 
                                     start = start->next_id; continue; }             //case: only one spec for this item
        
        //for each element 
        while ( node1 != NULL) {              //case: 2 or more specs referring to the same item              //if the current node is atmost the second from the end
            
            nlist = start->list_neg_cor;

            if (node1->next_element != NULL){ //except last one
                
                //printing spec couples that are correlated
                node2 = node1->next_element; 

                //for element+1 to last one
                while ( node2 != NULL){
                    
                    fprintf(output_file, "%s, %s, 1\n", node1->id, node2->id);
                    
                    node2 = node2->next_element;

                }

            }



            //printing spec couples that are correlated negatively
            
            while (nlist != NULL){ //for every node in list of cliques that our clique is negatively correlated with

                ///////////////////
                if (nlist->done==true){
                    nlist=nlist->next_negcor;
                    continue;
                }

                //setting done=true for pointer pointing back to our node
                back = nlist->aclist->list_neg_cor;
                while (back->aclist != start){
                    back = back->next_negcor;
                }
                back->done=true;
                ///////////////////

                //take the first node of a clique that it is neg. cor. with
                node3 = nlist->aclist->clique;

                //for element in list
                while ( node3 != NULL){
                    
                    fprintf(output_file0, "%s, %s, 0\n", node1->id, node3->id);
                    
                    node3 = node3->next_element;

                }


                nlist=nlist->next_negcor;
            }


        
            //next specid
            node1 = node1->next_element;
            
        }

        start = start->next_id;                          //next clique
    }

    fclose(output_file0);
    fclose(output_file);
    return 0;

}

/*


int print_results_all(struct AccessCliques *start){

    FILE *output_file, *output_file0;
    struct Clique *node1, *node2, *node3;
    struct NegCorrel *nlist, *back;
    //struct AccessCliques *nlist;
    int num_of_lines0 = 0;

    output_file = fopen("./output1.csv", "w");
    if ( output_file == NULL ){
        perror("Unsuccesful creation of file 'output.csv' \n ");
        return 1;
    }

    output_file0 = fopen("./output0.csv", "w");
    if ( output_file0 == NULL ){
        perror("Unsuccesful creation of file 'output.csv' \n ");
        return 1;
    }

    fprintf(output_file, "left_spec_id, right_Spec_id\n");
    fprintf(output_file0, "left_spec_id, right_Spec_id\n");

    //for each start of a clique
   
    while( start != NULL){                                      //take start of clique

        

        node1 = start->clique;
        nlist = start->list_neg_cor;


        //if (nlist == NULL){printf("No negative corellations with this clique!"); start = start->next_id; continue;}
        if (node1 == NULL) {printf("No elements in this clique!\n"); start = start->next_id; continue;}                            //case: empty list
        if ( node1->next_element == NULL){ //printf("Only one spec for this item!\n"); 
                                     start = start->next_id; continue; }             //case: only one spec for this item
        
        //for each element (except last one)
        while ( node1 != NULL) {                     //case: 2 or more specs referring to the same item              //if the current node is atmost the second from the end
            
            if (node1->next_element != NULL){
                
                //printing spec couples that are correlated
                node2 = node1->next_element; 

                //for element+1 to last one
                while ( node2 != NULL){
                    
                    fprintf(output_file, "%s, %s, 1\n", node1->id, node2->id);
                    
                    node2 = node2->next_element;

                }

            }



            //printing spec couples that are correlated negatively
            
            while (nlist != NULL){ //for every node in list of cliques that our clique is negatively correlated with

                ///////////////////
               if (nlist->done==true){
                    nlist=nlist->next_negcor;
                    continue;
                }

                //setting done=true for pointer pointing back to our node
                back = nlist->aclist->list_neg_cor;
                while (back->aclist != start){
                    back = back->next_negcor;
                }
                back->done=true;
                ///////////////////

                //take the first node of a clique that it is neg. cor. with
                node3 = nlist->aclist->clique;

                //for element in list
                while ( node3 != NULL){
                    
                    fprintf(output_file0, "%s, %s, 0\n", node1->id, node3->id);
                    num_of_lines0++;

                    if (num_of_lines0 > 1000000){

                        fclose(output_file0);
                        output_file0 = fopen("./output0(2).csv", "w");
                        if ( output_file0 == NULL ){
                            perror("Unsuccesful creation of file 'output.csv' \n ");
                            return 1;
                        }
                        num_of_lines0 = 0;
                    }
                    
                    node3 = node3->next_element;

                }


                nlist=nlist->next_negcor;
            }


        
            //next specid
            node1 = node1->next_element;
            
        }

        start = start->next_id;                          //next clique
    }


    fclose(output_file0);
    fclose(output_file);
    return 0;

}


*/