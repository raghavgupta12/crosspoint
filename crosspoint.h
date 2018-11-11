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

#define NUM_BANKS    1
#define NUM_ROWS     2
#define NUM_COLS     2

#define RD_PWR       1
#define RD_LAT       5

#define WR_PWR       3
#define WR_LAT       5

#define NOT_PWR      2
#define NOT_LAT      5

#define OR_PWR       4
#define OR_LAT       5

#define HALF_SEL_PWR   0.2
#define WR_SET_BIT_PWR 0.001

#define FILENAME     "trace.bin"
/*************************************/
          

enum requestType   {READ, WRITE, NOT, OR};

const string requestString[] = {"READ", "WRITE", "NOT", "OR"};


		    
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

  
 protected:
  int   numBanks, numRows, numCols;

  float requestTime, requestPower, requestEnergy;  
  stats requestStats;

  /* Memory */
  RRAMdata *** memory;

  
 public:
  void set_values                  (void);
  void free_memory                 (void);
  
  int service_readwrite_request (requestType, int,  int, int, unsigned int write_data[DATA_SIZE]);
  /*                             requestType, bank, row, col, data */
  
  int service_or_request        (requestType, int,  int,    int,    int,    int );
  /*                             requestType, bank, rowOne, colOne, rowTwo, colTwo */  

  int service_not_request       (requestType, int,  int, int);
  /*                             requestType, bank, row, col */

  
  void show_stats               (void);
  
  int verify_readwrite_request  (requestType, int,  int, int);
  /*                             requestType, bank, row, col*/

  int verify_or_request         (requestType, int,  int,    int,    int,    int);
  /*                             requestType, bank, rowOne, colOne, rowTwo, colTwo */

  int verify_not_request        (requestType, int,  int, int);
  /*                             requestType, bank, row, col */

  int parse();

};

