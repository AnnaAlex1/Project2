#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "readW.h"

int sizeOfDatasetW = 0;

int readDatasetW_1 (FILE* datasetW, char* left, char* right)
{
    int label = 0;
    char c = 0;
    char buffer[BSIZE];

    if (!datasetW)
    {
        perror("Opening dataset W failed");
        return ENOENT;
    }


    while ((c=fgetc(datasetW))!=EOF)
    {
        ungetc(c,datasetW);

        fgets(buffer,BSIZE,datasetW);

        strcpy(left,strtok(buffer,","));
        strcpy(right,strtok(NULL,","));
        label = atoi(strtok(NULL,","));

        if (label == 1){   // return this pair of specs
          sizeOfDatasetW++;
          break;            // else ignore unmatching pairs labeled by 0
        }
    }

    return c;
}

int readDatasetW_0 (FILE* datasetW, char* left, char* right)
{
    int label = 0;
    char c = 0;
    char buffer[BSIZE];

    if (!datasetW)
    {
        perror("Opening dataset W failed");
        return ENOENT;
    }


    while ((c=fgetc(datasetW))!=EOF)
    {
        ungetc(c,datasetW);

        fgets(buffer,BSIZE,datasetW);

        strcpy(left,strtok(buffer,","));
        strcpy(right,strtok(NULL,","));
        label = atoi(strtok(NULL,","));

        if (label == 0){   // return this pair of specs
          sizeOfDatasetW++;
          break;                // else ignore unmatching pairs labeled by 0
        }
    }

    return c;
}


int readDatasetW (FILE* datasetW, char* left, char* right, int lines, int currentLine)
{
    int label = 0;
    char buffer[BSIZE];

    if (!datasetW)
    {
        perror("Opening dataset W failed");
        return ENOENT;
    }

    if(currentLine <= lines){
      fgets(buffer,BSIZE,datasetW);

      if(currentLine == 0){
        fgets(buffer,BSIZE,datasetW);

      }
      strcpy(left,strtok(buffer,","));
      strcpy(right,strtok(NULL,","));
      label = atoi(strtok(NULL,","));
      return label;

    }
    return label;
}


void init_specid_parts(char** s)
{
  for (int i=0; i<NUM_OF_SPLITS; ++i) s[i]=NULL;

  return;

}

void free_specid_parts(char** s)
{
  for (int i=0; i<NUM_OF_SPLITS; ++i) free(s[i]);

  return;

}

void split_specid(char** specid_parts, char* specid)
{
  //create a local copy of specid
  char cp[strlen(specid) + 1];
  strcpy(cp,specid);

  //split copy of specid (cp) into: <link> - <code>
  init_specid_parts(specid_parts);                          //set all parts to NULL
  int i=0;
  char* split = strtok(cp,"//");

  while(split != NULL)
  {
    specid_parts[i] = malloc(strlen(split) + 1);
    strcpy(specid_parts[i++],split);
    split = strtok(NULL,"//");
  }
}

int greaterSpecId (char* left_spec_id, char* right_spec_id)
{
    //split spec_ids into: <link> - <code>
    char* lefts[NUM_OF_SPLITS];
    char* rights[NUM_OF_SPLITS];
    split_specid(lefts,left_spec_id);
    split_specid(rights,right_spec_id);

    /* Comparison of spec_ids:
        - firstly check if <link> parts are similar
    */
    int cmp = strcmp(lefts[0],rights[0]);
    if (!cmp) {  //left_link == right_link -> compare values of codes

      int codel = atoi(lefts[1]);
      int coder = atoi(rights[1]);
      free_specid_parts(lefts);
      free_specid_parts(rights);
      return (codel > coder) ? LEFT : RIGHT;

    } else {  //compare links (alphabetically)

      free_specid_parts(lefts);
      free_specid_parts(rights);
      return (cmp>0) ? LEFT : RIGHT;
    }

}





int readFromOutput(int choice, FILE* output0, FILE* output1, char* left, char* right, int lines, int currentLine)
{
    int label = 0;
    char buffer[BSIZE];


    if (!output0)
    {
        perror("Opening output0 failed");
        return ENOENT;
    }

    if (!output1)
    {
        perror("Opening output1 failed");
        return ENOENT;
    }

    if(currentLine <= lines){


        if ( choice%2 ){

            if ( fgets(buffer,BSIZE,output1) == NULL){
                fseek(output1,0,SEEK_SET);
                fgets(buffer,BSIZE,output1);
                currentLine = 1;
            }

            if(currentLine == 1){
                fgets(buffer,BSIZE,output1);
            }

        } else{

            if ( fgets(buffer,BSIZE,output0) == NULL){
                fseek(output0,0,SEEK_SET);
                fgets(buffer,BSIZE,output0);
                currentLine = 0;
            }

            if(currentLine == 0){
                fgets(buffer,BSIZE,output0);

            }
        }

      strcpy(left,strtok(buffer,", "));
      strcpy(right,strtok(NULL,", "));
      label = atoi(strtok(NULL,", "));
      return label;

    }
    return label;
}


int getDataSetSize (FILE* dataset)
{
    char c = 0;
    char buffer[BSIZE];
    int size = 0;

    if (!dataset)
    {
        perror("Opening dataset failed");
        return ENOENT;
    }

    fgets(buffer,BSIZE,dataset); //skip first line

    while ((c=fgetc(dataset))!=EOF)
    {
        ungetc(c,dataset);
        fgets(buffer,BSIZE,dataset);
        size++;
    }
    
    return size;
}


//read new dataset given by user for testing model
int readDataset (FILE* dataset, char* left, char* right, int *flag)
{
    char c = 0;
    char buffer[BSIZE];

    if (!dataset)
    {
        perror("Opening dataset failed");
        return ENOENT;
    }

    if ((*flag)==0)
    {
        fgets(buffer,BSIZE,dataset); //skip first line
        (*flag) = 1;
    }


    if ((c=fgetc(dataset))!=EOF)
    {
        ungetc(c,dataset);

        fgets(buffer,BSIZE,dataset);

        strcpy(left,strtok(buffer,","));
        strcpy(right,strtok(NULL,"\n"));

        return c;

    }

    return c;
}


int readTestFromDatasetW (FILE* dataset, char* left, char* right, int *flag, float trainSize, int size)
{

    char c = 0;
    char buffer[BSIZE];

    if (!dataset)
    {
        perror("Opening dataset failed");
        return ENOENT;
    }

    if ((*flag)==0)
    {
        fgets(buffer,BSIZE,dataset); //skip first line
        int lines = (int)(((float)size)*trainSize);     //number of lines to skip
        for (int i=0; i<lines; ++i) {
            fgets(buffer,BSIZE,dataset); //skip this line
        }
        (*flag) = 1;
    }


    if ((c=fgetc(dataset))!=EOF)
    {
        ungetc(c,dataset);

        fgets(buffer,BSIZE,dataset);

        strcpy(left,strtok(buffer,","));
        strcpy(right,strtok(NULL,","));
        strtok(NULL,"\n"); //skip label
        return c;

    }

    return c;
}