#ifndef __READW__
#define __READW__

#define BSIZE 105
#define LEFT 0
#define RIGHT 1
#define NUM_OF_SPLITS 2

int readDatasetW_1 (FILE*, char*, char*);
int readDatasetW_0 (FILE*, char*, char*);
void split_specid(char**, char*);
int greaterSpecId (char*, char*);
int readOutputCsv (FILE*, char*, char*);
void free_specid_parts(char** s);

#endif