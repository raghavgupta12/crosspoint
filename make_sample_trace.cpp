#include "crosspoint.h"

using namespace std;

#define NUM_OPS      10

/*  
4  bits 0

3  bits REQ, REQ = 0 -> READ, REQ = 1 -> WRITE, REQ = 2 -> NOT, REQ = 3 -> OR
3  bits bank

15 bits row2
10 bits col2
15 bits row1
10 bits col1

4  bits 0
                                                                              
*/ 

int main(){

  
  FILE * fp;
  unsigned long long bytes_write;
  unsigned int data[16];
  
  unsigned long long req, bank, rowOne, colOne, rowTwo, colTwo;
  
  fp = fopen("trace.bin", "wb");
  
  for (int i = 0; i < NUM_OPS; i++){
 
    req     = rand() % 4;
    bank    = rand() % NUM_BANKS;
    rowOne  = rand() % NUM_ROWS;
    colOne  = rand() % NUM_COLS;
    rowTwo  = rand() % NUM_ROWS;
    colTwo  = rand() % NUM_COLS;

    if (req == 3){
      if (   ((rowOne == rowTwo) && (colOne == colTwo))
	  || ((rowOne != rowTwo) && (colOne != colTwo))) {
	req = rand() % 3;
      }
    }
      
    for(int iter = 0; iter < 16; iter++){
      data[iter] = 0xFFFFFFFF;
    }
   
    bytes_write = (req<<57) | (bank<<54) | (rowTwo<<39) | (colTwo<<29) | (rowOne<<14) | (colOne<<4) | 0x0;

    #ifdef DEBUG
    printf("req = %llu, bank = %llu, rowOne = %llu, colOne = %llu, rowTwo = %llu, colTwo = %llu\n",
	    req, bank, rowOne, colOne, rowTwo, colTwo);
    #endif 

    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    if (req == 1){
      fwrite(&data[0], sizeof(unsigned int), 16, fp);
    }

    if (req != 0){
      req = 0;
      bytes_write = (req<<57) | (bank<<54) | (rowTwo<<39) | (colTwo<<29) | (rowOne<<14) | (colOne<<4);
      fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    }
  }
  
  fclose(fp);
  return 1;
}



