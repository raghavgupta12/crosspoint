#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <cstring>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <fstream>

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

/****************/
//#define DEBUG 
/****************/

//Default values
#define DATA_SIZE        16

#define WR_SET_BIT_PWR   0.001

#define NOT_PWR          2
#define NOT_LAT          5
#define NOT_HALF_SEL_PWR 0.2

#define OR_PWR           4
#define OR_LAT           5
#define OR_HALF_SEL_PWR  0.2
          

enum requestType   {READ, WRITE, NOT, OR};
enum requestGranularity  {ROW, COL, CELL, ELEMENT};
enum topology {BITWISE, ROWWISE, ROWCOL};

const string requestString[] = {"READ", "WRITE", "NOT", "OR"};


        
class stats{
 public:
  //overall energy statistics
  float total_energy;
  float average_energy;
  float average_power;

  //overall latency statistics
  float total_latency;
  float average_latency;

  //cell select energy
  float halfsel_energy;
  float fullsel_energy;

  //peripheral energy
  float TIA_energy;
  float wire_energy;
  float decoder_energy;

  //peripheral latency
  float TIA_latency;
  float wire_latency;
  float decoder_latency;

  //counters
  float requestCount;
  float halfSelCount;
  float fullSelCount;
  
  void init_values(void);
  void update_stats(float, float, float);
  void update_sel_count(int, requestType, requestGranularity, int, int, float, float, float, float);
  void print_stats(void);
  void print_csv(void);
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
  float requestTime, requestPower, requestEnergy;  
  stats requestStats;

  /* Memory */
  RRAMdata *** memory;

 public: 
  int numBanks, numRows, numCols;
  int numParArr;
  float readEnergy, readLat, readHalfSel;
  float writeEnergy, wrSetPwr, writeLat, writeHalfSel;
  float notPwr, notLat, notHalfSel;
  float orPwr, orLat, orHalfSel;
  float TIAEnergy, TIALat, decoderEnergy, decoderLat;
  topology top;

  const char* traceFilename;
  const char* config;

 public:
  void set_defaults                (void);
  void set_values                  (void);
  void free_memory                 (void);
  
  int service_readwrite_request (requestType, requestGranularity, int,  int, int, unsigned int write_data[DATA_SIZE]);
  /*                             requestType, bank, row, col, data */
  
  int service_or_request        (requestType, requestGranularity, int,  int,    int,    int,    int );
  /*                             requestType, sel,          bank, rowOne, colOne, rowTwo, colTwo */  

  int service_not_request       (requestType, requestGranularity, int,  int,    int,    int,    int);
  /*                             requestType, sel,          bank, rowOne, colOne, rowTwo, colTwo */

  float calculate_peripheral_energy(requestType, requestGranularity);

  float calculate_peripheral_latency(requestType, requestGranularity);

  float calculate_select_energy(requestType, requestGranularity);

  float calculate_request_energy(requestType, requestGranularity);

  float calculate_request_latency(requestType, requestGranularity);


  void show_stats               (void);

  void show_csv                (void);
  
  int verify_readwrite_request  (requestType, int,  int, int);
  /*                             requestType, bank, row, col*/

  int verify_or_request         (requestType, requestGranularity, int,  int,    int,    int,    int);
  /*                             requestType, sel,          bank, rowOne, colOne, rowTwo, colTwo */

  int verify_not_request        (requestType, requestGranularity, int,  int,    int,    int,    int);
  /*                             requestType, sel,          bank, rowOne, colOne, rowTwo, colTwo */

  int parse_trace();

};

