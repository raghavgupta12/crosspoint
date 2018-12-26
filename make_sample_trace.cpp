#include "crosspoint.h"

using namespace std;

#define NUM_OPS      10

/*
    *** BIT LAYOUT ***
    MSB
    2 bits SEL (granularity select)
    3 bits REQ
    9 bits bank
    15 bits row2
    10 bits col2
    15 bits row1
    10 bits col1 
    LSB

    SEL:
    0 = Row (two rows)
    1 = Col (two cols)
    2 = Cell same row (cell-level)
    3 = Cell same col

    REQ:
    0 = read
    1 = write
    2 = not
    3 = or
*/

int main(){

  
  FILE * fp;
  unsigned long long bytes_write;
  unsigned int data[16];
  
  unsigned long long req, sel, bank, rowOne, colOne, rowTwo, colTwo;
  
  fp = fopen("trace.bin", "wb");
  
  for (int i = 0; i < NUM_OPS; i++){
    sel     = 0;
    req     = rand() % 4;
    bank    = rand() % NUM_BANKS;
    rowOne  = rand() % NUM_ROWS;
    colOne  = rand() % NUM_COLS;
    rowTwo  = rand() % NUM_ROWS;
    colTwo  = rand() % NUM_COLS;

    if(req == 2) { //NOT
      if((rowOne == rowTwo) && (colOne != colTwo))
        sel = 2;
      else if((colOne == colTwo) && (rowOne != rowTwo))
        sel = 3;
      else
        sel = rand() % 2;
    }

    if(req == 3) { //OR
      if(rowOne != rowTwo)
        sel = 0;
      else if(colOne != colTwo)
        sel = 1;
      else
        req = rand() % 2;
    }

    for(int iter = 0; iter < 16; iter++){
      data[iter] = 0xFFFFFFFF;
    }
   
    bytes_write = (sel<<62) | (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (rowOne<<10) | (colOne);

    #ifdef DEBUG
    printf("sel = %llu, req = %llu, bank = %llu, rowOne = %llu, colOne = %llu, rowTwo = %llu, colTwo = %llu\n",
      sel, req, bank, rowOne, colOne, rowTwo, colTwo);
    #endif 

    fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    if (req == 1){
      fwrite(&data[0], sizeof(unsigned int), 16, fp);
    }

    /*
    //for non read requests, create an additional read request afterwards to check data
    if (req != 0){
      req = 0;
      bytes_write = (req<<59) | (bank<<50) | (rowTwo<<35) | (colTwo<<25) | (rowOne<<10) | (colOne);
      fwrite(&bytes_write, sizeof(unsigned long long), 1, fp);
    }*/
  }
  
  fclose(fp);
  return 1;
}



