
#include "helper.h"

/*Initialising stats */
void stats::init_values(){
  
  total_energy    = 0;
  average_energy  = 0;
  average_power   = 0;
  total_latency   = 0;
  average_latency = 0;
  count           = 0;
}


/* Updates energy, power and latency for stats object */
void stats::update_stats (float energy, float power, float latency){

  total_energy  += energy;
  average_energy = (total_energy)/(count + 1); 
  average_power  = (average_power*count + power)/(count + 1);

  total_latency  += latency;
  average_latency = (average_latency*count + latency)/(count + 1); 
  
  count += 1;
}



/* Initialising buffer data */
void bufferData::init_values(){
  
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    data[fourbytes] = 0;
  }
}


/* Initiliasing RRAM data */
void RRAMdata::init_values(){
  
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    data[fourbytes] = 0;
  }
}



/* Defining RRAMspec class Functions */
/* Init function for RRAM class */
void RRAMspec::set_values(){
  
  int bank, row, col;

  numBanks    = NUM_BANKS;
  numRows     = NUM_ROWS;
  numCols     = NUM_COLS;

  requestTime   = 0;
  requestPower  = 0;
  requestEnergy = 0;
  
  /* Init Memory */
  memory = (RRAMdata ***) malloc(sizeof(class RRAMdata **) * numBanks);
  
  for (bank = 0; bank < numBanks; bank++){
    memory[bank] = (RRAMdata **) malloc(sizeof(class RRAMdata *) * numRows);
  }
  
  for (bank = 0; bank < numBanks; bank++){
    for (row = 0; row < numRows; row++){
      memory[bank][row] = (RRAMdata *) malloc(sizeof(class RRAMdata) * numCols);
    }
  }

  for (bank = 0; bank < numBanks; bank++){
    for (row = 0; row < numRows; row++){
      for (col = 0; col < numCols; col++){
        memory[bank][row][col].init_values();
      }
    }
  }
  
  requestStats.init_values();
  
  cout << RED << endl << "SETTING UP RRAM VALUES " << BLUE << endl;
  cout << "No. of Banks:         " << numBanks    << endl;
  cout << "No. of Rows:          " << numRows     << endl;
  cout << "No. of Cols:          " << numCols     << endl;
  cout << RESET << endl;
}




/* Free RRAM emory */
void RRAMspec::free_memory(void){

  int bank, row;

  for (bank = 0; bank < numBanks; bank++){
    for (row = 0; row < numRows; row++){
      
      free(memory[bank][row]);
    }
    free(memory[bank]);
  }
  free(memory);
}




float calculate_request_energy(requestType request, float requestTime, int sameRowLOP){

  float requestEnergy = 0;
  
  if (request == READ){
    requestEnergy  = (RD_PWR * requestTime)  + (HALF_SEL_PWR * requestTime * (NUM_ROWS-1 + NUM_COLS-1));
  }
  else if (request == WRITE){
    requestEnergy  = (WR_PWR * requestTime)  + (HALF_SEL_PWR * requestTime * (NUM_ROWS-1 + NUM_COLS-1));
  }
  else if (request == NOT){
    requestEnergy  = (NOT_PWR * requestTime) + (HALF_SEL_PWR * requestTime * (NUM_ROWS-1 + NUM_COLS-1));
  }
  else { //OR
    requestEnergy  = (OR_PWR * requestTime)  + (HALF_SEL_PWR * requestTime * (NUM_ROWS-1 + NUM_COLS-1));
    requestEnergy += (sameRowLOP) ? (HALF_SEL_PWR * requestTime * (NUM_COLS-1)) : (HALF_SEL_PWR * requestTime * (NUM_ROWS-1));
  }

  return requestEnergy;
}



/* Function to find and add energy, latency consumed by a read/write request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_readwrite_request(requestType request, int bank, int row, int col, unsigned int data[DATA_SIZE]){

  if (verify_readwrite_request(request, bank, row, col) == -1){
       return -1;
  } 

  
  if (request == READ){
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      data[fourbytes] = memory[bank][row][col].data[fourbytes];
    }
  }
  else if (request == WRITE){
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][row][col].data[fourbytes] = data[fourbytes];
    }
  }
  else
    return -1; //invalid request

    
  requestTime   = (request == READ) ? RD_LAT : WR_LAT;
  requestEnergy = calculate_request_energy(request, requestTime, 0);

  if (request == WRITE){
    requestEnergy += requestTime * WR_SET_BIT_PWR * noSetBits(data);
  }
  
  requestPower = requestEnergy/requestTime;
  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  #ifdef DEBUG
  if (request == READ)
    cout << BLUE;
  else
    cout << GREEN;
  
  cout << "Request: "     << setw(5)  << requestString[request];
  cout << " Bank: "      << setw(4)  << bank;
  cout << " Row-1: "     << setw(6)  << row;
  cout << " Col-1: "     << setw(4)  << col;
  cout << "                          ";
  cout << " Time: "      << setw(6)  << requestTime;
  cout << " POWER: "     << setw(7)  << requestPower;
  cout << " ENERGY: "    << setw(7)  << requestEnergy;
  cout << endl << hex;
  cout <<  " DATA:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << data[fourbytes]  << "_";
  }
  cout << setw(8) << data[DATA_SIZE-1];
  cout << dec << RESET << endl << endl;
  cout << setfill(' ');

  #endif
  return 0;
}




/* Function to find and add energy, latency consumed by a bitwise NOT request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_not_request(requestType request, int bank, int row, int col){

  if (verify_not_request(request, bank, row, col) == -1){
      return -1;
  }
  
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][row][col].data[fourbytes] = ~memory[bank][row][col].data[fourbytes];
  }
 
  requestTime   = NOT_LAT;
  requestEnergy = calculate_request_energy(request, requestTime, 0);

  requestPower = requestEnergy/requestTime;
  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  #ifdef DEBUG
  cout << MAGENTA;

  cout << "Request: "    << setw(5)  << requestString[request];
  cout << " Bank: "     << setw(4)  << bank;
  cout << " Row-1: "    << setw(6)  << row;
  cout << " Col-1: "    << setw(4)  << col;
  cout << "                          ";
  cout << " Time: "    << setw(6)  << requestTime;
  cout << " POWER: "   << setw(7)  << requestPower;
  cout << " ENERGY :"  << setw(7)  << requestEnergy;
  cout << endl << endl << RESET;
  #endif

  return 0;
}



/* Function to find and add energy, latency consumed by a bitwise OR request. Returns 0 if sucessful.                                                                                                     
   Else returns -1*/

int RRAMspec::service_or_request(requestType request, int bank, int rowOne, int colOne, int rowTwo, int colTwo){

  if (verify_or_request(request, bank, rowOne, colOne, rowTwo, colTwo) == -1){
    return -1;
  }

  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    memory[bank][rowOne][colOne].data[fourbytes] =
    memory[bank][rowOne][colOne].data[fourbytes] | memory[bank][rowTwo][colTwo].data[fourbytes];
  }

  requestTime   = OR_LAT;
  requestEnergy = calculate_request_energy(request, requestTime, (rowOne == rowTwo));

  requestPower = requestEnergy/requestTime;
  requestStats.update_stats(requestEnergy, requestPower, requestTime);

  /* Print Request Details */
    #ifdef DEBUG
  cout << RED;

  cout << "Request: "    << setw(5)  << requestString[request];
  cout << " Bank: "      << setw(4)  << bank;
  cout << " Row-1: "     << setw(6)  << rowOne;
  cout << " Col-1: "     << setw(4)  << colOne;
  cout << " Row-2: "     << setw(6)  << rowTwo;
  cout << " Col-2: "     << setw(4)  << colTwo;
  cout << " Time: "    << setw(6)  << requestTime;
  cout << " POWER: "   << setw(7)  << requestPower;
  cout << " ENERGY :"  << setw(7)  << requestEnergy;
  cout << endl << endl << RESET;
    #endif

  return 0;
}



/*Function to ensure r/w request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_readwrite_request(requestType request, int bank, int row, int col){

  if ((request != READ) && (request != WRITE))
    return -1;
  if ((bank > NUM_BANKS) || (bank < 0))
    return -1;
  if ((row > NUM_ROWS) || (row < 0))
    return -1;
  if ((col > NUM_COLS) || (col < 0))
    return -1;

  return 0;
}



/*Function to ensure NOT request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_not_request(requestType request, int bank, int row, int col){

  if (request != NOT)
    return -1;
  if ((bank > NUM_BANKS) || (bank < 0))
    return -1;
  if ((row > NUM_ROWS) || (row < 0))
    return -1;
  if ((col > NUM_COLS) || (col < 0))
    return -1;

  return 0;
}



/*Function to ensure OR request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_or_request(requestType request, int bank, int rowOne, int colOne,
				 int rowTwo, int colTwo){

  if (request != OR)
    return -1;
  if ((bank > NUM_BANKS) || (bank < 0))
    return -1;
  if ((rowOne > NUM_ROWS) || (rowOne < 0))
    return -1;
  if ((colOne > NUM_COLS) || (colOne < 0))
    return -1;
  if ((rowTwo > NUM_ROWS) || (rowTwo < 0))
    return -1;
  if ((colTwo > NUM_COLS) || (colTwo < 0))
    return -1;
  if ((rowOne != rowTwo) && (colOne != colTwo))
    return -1;
  if ((rowOne == rowTwo) && (colOne == colTwo))
    return -1;

  return 0;
}



int RRAMspec::parse(){

  FILE * fp;
  unsigned long long bytes_read;
  int  request, bank, row1, col1, row2, col2;
  requestType REQ;
  
  unsigned int data[DATA_SIZE];
  
  fp = fopen(FILENAME, "rb");
  
  if (fp == NULL){
    cout << RED << "TRACE NOT FOUND" << RESET << endl;
    return -1;
  }
  
  while (fread((&bytes_read), sizeof(unsigned long long), 1, fp) != 0){

    bytes_read = bytes_read >> 4;

    col1       = bytes_read & 0x3FF;
    bytes_read = bytes_read >> 10;

    row1       = bytes_read & 0x7FFF;
    bytes_read = bytes_read >> 15;

    col2       = bytes_read & 0x3FF;
    bytes_read = bytes_read >> 10;

    row2       = bytes_read & 0x7FFF;
    bytes_read = bytes_read >> 15;

    bank       = bytes_read & 0x7;
    bytes_read = bytes_read >> 3;

    request    = bytes_read & 0x7;
    bytes_read = bytes_read >> 3;

    
    if (request == 0)
      REQ = READ;
    else if (request == 1)
      REQ = WRITE;
    else if (request == 2)
      REQ = NOT;
    else
      REQ = OR;

    
    if (REQ == WRITE) /* Also get the data to be written */{
      fread(&data[0], sizeof(unsigned int), DATA_SIZE, fp);
    }
    else{
      for(int iter = 0; iter < DATA_SIZE; iter++){
	data[iter] = 0x00000000;
      }
    }
    
    if (REQ == NOT){
      service_not_request(REQ, bank, row1, col1);
    }
    else if (REQ == OR){
      service_or_request(REQ, bank, row1, col1, row2, col2);
    }
    else {
      service_readwrite_request(REQ, bank, row1, col1, data);
    }
  }
  
  fclose(fp);
  return 0;
}




int main(){

  RRAMspec RRAM;

  RRAM.set_values();

  RRAM.parse();
  
  RRAM.free_memory();
  RRAM.show_stats();
  
  return 0; 
}

