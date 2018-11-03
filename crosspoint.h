#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

using namespace std;

#define RESET    "\033[0m"
#define RED      "\033[1m\033[31m"
#define GREEN    "\033[1m\033[32m"
#define YELLOW   "\033[1m\033[33m"
#define BLUE     "\033[1m\033[34m"
#define MAGENTA  "\033[1m\033[35m"
#define CYAN     "\033[1m\033[36m"
#define WHITE    "\033[1m\033[37m"
#define DARKBLUE "\033[1m\033[40m"


/************ TO CHANGE **************/
#define DEBUG 

#define DATA_SIZE    16

#define NUM_BANKS    8
#define NUM_ROWS     8
#define NUM_COLS     8

#define SEL_PWR      5
#define SEL_LAT      5

#define RD_PWR       6
#define RD_LAT       6

#define WR_PWR       7
#define WR_LAT       7

#define LOP_PWR      8
#define LOP_LAT      8
/*************************************/

                                

enum requestType   {READ, WRITE, LOP};
enum policyType    {OPEN, CLOSE};


const string requestString[]       = {"READ", "WRITE", "LOP"};
const string policyString[]        = {"OPEN", "CLOSE"};


class stats{
 public:

  float total_energy;
  float average_energy;
  float average_power;
  float total_latency;
  float average_latency;

  float count;
  
  void init_values(void);
  void update_stats(float, float, float);
  void print_stats(void);
}; 


class bufferData{
 public:
  unsigned int data[DATA_SIZE];
  void init_values(void);
};


class RRAMdata{
 public:
  unsigned int data[DATA_SIZE];
  void init_values(void);
};



class RRAMspec {

 public:
  policyType POLICY; //Options are OPEN and CLOSE
  
 protected:
  int   numBanks, numRows, numCols;

  float requestTime, requestPower, requestEnergy;
  
  stats requestStats;
  
  /* Buffer */
  bufferData bufferOne;  
  bufferData bufferTwo;

  /* Memory */
  RRAMdata *** memory;

  
 public:
  void set_values                  (void);
  void free_memory                 (void);
  
  int service_readwrite_request    (requestType, int,  int, int, int,    int,    unsigned int write_data[DATA_SIZE]);
  /*                                requestType, bank, row, col, rowHit, colHit, data */

  /* 
   READ REQUEST (ROW, COL):
   
   1. Data in memory goes to bufferOne - Eric
   2. Stats - Case on rowHit and colHit and accordingly consider power, energy, latency - Raghav
  
   WRITE REQUEST (ROW, COL)
  
   1. Data in write_data goes to memory - Eric
   2. Same as read - Raghav
  */
  
  
  int service_operation_request    (requestType, int,  int,    int,    int,    int,    int,       int,       int,       int);
  /*                                requestType, bank, rowOne, colOne, rowTwo, colTwo, rowOneHit, colOneHit, rowTwoHit, colTwoHit */  

  /* 
     OPERATION (ROW1, COL1, ROW2, COL2)

     1. Assume OPERATION is AND. Resultant goes to lower cell - Eric
     2. Stats - Case on rowHit and colHit and accordingly consider power, energy, latency - Raghav
  */ 

  
  void show_stats                  (void);
  
  int verify_readwrite_request     (requestType, int,  int, int);
  /*                                requestType, bank, row, col*/

  /* 
     VERIFY
     1. Make sure bank, row, col is within bounds i.e >=0 <=TOTAL - Eric
  */

  int verify_operation_request     (requestType, int,  int,    int,    int,    int,    int,       int,       int,       int);
  /*                                requestType, bank, rowOne, colOne, rowTwo, colTwo, rowOneHit, colOneHit, rowTwoHit, colTwoHit */

  /* 
     VERIFY
     1. Make sure bank, row, col is within bounds i.e >=0 <= TOTAL - Eric
     2. Either rowOne = rowTwo or colOne = colTwo - Eric
     3. Cannot be the same cell - Eric
  */

  int parse();


};

