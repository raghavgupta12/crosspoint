#include "crosspoint.h"

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
  
  /* Init buffers */
  bufferOne.init_values();
  bufferTwo.init_values();

  requestStats.init_values();
  
  cout << RED << endl << "SETTING UP RRAM VALUES " << BLUE << endl;
  cout << "No. of Banks:         " << numBanks    << endl;
  cout << "No. of Rows:          " << numRows     << endl;
  cout << "No. of Cols:          " << numCols     << endl;
  cout << "Policy:               " << policyString[POLICY] << endl;
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




/* Function to calculate selection energy given whether row hit, col hit. */
float calculate_sel_energy(policyType POLICY, int rowHit, int colHit){

  float sel_energy  = 0;

  if (POLICY == OPEN){

    if ((rowHit == 1) && (colHit == 1)){
      sel_energy  = 0;
    }
    else if ((rowHit == 1) || (colHit == 1)){
      sel_energy  = SEL_LAT * SEL_PWR;
    }
    else {
      sel_energy  = SEL_LAT * SEL_PWR * 2;
    }
  }

  else { /* POLICY = CLOSE */
    sel_energy = SEL_LAT * SEL_PWR * 2;
  }

  return sel_energy;
}




/* Function to calculate selection latency given whether row hit, col hit. */
float calculate_sel_latency(policyType POLICY, int rowHit, int colHit){

  float sel_latency  = 0;

  if (POLICY == OPEN){

    if ((rowHit == 1) && (colHit == 1)){
      sel_latency  = 0;
    }
    else if ((rowHit == 1) || (colHit == 1)){
      sel_latency  = SEL_LAT;
    }
    else {
      sel_latency  = SEL_LAT;
    }
  }

  else { /* POLICY = CLOSE */
    sel_latency = SEL_LAT;
  }

  return sel_latency;
}




/* Function to find and add energy, latency consumed by a read/write request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_readwrite_request(requestType request, int bank, int row, int col,
          int rowHit, int colHit, unsigned int data[DATA_SIZE]){

  if (verify_readwrite_request(request, bank, row, col) == -1){
       return -1;
  } 

  if (request == READ)
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      bufferOne.data[fourbytes] = memory[bank][row][col].data[fourbytes];
    }
  else if (request == WRITE)
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][row][col].data[fourbytes] = data[fourbytes];
    }
  else
    return -1; //invalid request
                 
  float REQ_LAT = (request == READ) ? RD_LAT : WR_LAT;
  float REQ_PWR = (request == READ) ? RD_PWR : WR_PWR;  
  
  requestTime    = calculate_sel_latency(POLICY, rowHit, colHit);
  requestTime   += REQ_LAT;

  requestEnergy  = calculate_sel_energy(POLICY, rowHit, colHit);
  requestEnergy += (REQ_LAT * REQ_PWR);
    
  requestPower = requestEnergy/requestTime;

  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  #ifdef DEBUG
  if (request == READ)
    cout << BLUE;
  else
    cout << GREEN;
  
  cout << "Request: "     << setw(5)  << requestString[request];
  cout << "  Row-1-Hit: " << setw(2)  << rowHit;
  cout << " Col-1-Hit: "  << setw(2)  << colHit;
  cout << "                             ";
  cout << " Bank: "      << setw(4)  << bank;
  cout << " Row-1: "     << setw(6)  << row;
  cout << " Col-1: "     << setw(4)  << col;
  cout << "                          ";
  cout << " Time: "      << setw(6)  << requestTime;
  cout << " POWER: "     << setw(6)  << requestPower;
  cout << " ENERGY: "    << setw(6)  << requestEnergy;
  cout << endl << hex;
  cout <<  " DATA:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << data[fourbytes]  << "_";
  }
  cout << setw(8) << data[DATA_SIZE];
  cout << dec << RESET << endl << endl;
  cout << setfill(' ');

  /*cout <<  " Buffer Data:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << bufferOne.data[fourbytes]  << "_";
  }
  cout << endl << hex;*/
  #endif

  return 0;
}




/* Function to find and add energy, latency consumed by a logical operation request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_operation_request(requestType request, int bank, int rowOne, int colOne,
          int rowTwo, int colTwo, int rowOneHit, int colOneHit,
          int rowTwoHit, int colTwoHit){

  if (verify_operation_request(request, bank, rowOne, colOne, rowTwo, colTwo,
                                 rowOneHit, colOneHit, rowTwoHit, colTwoHit) == -1){
      return -1;
  } 

  if (rowOne < rowTwo) {
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][rowOne][colOne].data[fourbytes] = 
      memory[bank][rowOne][colOne].data[fourbytes] & memory[bank][rowTwo][colTwo].data[fourbytes];
    }
  }
  else if (rowTwo < rowOne) {
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][rowTwo][colTwo].data[fourbytes] = 
      memory[bank][rowOne][colOne].data[fourbytes] & memory[bank][rowTwo][colTwo].data[fourbytes];
    }
  }
  else if (colTwo < colOne) {
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][rowTwo][colTwo].data[fourbytes] = 
      memory[bank][rowOne][colOne].data[fourbytes] & memory[bank][rowTwo][colTwo].data[fourbytes];
    }
  }
  else {
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][rowOne][colOne].data[fourbytes] = 
      memory[bank][rowOne][colOne].data[fourbytes] & memory[bank][rowTwo][colTwo].data[fourbytes];
    }
  }

  requestTime    = calculate_sel_latency(POLICY, rowOneHit, colOneHit);
  requestTime   += calculate_sel_latency(POLICY, rowTwoHit, colTwoHit);
  requestTime   += LOP_LAT;

  requestEnergy  = calculate_sel_energy(POLICY, rowOneHit, colOneHit);
  requestEnergy += calculate_sel_energy(POLICY, rowTwoHit, colTwoHit);
  requestEnergy += (LOP_LAT * LOP_PWR);

  requestPower = requestEnergy/requestTime;

  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  #ifdef DEBUG
  cout << DARKBLUE;

  cout << "Request: "    << setw(5)  << requestString[request];
  cout << "  Row-1-Hit: " << setw(2)  << rowOneHit;
  cout << " Col-1-Hit: "  << setw(2)  << colOneHit;
  cout << " Row-2-Hit: "  << setw(2)  << rowTwoHit;
  cout << " Col-2-Hit: "  << setw(2)  << colTwoHit;
  cout << "  Bank: "      << setw(4)  << bank;
  cout << " Row-1: "      << setw(6)  << rowOne;
  cout << " Col-1: "      << setw(4)  << colOne;
  cout << " Row-2: "      << setw(6)  << rowTwo;
  cout << " Col-2: "      << setw(4)  << colTwo;
  cout << " Time: "    << setw(6)  << requestTime;
  cout << " POWER: "   << setw(6)  << requestPower;
  cout << " ENERGY :"  << setw(6)  << requestEnergy;
  cout << endl;
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

/*Function to ensure lop request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_operation_request(requestType request, int bank, int rowOne, int colOne, 
            int rowTwo, int colTwo, int rowOneHit, int colOneHit, int rowTwoHit, int colTwoHit){

  if (request != LOP)
    return -1;
  if ((bank > NUM_BANKS) || (bank < 0))
    return -1;
  if ((rowOne > NUM_ROWS) || (rowOne < 0))
    return -1;
  if ((rowTwo > NUM_ROWS) || (rowTwo < 0))
    return -1;
  if ((colOne > NUM_COLS) || (colOne < 0))
    return -1;
  if ((colTwo > NUM_COLS) || (colTwo < 0))
    return -1;
  if ((rowOne != rowTwo) && (colOne != colTwo))
    return -1;
  if ((rowOne == rowTwo) && (colOne == colTwo))
    return -1;

  return 0;
}

int parse(){

  FILE * fp;
  unsigned long long bytes_read;
  int request, bank, row1, col1, row2, col2, row1Hit, col1Hit, row2Hit, col2Hit;
  requestType REQ;
  
  unsigned int data[DATA_SIZE];

  RRAMspec RRAM;
  RRAM.POLICY = OPEN;
  RRAM.set_values();
  
  fp = fopen(filename, "rb");
  
  if (fp == NULL){
    cout << RED << "TRACE NOT FOUND" << RESET << endl;
    return -1;
  }
  
  while (fread((&bytes_read), sizeof(unsigned long long), 1, fp) != 0){

    col1Hit    = bytes_read & 1;
    bytes_read = bytes_read >> 1;

    row1Hit    = bytes_read & 1;
    bytes_read = bytes_read >> 1;

    col2Hit    = bytes_read & 1;
    bytes_read = bytes_read >> 1;

    row2Hit    = bytes_read & 1;
    bytes_read = bytes_read >> 1;

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
    else
      REQ = LOP;

    if (REQ == WRITE) /* Also get the data to be written */
      fread(&data[0], sizeof(unsigned int), DATA_SIZE, fp);

    for(int iter = 0; iter < 16; iter++){
      data[iter] = 0x00000000;
    }
  
    if (REQ == LOP)
      RRAM.service_operation_request(REQ, bank, row1, col1, row2, col2, 
                                row1Hit, col1Hit, row2Hit, col2Hit);
    else
      RRAM.service_readwrite_request(REQ, bank, row1, col1, row1Hit, col1Hit, data);
  }
  fclose(fp);
  return 0;
}

int main(){

  RRAMspec RRAM;

  RRAM.POLICY = OPEN;
  RRAM.set_values();

  /*requestType Req = LOP;

  RRAM.service_operation_request(Req, 1, 0, 0, 0, 1,
                     1, 0,
                     1, 0);
  Req = READ;

  unsigned int data[DATA_SIZE];
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    data[fourbytes] = 0;
  }
    
  RRAM.service_readwrite_request(Req, 1, 0, 0,
               0, 0, data);*/

  requestType Req = WRITE;

  unsigned int data[DATA_SIZE];
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    data[fourbytes] = 1;
  }

  RRAM.service_readwrite_request(Req, 1, 0, 0, 0, 0, data);

  Req = READ;

  RRAM.service_readwrite_request(Req, 1, 0, 0, 0, 0, data);
  RRAM.service_readwrite_request(Req, 1, 0, 1, 0, 0, data);

  Req = LOP;
  RRAM.service_operation_request(Req, 1, 0, 0, 0, 1, 1, 0, 1, 0);

  Req = READ;

  RRAM.service_readwrite_request(Req, 1, 0, 1, 0, 0, data);
  
  RRAM.free_memory();
  RRAM.show_stats();
  
  return 0; 
}

