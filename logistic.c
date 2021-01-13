#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"
#include "readX.h"
#include "hashTable.h"
#include "result.h"
#include "readW.h"
#include "BOW.h"
#include "words.h"
#include "tfidf.h"
#include "bowvector.h"
#include "logistic.h"
#include "collision.h"

#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


extern int sizeOfDatasetW;
extern int finalVectorsSize;

extern int lines0;
extern int lines1;

#define e 0.00002



int last_update = 0;
int epochs = 200;

int ysize = 0;
float b = (float)0.0;
float a = 0.0088;



void logistic_regression(float trainSize,struct Entry* hashTable,char* argv){
  float *weight;


  weight = malloc(sizeof(float)*(finalVectorsSize+1));        // array to keep weights and value of b
  init_w(weight);                             // initialise weights

  feedModel (hashTable, argv, weight, trainSize, TRAIN, NULL, NULL);     //model training
  test(hashTable, argv, weight, trainSize);    // model testing
  free(weight);    // release weights array

}





//////////////////////////////////////////////////////////////////////////////
/////////////////////////TRAINING

void init_w(float *weight){                 // give initial values to weight array
  for(int i=0;i<finalVectorsSize;i++){
    weight[i] = (float)0.0;
  }
  weight[finalVectorsSize] = b;
}



float *update_w(float *weight,float *gradient){ //updating the weights
  float updated_w;
  float w_dif = (float)0.0;


  for(int i=0;i<finalVectorsSize;i++){
    updated_w = weight[i]- (a*gradient[i]);                   // update weight
    w_dif += (updated_w-weight[i])*(updated_w-weight[i]);      // starting calculating the difference between the updated and the previous weights
    weight[i] = updated_w;                                    // save new value in weight array
  }

  updated_w = weight[finalVectorsSize]- (a*gradient[finalVectorsSize]);      // update b value
  weight[finalVectorsSize] = updated_w;                                     // save new b value in weight array
  b = weight[finalVectorsSize];                                           // update public b value

  w_dif = sqrt(w_dif);                          // find the difference between the updated and the previous weights
  epochs--;                                     // one repetition ended -> reduce epochs

  if( epochs == 0 || w_dif < e){          // if final condition is reached, weights shouln't be updated again (last update = 1)
    last_update = 1;
  }

  return weight;              // return the updated weights
}


float f(float *w, float *x){
  float result = b;

  for(int i=0;i<finalVectorsSize;i++){        //f(x) = b + w1*x1 + w2*x2 + ... + wn*xn  , n = finalVectorsSize
    result += (float)(x[i]*w[i]);
  }
  return result;        //return f(x)
}



float sigmoid(float f_x){

  return (float)1.0 / (float)((float)1.0 + (float)exp(-f_x));             // return 1/(1+e^(-f(x)))
}



void gradient(float **gradients,float *weight,float *input, int label){
  float prediction;

  prediction = sigmoid(f(weight,input));                          // find prediction for specific input

  for(int j=0;j<finalVectorsSize;j++){
    (*gradients)[j] = (float)((prediction-(float)label)*input[j]);
  }
  (*gradients)[finalVectorsSize] = (float)(prediction-(float)label);                      // calculate derivative db
}



void feedModel (struct Entry* hashTable, char* argv, float *weight, float trainSize, char mode, char* y_true, float** y_fpred)
{
    int label;
    char left_spec_id[70];
    char right_spec_id[70];

    float *input;
    float *gradients;

    static FILE *output0, *output1;
    FILE *weights_file;

  //  int lines = (int)(((float)sizeOfDatasetW)*trainSize);     //number of input vectors
    int train_lines0 =  (int)(((float)lines0)*trainSize);
    int train_lines1 =  (int)(((float)lines1)*trainSize);

    int curline0 = 0;
    int curline1 = 1;
    int turn = 0;

    switch (mode)
    {
            case TRAIN:
                    printf("Training Size Percentage: %f\n", trainSize);

                    //Opening files to get pair of specs and their correlation
                    output0 = fopen("./output0.csv","r");
                    output1 = fopen("./output1.csv","r");

                  //  while (currentLine <= lines)  //for every input vector
                    while (curline0 < train_lines0 || curline1 <= train_lines1 ){
                            if(turn == 0 || curline1 > train_lines1){
                              label = readFromOutput(0, output0, output1, left_spec_id, right_spec_id, train_lines0, curline0);
                              curline0++;
                              turn = 1;
                            }
                            else{
                              label = readFromOutput(1, output0, output1, left_spec_id, right_spec_id, train_lines1, curline1);
                              curline1++;
                              turn = 0;
                            }

                            if (label == ENOENT) return; //if end of file, return


                            input = get_model_input(left_spec_id, right_spec_id, hashTable, argv);  //get model input
                            gradients = malloc(sizeof(float)*(finalVectorsSize+1));
                            //Calculation of partial derivatives
                            gradient(&gradients,weight,input,label);
                            free(input);

                            //Updating of the weights
                            weight = update_w(weight,gradients);
                            free(gradients);

                            //if the training is done (small difference between new weights and past weights or reached maximum number of repetitions)
                            if(last_update == 1){

                              printf("\nBest Weights Found.\n");
                              weights_file = fopen("./best_weights.txt","w");   //open file for saving weights

                              if(weights_file == NULL){
                                printf("Can't create file\n");
                                exit(EXIT_FAILURE);
                              }

                              for(int j=0;j<finalVectorsSize+1;j++){       //save weights in the file
                                fprintf(weights_file,"%f\n",weight[j]);
                              }

                              fclose(weights_file);                       //close file

                              break;

                            }


                    }//end of while
                    fclose(output0);
                    fclose(output1);

                    return;


            case TEST:

                //    datasetW = fopen("./sigmod_medium_labelled_dataset.csv","r");
                    output0 = fopen("./output0.csv","r");
                    output1 = fopen("./output1.csv","r");

                    /////////MODEL TESTING
                    struct PredStruct *pred_struct;
                    struct PredList *pred_list = NULL;
                    ////////////////////////////

                    for (int cc=0; cc < train_lines0 ;cc++){
                      //label = readDatasetW(datasetW, left_spec_id, right_spec_id, sizeOfDatasetW, cc);
                      label = readFromOutput(0, output0, output1, left_spec_id, right_spec_id, lines0, cc);

                      if (label==ENOENT) return;
                    }



                    for (int cc=1; cc <= train_lines1 ;cc++){
                      //label = readDatasetW(datasetW, left_spec_id, right_spec_id, sizeOfDatasetW, cc);
                      label = readFromOutput(1, output0, output1, left_spec_id, right_spec_id, lines1, cc);

                      if (label==ENOENT) return;
                    }


                    //GET WEIGHTS
                    int i=0;
                    float test_ws[finalVectorsSize+1];          // array to keep best weights and b (will be found in best_weights.txt)

                    weights_file = fopen("./best_weights.txt","r");   //open file for getting weights
                    int count = 0;
                    while(fscanf(weights_file,"%f",&test_ws[count]) != EOF){      // save weights and b in test_ws
                      count++;
                    }
                    fclose(weights_file);                       //close file

                    b = test_ws[finalVectorsSize];          // update public b based on test_ws

                //    while (lines < sizeOfDatasetW)

                    while (train_lines0 < lines0 || train_lines1 < lines1 ){
                            if(turn == 0 || train_lines1 == lines1){
                              label = readFromOutput(0, output0, output1, left_spec_id, right_spec_id, lines0, train_lines0);
                              train_lines0++;
                              turn = 1;
                            }
                            else{
                              label = readFromOutput(1, output0, output1, left_spec_id, right_spec_id, lines1, train_lines1);
                              train_lines1++;
                              turn = 0;
                            }

                        //    label = readFromOutput(i, output0, output1, left_spec_id, right_spec_id, sizeOfDatasetW, lines);

                            if (label==ENOENT) return;

                            input = get_model_input(left_spec_id, right_spec_id, hashTable, argv);  //get model input

                            (*y_fpred)[i] = sigmoid(f(test_ws,input));


                            ///////PREDICTIONS
                            pred_struct = set_PredStruct(hashTable, left_spec_id, right_spec_id, (*y_fpred)[i]);

                            if ( !collisions(*pred_struct, pred_list) ){
                                addInPredictions(&pred_list, pred_struct);
                            }
                            else{
                              free(pred_struct->specid1);
                              free(pred_struct->specid2);
                              free(pred_struct);
                            }
                            ///////////////////////////

                            free(input);
                           i++;
                    }

                    fclose(output0);
                    fclose(output1);


                    /////////PREDICTIONS
                    print_predictions(pred_list);
                    free_PredList(&pred_list);
                    ///////////////////////////


                    return;
    }

}




/////////////////////////////////////////////////////////////////////////////////
/////////////////TESTING

void threshold(float** y_fpred,float trainSize)
{
  int totalLines = lines0 + lines1;

        int size = (int)(((float)totalLines)*((float)1-trainSize));
        for (int i=0; i<size; ++i) {

                if ((float)(*y_fpred)[i]>=(float)0.5) {
                  (*y_fpred)[i]=(float)1;
                }
                else{
                  (*y_fpred)[i]=(float)0;
                }
        }
}


void evaluate(char* y_true, float* y_fpred,float trainSize)
{
    float f1;           //range: [0.0,1.0]
    float recall;       //range: [0.0,1.0]
    float precision;    //range: [0.0,1.0]

    int truePositives = 0;      //counter: model predicts correctly the positive class
    int falsePositives = 0;     //counter: model predicts correctly the negative class
    int trueNegatives = 0;      //counter: model predicts incorrectly the positive class
    int falseNegatives = 0;     //counter: model predicts incorrectly the negative class
    int label;

    char left_spec_id[70];
    char right_spec_id[70];

    static FILE *output0, *output1;

    int totalLines = lines0 + lines1;

    int lines = (int)(((float)totalLines)*trainSize);     //number of input vectors
    int ysize = (int)(((float)totalLines)*((float)1-trainSize));

    //datasetW = fopen("./sigmod_medium_labelled_dataset.csv","r");
    output0 = fopen("./output0.csv","r");
    output1 = fopen("./output1.csv","r");

    for (int cc=0; cc < lines;cc++){
      label = readFromOutput(cc, output0, output1, left_spec_id, right_spec_id, totalLines, cc);
      //label = readDatasetW(datasetW, left_spec_id, right_spec_id, sizeOfDatasetW, cc);
      if (label==ENOENT) return;
    }

    for (int i=0; i<ysize; ++i) {
        //label = readDatasetW(datasetW, left_spec_id, right_spec_id, sizeOfDatasetW, lines);
        label = readFromOutput(i, output0, output1, left_spec_id, right_spec_id, totalLines, lines);

        if (label==ENOENT) return;
        if (label==0) //Negative
        {
            if (y_fpred[i]==(float)0) //is correctly predicted
                trueNegatives++;
            else //is incorrectly predicted
                falseNegatives++;
        }
        else    //Positive
        {
            if (y_fpred[i]==(float)1) //is correctly predicted
                truePositives++;
            else //is incorrectly predicted
                falsePositives++;
        }
        lines++;
    }

    recall = ((float)truePositives) / ((float)(truePositives + falseNegatives));
    precision = ((float)truePositives) / ((float)(truePositives + falsePositives));

    f1 = (float)2.0 * ((precision*recall)/(precision+recall));

    print_scores(truePositives,falsePositives,trueNegatives,falseNegatives,f1);

    fclose(output0);
    fclose(output1);

    return;

}


void print_scores(int tp, int fp, int tn, int fn, float f1)
{
    printf("\n-------------- Evaluation of model --------------\n\n");
    printf("True Positives:     %d\n",tp);
    printf("False Positives:    %d\n",fp);
    printf("True Negatives:     %d\n",tn);
    printf("False Negatives:    %d\n",fn);
    printf("\nF1-score:           %f\n",f1);
}



void test(struct Entry* hashTable, char* argv, float *weight,float trainSize)
{
        float* y_fpred = NULL;                  // y predicted from logistic regression
      //  char* y_pred = NULL;                    // y with threshold
        char* y_true = NULL;                      // real y
        int totalLines = lines0 + lines1;
      //  y_fpred = malloc(sizeof(float) * ((float)sizeOfDatasetW*((float)1-trainSize)));
        y_fpred = malloc(sizeof(float) * ((float)totalLines*((float)1-trainSize)));

        feedModel(hashTable,argv,weight,trainSize,TEST,y_true,&y_fpred);

        threshold(&y_fpred,trainSize);

        evaluate(y_true,y_fpred,trainSize);

        free(y_true);
        free(y_fpred);

}

float* get_model_input(char* specid1, char* specid2, struct Entry* hashTable, char* argv){

  struct Spec *spec1, *spec2;

  if (strcmp(specid1, specid2) == 0) printf("The same!!\n");

  spec1 = findSpecInHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, specid1);
  spec2 = findSpecInHashTable(hashTable, NUM_OF_ENTRIES, BUCKET_SIZE, specid2);
  float *input = malloc(sizeof(float) * finalVectorsSize);





  for (int i=0; i<finalVectorsSize; i++){
    input[i] = (float)(spec1->vector[i]);
  }

   for (int i=finalVectorsSize; i<2*finalVectorsSize; i++){
    input[i] = (float)(spec2->vector[i]);
  } 

  return input;

}


void thresholdSingle(float* y_fpred)
{
    if ((float)(*y_fpred) >= (float)0.5) {
        (*y_fpred)=(float)1;
    }
    else{
        (*y_fpred)=(float)0;
    }
}


void test2(FILE *dataset, struct Entry* hashTable, char* argv, float b, float *weights,float trainSize)
{

    int status;
    char left_spec_id[50];
    char right_spec_id[50];
    float* input;

    int sizeOfDataset = getDataSetSize(dataset);
    int flag = 0, i=0;
    float y_fpred[sizeOfDataset];   // y predicted from logistic regression

    FILE *predictions = fopen("./test_predictions.csv","w");

    rewind(dataset);

    //checkHashTable(hashTable,NUM_OF_ENTRIES,BUCKET_SIZE);



    while ((status = readDataset(dataset,left_spec_id,right_spec_id,&flag)) != EOF)
    {
            if (status==ENOENT) return;

            input = get_model_input(left_spec_id, right_spec_id, hashTable, argv);  //get model input

            y_fpred[i] = sigmoid(f(weights,input));
            thresholdSingle(&(y_fpred[i]));
            free(input);

            fprintf(predictions,"%s,%s,%d\n",left_spec_id,right_spec_id,(int)y_fpred[i]);

            i++;
    }

    fclose(dataset);
    fclose(predictions);


}


void testDefault (FILE *dataset, struct Entry* hashTable, char* argv, float b, float *weights,float trainSize)
{

    int status;
    int sizeOfDataset = 0;
    char left_spec_id[50];
    char right_spec_id[50];

    float* input;
    int flag = 0, i=0;

    sizeOfDataset = getDataSetSize(dataset);
    float y_fpred[sizeOfDataset];   // y predicted from logistic regression

    rewind(dataset);
    FILE *predictions = fopen("./test_predictions.csv","w");


    while ((status = readTestFromDatasetW(dataset,left_spec_id,right_spec_id,&flag,trainSize,sizeOfDataset)) != EOF)
    {

            if (status==ENOENT) return;

            input = get_model_input(left_spec_id, right_spec_id, hashTable, argv);  //get model input

            y_fpred[i] = sigmoid(f(weights,input));
            thresholdSingle(&(y_fpred[i]));
            free(input);

            fprintf(predictions,"%s,%s,%d\n",left_spec_id,right_spec_id,(int)y_fpred[i]);

            i++;
    }

    fclose(dataset);
    fclose(predictions);

}
