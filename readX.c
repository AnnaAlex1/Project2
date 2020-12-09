#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>

#include "readX.h"
#include "hashTable.h"
#include "words.h"


#define MAXBUF 51000

extern struct WordList* vocabulary;

int read_datasetX(int HashtableNumOfEntries, struct Entry* HashTable, int bucketSize){

  FILE *spec_file;
  

  DIR *maindr, *subdr;
  struct dirent *dsub_dir, *dfiles;


  struct Spec *spec;
  spec = malloc( sizeof(struct Spec));

  //open directory "2013_camera_specs"
  if ( (maindr = opendir("./camera_specs/2013_camera_specs")) == NULL ) { perror("Directory cannot open!\n"); return 1; }


  //for every subdirectory
  while ( (dsub_dir = readdir(maindr)) != NULL ){

                                    /*printf("%s\n", dsub_dir->d_name);*/

    if ( (strcmp(dsub_dir->d_name,".") != 0) && (strcmp(dsub_dir->d_name,"..") != 0) ){


                                    printf("Opening Directory: %s\n", dsub_dir->d_name);

        char *path;
        path = malloc(sizeof(char) * ( strlen(dsub_dir->d_name) + strlen("./camera_specs/2013_camera_specs/") + 1 ) );
        strcpy(path, "./camera_specs/2013_camera_specs/");
        strcat(path, dsub_dir->d_name);

                                    /*printf("diretory to open: %s\n", path);*/

        //open sub-directory
        if ( (subdr = opendir(path)) == NULL ) { perror("Sub-Directory cannot open!"); return 1; }


            //for every file in the sub-directory
            while ( (dfiles = readdir(subdr)) != NULL ){

                if ( (strcmp(dfiles->d_name,".") != 0) && (strcmp(dfiles->d_name,"..") != 0) ){

                                    /*printf("File: %s\n", dfiles->d_name);*/

                    char *filepath;
                    filepath = malloc(sizeof(char) * ( strlen(dfiles->d_name) + strlen(path) + 2 ) );         //+1 for the character '/'
                    strcpy(filepath, path);
                    strcat(filepath, "/");
                    strcat(filepath, dfiles->d_name);

                                    /*printf("file to open: %s\n", filepath);*/

                    //open file for spec
                    spec_file = fopen(filepath, "r");
                    if (spec_file == NULL){
                      perror("File cannot open!\n");
                      return 1;
                    }

                                                                            
                    set_spec(spec, spec_file, dsub_dir->d_name, dfiles->d_name); //passing file pointer, name of directory, file name
                    print_spec(*spec);

                    //give spec to create nodes
                    insertInHashTable( spec, HashtableNumOfEntries, HashTable, bucketSize);

                    //close file
                    fclose(spec_file);

                    free(filepath);

                }


            }



        closedir(subdr);

        free(path);

    }



  }

  
  free(spec);
  closedir(maindr);

  return 0;
}









void set_spec(struct Spec *spec, FILE *spec_file, char* dir_name, char* file_name){            //returns a struct of spec from the file given
  
  spec->spec_id = extract_id( dir_name, file_name );
  spec->num_of_fields = 0;
  spec->fields.category = NULL;
  spec->fields.details = NULL;

  printf("SPECID: %s\n", spec->spec_id);
  
  
  //////////
  char *line = malloc(sizeof(char) * 700);
  char first_part[200];
  char second_part[MAXBUF];
  char word[30];
 
  
  
  int i,j;
  char prev_ch, ch;
  char help = '\\';
  bool help_flag = false; //becomes true when 'we want to keep a character we normally wouldn't 
  //char temp = '0';  //helping character for handling '\n' in text
  bool changeword = false; //helping flag for handling '\n' in text
  int count_chars = -1; //to count characters (if -1 don't start counting)
  
  while ( (ch = fgetc(spec_file)) != EOF){                    
      prev_ch = ch;

      //skip first characters (until the start of the category)
      while( ( (ch = fgetc(spec_file)) != EOF) && ( (ch != '"') || ( (ch == '"') && (prev_ch == help) ) ) ){ prev_ch = ch; }
      if (ch == EOF){break;}



      ///////////////category////////////////////////////
      i = 0;
      j = 0;

      
      ch = prev_ch;                       //change value '"' in order to enter the loop 
      while( ( (ch = fgetc(spec_file)) != EOF) && ( (ch != '"') || ( (ch == '"') && (prev_ch == help) ) )  ){  //reading first part (category name)
          
          prev_ch = ch;
          first_part[i] = ch;
          i++; 

          if (ch == help){      //if character == '\' avoid it,but put next one in word
            continue;
          }

          if ( endofword(ch, help_flag) ){     /*end of word*/ 
            word[j] = '\0';
            if ( (strcmp(word, " ") != 0) && (strlen(word)!= 0) ){
              printf("word: %s\n", word);
              //PROCEDURE!!!
              j=0;
              continue;
            }
          }

          if ( !( (j == 0) && endofword(ch, help_flag)) ){    //if the first character is not a symbol
            word[j] = ch;
            j++;
          }     
          
          
         
        
      }

      word[j] =   '\0';
      if (strlen(word) != 0){
        printf("wordend: %s\n", word);
        //PROCEDURE!!!
      }
      first_part[i] = '\0';
      
      ///////////////////


      if ( (ch = fgetc(spec_file)) != ':') {
        
        printf("Error: no ':' after!\n" );
        
        return;
      }

      //skip characters (until the start of the details)
      while( ( (ch = fgetc(spec_file)) != EOF) && ( (ch != '"') || ( (ch == '"') && (prev_ch == help) ) ) && (ch != '[')    ){ prev_ch = ch;}






      ///////////////details//////////////////////////
      i = 0;
      j = 0;
      if ( ch == '['){                          //reading second part (details)

          
          while(  ( (ch = fgetc(spec_file)) != EOF) &&  (( ch != ',')  ||  prev_ch != ']')){
            
            //checking characters and setting the needed flags
            set_flags(prev_ch, ch, &help_flag, &changeword);


            
            //for setting the struct Spec
            prev_ch = ch;
            second_part[i] = ch;
            i++;

                        

            //case: '\u' found
            if (ch == 'u' && changeword){   
              count_chars = 0;      //start counting
            }

            if (count_chars == 5){  //no more unicode characters (stop skipping characters)
              count_chars = -1;       //finished counting for now
            } else if (count_chars != -1){  //case \u
              count_chars++;
              continue;
            } 

            //case: character '\' found
            if ( ch == help ){   //continue to next interation
              continue;                    
            }


            //extract word for list of words
            if ( endofword(ch, help_flag) || changeword){     //if it's the end of word
              word[j] = '\0';
              if ( (strcmp(word, " ") != 0) && (strlen(word)!= 0) ){
                printf("      word2: %s\n", word); 
                //PROCEDURE!!!!!!!!!
                j=0;
                changeword = false;
                continue;

              }

            }

            help_flag = false;

            //put character in word
            if ( !( (j == 0) && endofword(ch, help_flag)) && !changeword && count_chars == -1){    //if the first character is not a symbol
              word[j] = ch;
              j++;
            } 
            
            changeword = false;       
          }

          //case: last word of string (last word of details)
          word[j] =   '\0';
          if (strlen(word) != 0){
            printf("      word2end: %s\n", word);
            //PROCEDURE!!!
          }
        


      } else if (ch == '"'){

          ch = prev_ch;                       //change value '"' in order to enter the loop 
          while(  ( (ch = fgetc(spec_file)) != EOF) && ( (ch != '"') || ( (ch == '"') && (prev_ch == help) ) )  ){ 
            
            //checking characters and setting the needed flags
            set_flags(prev_ch, ch, &help_flag, &changeword);


            
            //for setting the struct Spec
            prev_ch = ch;
            second_part[i] = ch;
            i++;

                        

            //case: '\u' found
            if (ch == 'u' && changeword){   
              count_chars = 0;      //start counting
            }

            if (count_chars == 5){  //no more unicode characters (stop skipping characters)
              count_chars = -1;       //finished counting for now
            } else if (count_chars != -1){  //case \u
              count_chars++;
              continue;
            } 

            //case: character '\' found
            if ( ch == help ){   //continue to next interation
              continue;                    
            }


            //extract word for list of words
            if ( endofword(ch, help_flag) || changeword){     //if it's the end of word
              word[j] = '\0';
              if ( (strcmp(word, " ") != 0) && (strlen(word)!= 0) ){
                printf("      word2: %s\n", word); 
                //PROCEDURE!!!!!!!!!
                j=0;
                changeword = false;
                continue;

              }

            }

            help_flag = false;

            //put character in word
            if ( !( (j == 0) && endofword(ch, help_flag)) && !changeword && count_chars == -1){    //if the first character is not a symbol
              word[j] = ch;
              j++;
            } 
            
            changeword = false;       
          }

          //case: last word of string (last word of details)
          word[j] =   '\0';
          if (strlen(word) != 0){
            printf("      word2end: %s\n", word);
            //PROCEDURE!!!
        }

      }

      second_part[i] = '\0';

      //put in fields
      set_pair(spec, first_part, second_part);      //put category and details to the spec
      
      spec->num_of_fields++;  

      
    
  }
  
  free(line);


}


char * extract_id(char * dir_name, char *file_name){

  char *spec_id = malloc( sizeof(char) * ( strlen(dir_name) + strlen(file_name) - 2 ) );      // +2 (for '//')  -5 (for '.json') + 1 (for '\0') =  -2

  int size_of_temp = sizeof(char) * (strlen(file_name) - 4) ;
  char temp[size_of_temp];
  temp[size_of_temp-1] = '\0';


  memcpy(temp, file_name, size_of_temp-1); //copy only the name (without .json or \0)

  strcpy(spec_id, dir_name);  //first the directory d_name
  strcat(spec_id, "//");      //then the characters '//'
  strcat(spec_id, temp);      //lastly, the filename without '.json'


  return spec_id;
}





void set_pair(struct Spec *spec, char *first_part, char *second_part){       //set strings in corresponding lists
  
  add_string_end( &(spec->fields.category), first_part);  //add string in list of words for category
  add_string_end( &(spec->fields.details), second_part);  //add string in list of words for details


}




void print_spec(struct Spec spec){

  printf("\nSpec_id: %s\n", spec.spec_id);
  printf("Num of fields: %d\n", spec.num_of_fields);

  struct WordList *category, *details;

  category = spec.fields.category;
  details = spec.fields.details;

  while (category != NULL && details != NULL){
    printf("  %s -> %s\n", category->word, details->word);
    category = category->next;
    details = details->next;
  }


}


void free_spec(struct Spec* spec){
  free(spec->spec_id);

  struct WordList *category, *details, *temp1, *temp2;

  category = spec->fields.category;
  details = spec->fields.details;



  while (category != NULL && details != NULL){
    temp1 = category;
    temp2 = details;
    free(category->word);
    free(details->word);
    category = category->next;
    details = details->next;
    free(temp1);
    free(temp2);
  }

}



int endofword(char current, bool flag){

  if (flag) return 0; //if flag == true, then we don't want to end the word with this character

  if ( (current >= 48 && current <= 57) ||      //if current character is a number
       ( current >= 65 && current <= 90 ) ||    //if current character is a capital letter
       ( current >= 97 && current <= 122 ) ){    //if current character is a lowercase letter
    return 0;   //it's not the end of the word
  }

  return 1;   //it's the end of the word

}



void set_flags(char prev_ch, char ch, bool* help_flag, bool* changeword){ //setting helping flags 

  char help = '\\';

  //don't allow entrance in if(endof...)statement, if previous character == '\', or a number is followed by fullstop
  if ( ((prev_ch == help) && (ch != 'n' && ch != 'u' ) )|| 
        (prev_ch >= 48 && prev_ch <= 57 && (ch == '.' || ch == ':'))    ){      
      
      *help_flag = true;                      
  
  } else if ( (prev_ch == help) && (ch == 'n') ){
     
      *changeword = true; //we found change of line
  } else if ( (prev_ch == help) &&  (ch == 'u') ){   //skip next for characters
    
      *changeword = true; //we found '\u'

  }

}



//PROCEDURE:

//if it's punctuation, continue;

//change capitals to lower case                                       void lower();
//if not a stopword - maybe                                             bool stopword();
//call functions for vocabulary and private list                    addword_beg(vocabulary);    addword_beg(private list);
//++ in vector,increase frequency or if not already in, add it             wordinvector();



//if it's not stopword
//change capital letters
//if it's not in private list of words
//add in private list of words
//if not already in vocabulary
//add in voacbulary
