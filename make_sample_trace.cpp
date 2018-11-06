#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <stdio.h>    


using namespace std;

#define NUM_OPS      15
#define NUM_BANKS    1
#define NUM_ROWS     2
#define NUM_COLS     2
#define DEBUG        

/*  
4  bits 0

3 bits REQ, REQ = 0 -> READ, REQ = 1 -> WRITE, REQ = 2 -> LOP
3 bits bank

15 bits row2
10 bits col2
15 bits row1
10 bits col1

1 bit row2Hit
1 bit col2Hit
1 bit row1Hit
1 bit col1Hit
                                                                              
*/ 

int main(){

  
  FILE * fp;
  unsigned long long bytes_write;
  unsigned int data[16];
  
  unsigned long long req, bank, rowOne, colOne, rowTwo, colTwo;
  
  unsigned long long rowOneHit, colOneHit;
  unsigned long long rowTwoHit, colTwoHit;
  
  fp = fopen("trace.bin", "wb");
  
  for (int i = 0; i < NUM_OPS; i++){
 
    req     = rand() % 3;
    bank    = rand() % NUM_BANKS;
    rowOne  = rand() % NUM_ROWS;
    colOne  = rand() % NUM_COLS;
    rowTwo  = rand() % NUM_ROWS;
    colTwo  = rand() % NUM_COLS;

    rowOneHit = rand() % 2;
    colOneHit = rand() % 2;
    rowTwoHit = rand() % 2;
    colTwoHit = rand() % 2;

    if (req == 2){
      if (   ((rowOne == rowTwo) && (colOne == colTwo))
	  || ((rowOne != rowTwo) && (colOne != colTwo))) {
	req = rand() % 2;
      }
    }
      
    
    for(int iter = 0; iter < 16; iter++){
      data[iter] = rand();
    }
   
    bytes_write = (req<<57) | (bank<<54) | (rowTwo<<39) | (colTwo<<29) | (rowOne<<14) | (colOne<<4) | (rowTwoHit<<3) | (colTwoHit<<2) | (rowOneHit<<1) | colOneHit;

    #ifdef DEBUG
    printf("req = %llu, bank = %llu, rowOne = %llu, colOne = %llu, rowTwo = %llu, colTwo = %llu, rowOneHit = %llu, colOneHit = %llu, rowTwoHit = %llu, colTwoHit = %llu\n",
	   req, bank, rowOne, colOne, rowTwo, colTwo,
	   rowOneHit, colOneHit, rowTwoHit, colTwoHit);
    #endif 

    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    
    if (req == 1){
      fwrite(&data[0], sizeof(unsigned int), 16, fp);
    }
  }
  
  fclose(fp);
  return 1;
}



