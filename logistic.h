#ifndef __LOGISTIC__
#define __LOGISTIC__


#define TRAIN 1
#define TEST 2


void logistic_regression(float trainSize,struct Entry* hashTable,char* argv);
void init_w(float *weight);
float *update_w(float *weight,float *gradient);
float f(float *w, float *x);
float sigmoid(float f_x);
void gradient(float **gradients,float *weight,float *input, int label);
void feedModel (struct Entry* hashTable, char* argv, float *weight, float trainSize, char mode, char* y_true, float** y_fpred);
float* get_model_input(char* specid1, char* specid2, struct Entry* hashTable, char* argv);
void threshold(float** y_fpred,float trainSize);
void evaluate(char* y_true, float* y_pred,float trainSize);
void print_scores(int tp, int fp, int tn, int fn, float f1);
void test(struct Entry* hashTable, char* argv, float *weight,float trainSize);
float* get_model_input(char* specid1, char* specid2, struct Entry* hashTable, char* argv);
void thresholdSingle(float* y_fpred);
void test2(FILE *dataset, struct Entry* hashTable, char* argv, float b, float *weights,float trainSize);
void testDefault (FILE *dataset, struct Entry* hashTable, char* argv, float b, float *weights,float trainSize);


#endif
