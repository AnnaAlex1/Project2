#ifndef __QUICK__
#define __QUICK__



void quicksort(float *array, int *pos_array, int low, int high);
int partition(float *array, int *pos_array, int low, int high);
int median_index(float* array, int low, int high);
void swap(float *pos1, float *pos2);

/*
void quicksort2(int *array, int low, int high);
int partition2(int *array, int low, int high);
int median_index2(int* array, int low, int high);*/
void swapint(int *pos1, int *pos2);




#endif