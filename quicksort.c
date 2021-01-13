#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quicksort.h"


//QUICKSORTING TF_IDF_MEAN TABLE (DESCENDING)
void quicksort(float *array, int *pos_array, int low, int high){

    if (low >= high){
        return;
    }

    int par_index = partition(array,pos_array, low, high);

    quicksort(array, pos_array, low, par_index-1);
    quicksort(array, pos_array, par_index + 1, high);

}



int partition(float *array, int *pos_array,int low, int high){

    //choice of pivot: median-of-three
    int mid_value_pos = median_index(array, low, high);     //returns the position of the middle value

    //take pivot element to the last position of the array
    swap( &(array[high]), &array[mid_value_pos]);
    if (pos_array != NULL)
        swapint( &(pos_array[high]), &pos_array[mid_value_pos]);

    int pivot = array[high];

    //indexes
    int i = low -1;
    int j = low;

    while ( j<high){

        if ( array[j] > pivot ){

            i++;
            swap( &array[i], &array[j] );
            if (pos_array != NULL)
                swapint( &pos_array[i], &pos_array[j] );
        }


        j++;
    }

    //put pivot in the right position
    swap( &array[i+1], &array[high]  );
    if (pos_array != NULL)
        swapint( &pos_array[i+1], &pos_array[high] );

    return i+1;

}

int median_index(float* array, int low, int high){

    float first = array[0];
    float second = array[high/2];
    float third =  array[high-1];


    if (first <= second && first<=third ){
        return low;
    } else if ( second <= first && second <= third){
        return high/2;
    } else {
        return high;
    }

}


void swap(float *pos1, float *pos2){

    float temp;
    temp = *pos1;
    *pos1 = *pos2;
    *pos2 = temp;

}

void swapint(int *pos1, int *pos2){

    int temp;
    temp = *pos1;
    *pos1 = *pos2;
    *pos2 = temp;

}


/*
//QUICKSORT POSITIONS (ASCENDING)
void quicksort2(int *array, int low, int high){

    if (low >= high){
        return;
    }

    int par_index = partition2(array, low, high);

    quicksort2(array, low, par_index-1);
    quicksort2(array, par_index + 1, high);

}



int partition2(int *array, int low, int high){

    //choice of pivot: median-of-three
    int mid_value_pos = median_index2(array, low, high);     //returns the position of the middle value

    //take pivot element to the last position of the array
    swapint( &(array[high]), &array[mid_value_pos]);

    int pivot = array[high];

    //indexes
    int i = low -1;
    int j = low;

    while ( j<high){

        if ( array[j] < pivot ){

            i++;
            swapint( &array[i], &array[j] );
        }


        j++;
    }

    //put pivot in the right position
    swapint( &array[i+1], &array[high]  );

    return i+1;

}

int median_index2(int* array, int low, int high){

    int first = array[0];
    int second = array[high/2];
    int third =  array[high-1];


    if (first <= second && first<=third ){
        return low;
    } else if ( second <= first && second <= third){
        return high/2;
    } else {
        return high;
    }

}

*/




