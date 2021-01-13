#ifndef __READW__
#define __READW__

#define BSIZE 105
#define LEFT 0
#define RIGHT 1
#define NUM_OF_SPLITS 2

int readDatasetW_1 (FILE*, char*, char*);
int readDatasetW_0 (FILE*, char*, char*);
int readDatasetW (FILE* , char* , char* , int , int);
void split_specid(char**, char*);
int greaterSpecId (char*, char*);
int readFromOutput(int, FILE*, FILE*, char*, char*, int, int);
void free_specid_parts(char** s);
int readDataset (FILE* dataset, char* left, char* right, int *flag);
int getDataSetSize(FILE* dataset);
int readTestFromDatasetW (FILE* datasetw, char* left, char* right, int *flag, float trainSize,int size);

#endif