
#include "helper.h"


/*Initialising stats */
void stats::init_values(){
  
  total_energy    = 0;
  average_energy  = 0;
  average_power   = 0;
  total_latency   = 0;
  average_latency = 0;
  requestCount    = 0;
  halfSelCount    = 0;
  fullSelCount    = 0;
  halfsel_energy  = 0;
  fullsel_energy  = 0;
}


/* Updates energy, power and latency for stats object */
void stats::update_stats (float energy, float power, float latency){

  total_energy  += energy;
  average_energy = (total_energy)/(requestCount + 1); 
  average_power  = (average_power*requestCount + power)/(requestCount + 1);

  total_latency  += latency;
  average_latency = (average_latency*requestCount + latency)/(requestCount + 1); 
  
  requestCount += 1;
}

//Updates counters for number of full selected and half selected cells + related energy
void stats::update_sel_count (int numParArr, requestType request, requestGranularity sel, int numRows, int numCols,
                              float readEnergy, float readHalfSel, float writeEnergy, float writeHalfSel){
    if (request == READ){
      if(numParArr== 0) { //non parallel
        if(sel == ROW) {
          fullSelCount += numCols;
          halfSelCount += (float)numCols * (float)(numRows-1);
          halfsel_energy += (readHalfSel * numCols * (numRows-1));
          fullsel_energy += (numCols * readEnergy);
        }
        else if(sel == COL) {
          fullSelCount += numRows;
          halfSelCount += (float)numRows * (float)(numCols-1);
          halfsel_energy += (readHalfSel * numRows * (numCols-1));
          fullsel_energy += (numRows * readEnergy);
        }
        else if (sel == CELL) { //64 single cell operations in parallel
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1 + numCols-1);
          halfsel_energy += (readHalfSel * 64 * (numRows-1 + numCols-1));
          fullsel_energy += readEnergy * 64;
        }
        else { //read one matrix element (64 bits)
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1) + (numCols-64);
          halfsel_energy += (readHalfSel * (64 * (numRows-1) + (numCols-64)));
          fullsel_energy += readEnergy * 64;
        }
      }
      else { //numParArrays in lockstep
        if(sel == ROW) {
          fullSelCount += numParArr* numCols;
          halfSelCount += numParArr* numCols * (numRows-1);
          halfsel_energy += readHalfSel * numParArr* numCols * (numRows-1);
          fullsel_energy += readEnergy * numParArr* numCols;
        }
        else if(sel == COL) {
          fullSelCount += numParArr* numRows;
          halfSelCount += numParArr* numRows * (numCols-1);
          halfsel_energy += readHalfSel * numParArr* numRows * (numCols-1);
          fullsel_energy += readEnergy * numParArr* numRows;
        }
        else if (sel == CELL) { //64 single cell operations in parallel
          fullSelCount += numParArr;
          halfSelCount += numParArr* (numRows-1 + numCols-1);
          halfsel_energy += readHalfSel * numParArr* (numRows-1 + numCols-1);
          fullsel_energy += readEnergy * numParArr;
        }
        else { //read one matrix element (64 bits)
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1) + (numCols-64);
          halfsel_energy += readHalfSel * (64 * (numRows-1) + (numCols-64));
          fullsel_energy += readEnergy * 64;
        }
      }
    }
    else if (request == WRITE){
      if(numParArr== 0) { //non parallel
        if(sel == ROW) { 
          fullSelCount += numCols;
          halfSelCount += numCols * (numRows-1);
          halfsel_energy += (writeHalfSel * numCols * (numRows-1));
          fullsel_energy += (numCols * writeEnergy);
          //requestEnergy  = ((numCols * readEnergy)  + (readHalfSel * numCols * (numRows-1)));
        }
        else if(sel == COL) {
          fullSelCount += numRows;
          halfSelCount += numRows * (numCols-1);
          halfsel_energy += (writeHalfSel * numRows * (numCols-1));
          fullsel_energy += (numRows * writeEnergy);
        }
        else if (sel == CELL) { //64 single cell operations in parallel
          fullSelCount += numParArr;
          halfSelCount += numParArr* (numRows-1 + numCols-1);
          halfsel_energy += (writeHalfSel * numParArr* (numRows-1 + numCols-1));
          fullsel_energy += writeEnergy * numParArr;
        }
        else { //read one matrix element (64 bits)
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1) + (numCols-64);
          halfsel_energy += (writeHalfSel * (64 * (numRows-1) + (numCols-64)));
          fullsel_energy += writeEnergy * 64;
        }
      }
      else { // numParArrays in lockstep
        if(sel == ROW) {
          fullSelCount += numParArr* numCols;
          halfSelCount += numParArr* numCols * (numRows-1);
          halfsel_energy += writeHalfSel * numParArr* numCols * (numRows-1);
          fullsel_energy += writeEnergy * numParArr* numCols;
        }
        else if(sel == COL) {
          fullSelCount += numParArr* numRows;
          halfSelCount += numParArr* numRows * (numCols-1);
          halfsel_energy += writeHalfSel * numParArr* numRows * (numCols-1);
          fullsel_energy += writeEnergy * numParArr* numRows;
        }
        else if (sel == CELL) { //64 single cell operations in parallel
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1 + numCols-1);
          halfsel_energy += writeHalfSel * 64 * (numRows-1 + numCols-1);
          fullsel_energy += writeEnergy * 64;
        }
        else { //read one matrix element (64 bits)
          fullSelCount += 64;
          halfSelCount += 64 * (numRows-1) + (numCols-64);
          halfsel_energy += writeHalfSel * (64 * (numRows-1) + (numCols-64));
          fullsel_energy += writeEnergy * 64;
        }
      }
    }
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

/** 
 * Sets parameters for RRAM class to default values
 * Mostly sets values for currently unused parameters not included in config file
 **/
void RRAMspec::set_defaults(){
  wrSetPwr    = WR_SET_BIT_PWR;
  notPwr      = NOT_PWR;
  notLat      = NOT_LAT;
  notHalfSel  = NOT_HALF_SEL_PWR;
  orPwr       = OR_PWR;
  orLat       = OR_LAT;
  orHalfSel   = OR_HALF_SEL_PWR;
}

/** 
 * Init function for RRAM class
 * allocates memory to track values being read/written, currently unused
 **/
void RRAMspec::set_values(){
  
  //int bank, row, col;

  requestTime   = 0;
  requestPower  = 0;
  requestEnergy = 0;
  
  /* Init Memory */
  /*memory = (RRAMdata ***) malloc(sizeof(class RRAMdata **) * numBanks);
  
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
  }*/
  
  requestStats.init_values();

}




/* Free allocated memory */
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

//Calculate the total energy consumed by peripheral circuitry and wire resistance for a request
float RRAMspec::calculate_peripheral_energy(requestType request, requestGranularity gran){
  //TIA
  float energy_TIA = TIAEnergy;

  //decoder
  float energy_decoder;
  if(request == READ) energy_decoder = (0.5/1000)*numCols + 0.05;
  else if(request == WRITE) energy_decoder = (0.5/1000)*numRows + 0.05;
  else energy_decoder = 0;

  //wire resistivity
  float energy_wire;
  if(request == READ) {
    if(gran == CELL) energy_wire = numRows*5*(1/(5*numRows + 500000))*(1/(5*numRows + 500000))*1000;
    else if(gran == ROW) energy_wire = numCols*numCols*5*(1/(5*numCols + 500000))*(1/(5*numCols + 500000))*1000;
    else if(gran == COL) energy_wire = numRows*numRows*5*(1/(5*numRows + 500000))*(1/(5*numRows + 500000))*1000;
    else energy_wire = 0;
  }
  else if (request == WRITE) {
    if(gran == CELL) energy_wire = numRows*5*(1/(5*numRows + 50000))*(1/(5*numRows + 50000))*10000;
    else if(gran == ROW) energy_wire = numCols*numCols*5*(1/(5*numCols + 50000))*(1/(5*numCols + 50000))*10000;
    else if(gran == COL) energy_wire = numRows*numRows*5*(1/(5*numRows + 50000))*(1/(5*numRows + 50000))*10000;
    else energy_wire = 0;
  }
  else energy_wire = 0; //model for PIM ops not known

  return energy_TIA + energy_decoder + energy_wire;
}

//Calculate total latency incurred by peripheral circuitry and wire delay for a request
float RRAMspec::calculate_peripheral_latency(requestType request, requestGranularity gran){
  float totalLat;
  //TIA
  float lat_TIA = TIALat;

  //Decoder
  float lat_decoder;
  if(gran == CELL) lat_decoder = decoderLat * numRows + 0.1;
  else if(gran == ROW) lat_decoder = decoderLat * numCols + 0.1;
  else if (gran == COL) lat_decoder = decoderLat * numRows + 0.1;
  else lat_decoder = 0;
  
  //Wire Delay, TODO update function
  float lat_wire;
  if(gran == CELL) lat_wire = 0;
  else if(gran == ROW) lat_wire = 0;
  else if (gran == COL) lat_wire = 0;
  else lat_decoder = 0;

  totalLat = lat_TIA + lat_decoder + lat_wire;

  return totalLat;
}

//Calculate total cell full and half select energy for a request
//TODO Update after clarifying cost function
float RRAMspec::calculate_select_energy(requestType request, requestGranularity gran){
  float fullSelEnergy, halfSelEnergy, totalSelEnergy;
  if(request == READ) {
    if(gran == CELL) {
      halfSelEnergy = numParArr*(readHalfSel * (numRows-1 + numCols-1));
      fullSelEnergy = readEnergy;
    }
    else if(gran == ROW) {
      halfSelEnergy = readHalfSel * numParArr * numCols * (numRows-1);
      fullSelEnergy = readEnergy * numParArr * numCols;
    }
    else if(gran == COL) {
      halfSelEnergy = readHalfSel * numParArr * numRows * (numCols-1);
      fullSelEnergy = readEnergy * numParArr * numRows;
    }
    else {
      halfSelEnergy = 0;
      fullSelEnergy = 0;
    }
  }
  else if(request == WRITE) {
    if(gran == CELL) {
      halfSelEnergy = numParArr*(writeHalfSel * (numRows-1 + numCols-1));
      fullSelEnergy = writeEnergy;
    }
    else if(gran == ROW) {
      halfSelEnergy = writeHalfSel * numParArr * numCols * (numRows-1);
      fullSelEnergy = writeEnergy * numParArr * numCols;
    }
    else if(gran == COL) {
      halfSelEnergy = writeHalfSel * numParArr * numRows * (numCols-1);
      fullSelEnergy = writeEnergy * numParArr * numRows;
    }
    else {
      halfSelEnergy = 0;
      fullSelEnergy = 0;
    }
  }
  //not/or cost functions unimplemented
  else {
    halfSelEnergy = 0;
    fullSelEnergy = 0;
  }
  totalSelEnergy = fullSelEnergy + halfSelEnergy;
  return totalSelEnergy;
}

//calculate overall energy for request
float RRAMspec::calculate_request_energy(requestType request, requestGranularity gran) {
  float energy_peripheral = calculate_peripheral_energy(request, gran);
  float energy_select = calculate_select_energy(request, gran);

  return energy_peripheral + energy_select;
}

//calculate overall latency for request
//TODO Using old equation, update after clarifying 
float RRAMspec::calculate_request_latency(requestType request, requestGranularity gran) {
  float reqLat, periphLat;
  //OLD LATENCY CALCULATION
  float rt = 0.5 * numRows * 2;
  float ct = 20 * numRows;
  float lat = (rt*ct/2)*(50000 + rt/3)/(1000000*(50000 + rt));

  if(request == READ) {
    reqLat = lat;
  }  
  if(request == WRITE) {
    reqLat = lat;
  }
  if(request == NOT) reqLat = notLat;
  if(request == OR) reqLat = orLat;

  periphLat = calculate_peripheral_latency(request, gran);
  reqLat += periphLat;
  
  return reqLat;
}



/* Function to find and add energy, latency consumed by a read/write request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_readwrite_request(requestType request, requestGranularity sel, int bank, int row, int col, unsigned int data[DATA_SIZE]){
  if (verify_readwrite_request(request, bank, row, col) == -1){
       return -1;
  } 
  
  //read/write data to memory - for tracking data being read/written
  /*if (request == READ){ //TODO add sel casing - need to change buffer implementation & likely prints to make this work
    if(sel == ROW) {
      for(int c = 0; c < numCols; c++) {
        for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++) {
          data[fourbytes] = memory[bank][row][c].data[fourbytes];
        }
      }
    }
    else if(sel == COL) {

    }
    else {

    }
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++) {
          data[fourbytes] = memory[bank][row][col].data[fourbytes];
      }
  }
  else if (request == WRITE){
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][row][col].data[fourbytes] = data[fourbytes];
    }
  }
  else
    return -1; //invalid request*/


  //Calculate request latency and energy
  requestTime = calculate_request_latency(request, sel);

  requestEnergy = calculate_request_energy(request, sel);

  //additional energy for writing set bits. Not included in current cost function
  /*if (request == WRITE){
    requestEnergy += requestTime * wrSetPwr * noSetBits(data);
  }*/
  
  requestPower = requestEnergy/requestTime;
  requestStats.update_sel_count(numParArr, request, sel, numRows, numCols, readEnergy, readHalfSel, writeEnergy, writeHalfSel);
  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  //TODO fix debug printing
  #ifdef DEBUG
  if (request == READ)
    cout << BLUE;
  else
    cout << GREEN;
  
  cout << "Request: "    << setw(5)  << requestString[request];
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




/* Function to find and add energy, latency consumed by a bitwise NOT request. 
   Returns 0 if sucessful. Else returns -1
   CURRENTLY UNUSED/NOT UPDATED
   */

int RRAMspec::service_not_request(requestType request, requestGranularity sel, int bank, int rowOne, int colOne,
         int rowTwo, int colTwo){

  if (verify_not_request(request, sel, bank, rowOne, colOne, rowTwo, colTwo) == -1){
      return -1;
  }
  
  /*if(sel == ROW) {
    for(int c = 0; c < NUM_COLS; c++) {
      for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][rowOne][c].data[fourbytes] = ~memory[bank][rowOne][c].data[fourbytes];
      }
    }
  }
  else if(sel == COL) {
    for(int r = 0; r < NUM_ROWS; r++) {
      for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][r][colOne].data[fourbytes] = ~memory[bank][r][colOne].data[fourbytes];
      }
    }
  }
  else if(sel == CELL) {//TODO fix
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][rowOne][colOne].data[fourbytes] = ~memory[bank][rowOne][colOne].data[fourbytes];
        memory[bank][rowOne][colTwo].data[fourbytes] = ~memory[bank][rowOne][colTwo].data[fourbytes];
    }
  }
  else { //sel == CELL_SAMECOL
    for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][rowOne][colOne].data[fourbytes] = ~memory[bank][rowOne][colOne].data[fourbytes];
        memory[bank][rowTwo][colOne].data[fourbytes] = ~memory[bank][rowTwo][colOne].data[fourbytes];
    }
  }*/
 
  requestTime   = notLat;
  requestEnergy = calculate_request_energy(request, sel);

  requestPower = requestEnergy/requestTime;
  requestStats.update_stats(requestEnergy, requestPower, requestTime);
  
  /* Print Request Details */
  #ifdef DEBUG
  cout << MAGENTA;

  cout << "Request: "    << setw(5)  << requestString[request];
  cout << " Bank: "     << setw(4)  << bank;
  cout << " Row-1: "     << setw(6)  << rowOne;
  cout << " Col-1: "     << setw(4)  << colOne;
  cout << " Row-2: "     << setw(6)  << rowTwo;
  cout << " Col-2: "     << setw(4)  << colTwo;
  cout << "                          ";
  cout << " Time: "    << setw(6)  << requestTime;
  cout << " POWER: "   << setw(7)  << requestPower;
  cout << " ENERGY :"  << setw(7)  << requestEnergy;
  cout << endl << hex;
  cout <<  " DATA:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << memory[bank][rowOne][colOne].data[fourbytes]  << "_";
  }
  cout << setw(8) << memory[bank][rowOne][colOne].data[DATA_SIZE-1];
  cout << dec << RESET << endl << endl;
  cout << setfill(' ');
  cout << endl << endl << RESET;
  #endif

  return 0;
}



/* Function to find and add energy, latency consumed by a bitwise OR request. Returns 0 if sucessful.                                                                                                     
   Else returns -1

   CURRENTLY UNUSED/NOT UPDATED
   */

int RRAMspec::service_or_request(requestType request, requestGranularity sel, int bank, int rowOne, int colOne, int rowTwo, int colTwo){

  if (verify_or_request(request, sel, bank, rowOne, colOne, rowTwo, colTwo) == -1){
    return -1;
  }

  /*if(sel == ROW) {
    for(int c = 0; c < numCols; c++) {
      for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][rowOne][c].data[fourbytes] =
        memory[bank][rowOne][c].data[fourbytes] | memory[bank][rowTwo][c].data[fourbytes];
      }
    }
  }
  else if(sel == COL) {
    for(int r = 0; r < numRows; r++) {
      for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
        memory[bank][r][colOne].data[fourbytes] =
        memory[bank][r][colOne].data[fourbytes] | memory[bank][r][colTwo].data[fourbytes];
      }
    }
  }
  else {
    return -1;
  }*/

  requestTime   = orLat;
  requestEnergy = calculate_request_energy(request, sel);

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
  cout << endl << hex;
  cout <<  " DATA:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << memory[bank][rowOne][colOne].data[fourbytes]  << "_";
  }
  cout << setw(8) << memory[bank][rowOne][colOne].data[DATA_SIZE-1];
  cout << dec << RESET << endl << endl;
  cout << setfill(' ');
  cout << endl << endl << RESET;
    #endif

  return 0;
}



/* Function to ensure r/w request is valid. Returns 0 if successful, else returns -1 */
//TODO update
int RRAMspec::verify_readwrite_request(requestType request, int bank, int row, int col){

  if ((request != READ) && (request != WRITE))
    return -1;
  if ((bank > numBanks) || (bank < 0))
    return -1;
  if ((row > numRows) || (row < 0))
    return -1;
  if ((col > numCols) || (col < 0))
    return -1;

  return 0;
}



/* Function to ensure NOT request is valid. Returns 0 if successful, else returns -1 */
int RRAMspec::verify_not_request(requestType request, requestGranularity sel, int bank, int rowOne, int colOne,
         int rowTwo, int colTwo){

  if (request != NOT)
    return -1;
  if ((bank > numBanks) || (bank < 0))
    return -1;
  if ((rowOne > numRows) || (rowOne < 0))
    return -1;
  if ((colOne > numCols) || (colOne < 0))
    return -1;
  if ((rowTwo > numRows) || (rowTwo < 0))
    return -1;
  if ((colTwo > numCols) || (colTwo < 0))
    return -1;

  if(sel == ROW) {
    if(rowOne == rowTwo)
      return -1;
  }
  if(sel == COL) {
    if(colOne == colTwo)
      return -1;
  }
  if(sel == CELL) {
    if(colOne == colTwo) 
      return -1;
  }
  /*if(sel == CELL_SAMECOL) {
    if(rowOne == rowTwo)
      return -1;
  }*/

  return 0;
}



/* Function to ensure OR request is valid. Returns 0 if successful, else returns -1 */
int RRAMspec::verify_or_request(requestType request, requestGranularity sel, int bank, int rowOne, int colOne,
         int rowTwo, int colTwo){

  if (request != OR)
    return -1;
  if ((bank > numBanks) || (bank < 0))
    return -1;
  if ((rowOne > numRows) || (rowOne < 0))
    return -1;
  if ((colOne > numCols) || (colOne < 0))
    return -1;
  if ((rowTwo > numRows) || (rowTwo < 0))
    return -1;
  if ((colTwo > numCols) || (colTwo < 0))
    return -1;
  if ((rowOne != rowTwo) && (colOne != colTwo))
    return -1;
  if ((rowOne == rowTwo) && (colOne == colTwo))
    return -1;

  if(sel == ROW) {
    if(rowOne == rowTwo)
      return -1;
  }
  if(sel == COL) {
    if(colOne == colTwo)
      return -1;
  }
  if(sel == CELL) {
    return -1;
  }
  /*if(sel == CELL_SAMECOL) {
    return -1;
  }*/
  return 0;
}


/* Parses trace and services requests */
int RRAMspec::parse_trace(){

  FILE * fp;
  unsigned long long bytes_read;
  int  select, request, bank, row1, col1, row2, col2;
  requestType REQ;
  requestGranularity SEL;
  
  unsigned int data[DATA_SIZE];
  
  fp = fopen(traceFilename, "rb");
  
  if (fp == NULL){
    cout << RED << "TRACE NOT FOUND" << RESET << endl;
    return -1;
  }
  
  while (fread((&bytes_read), sizeof(unsigned long long), 1, fp) != 0){

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
      0 = Row wise
      1 = Col wise
      2 = Cell (single cell/bit wise)

      REQ:
      0 = read
      1 = write
      2 = not
      3 = or
    */

    col1       = bytes_read & 0x3FF;
    bytes_read = bytes_read >> 10;

    row1       = bytes_read & 0x7FFF;
    bytes_read = bytes_read >> 15;

    col2       = bytes_read & 0x3FF;
    bytes_read = bytes_read >> 10;

    row2       = bytes_read & 0x7FFF;
    bytes_read = bytes_read >> 15;

    bank       = bytes_read & 0x1FF;
    bytes_read = bytes_read >> 9;

    request    = bytes_read & 0x7;
    bytes_read = bytes_read >> 3;

    select     = bytes_read & 0x3;
    bytes_read = bytes_read >> 2;
    
    if (request == 0)
      REQ = READ;
    else if (request == 1)
      REQ = WRITE;
    else if (request == 2) 
      REQ = NOT;
    else //(request == 3)
      REQ = OR;

    if(select == 0)
      SEL = ROW;
    else if(select == 1)
      SEL = COL;
    else if(select == 2)
      SEL = CELL;
    else assert(0 && "Bad Trace");


    
    if (REQ == WRITE) /* Also get the data to be written */{
      fread(&data[0], sizeof(unsigned int), DATA_SIZE, fp);
    }
    else{
      for(int iter = 0; iter < DATA_SIZE; iter++){
        data[iter] = 0x00000000;
      }
    }
    
    if (REQ == NOT){
      service_not_request(REQ, SEL, bank, row1, col1, row2, col2);
    }
    else if (REQ == OR){
      service_or_request(REQ, SEL, bank, row1, col1, row2, col2);
    }
    else {
      service_readwrite_request(REQ, SEL, bank, row1, col1, data);
    }
  }
  
  fclose(fp);
  return 0;
}


/**
 * Function to parse command line arguments
 * Format:
 * ./crosspoint -f <trace_file_name> -c <config_file_name>
 **/
void parse_args(int argc, char * argv[], RRAMspec &RRAM) {
  for(int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-f") == 0) {
        RRAM.traceFilename = argv[i+1];
    }
    if (strcmp(argv[i], "-c") == 0) {
        RRAM.config = argv[i+1];
    }

  }
    #ifdef DEBUG
  cout << RED << endl << "SETTING UP RRAM VALUES " << BLUE << endl;
  cout << "Trace Name: " << RRAM.traceFilename << endl;
  cout << "No. of Banks:         " << RRAM.numBanks    << endl;
  cout << "No. of Rows:          " << RRAM.numRows     << endl;
  cout << "No. of Cols:          " << RRAM.numCols     << endl;
  cout << endl;
  cout << "readEnergy:  " << RRAM.readEnergy << "    readLat:  " << RRAM.readLat << "    readHalfSelPwr:  " << RRAM.readHalfSel << endl;
  cout << "writeEnergy:  " << RRAM.writeEnergy << "    writeLat:  " << RRAM.writeLat << "    writeHalfSelPwr:  " << RRAM.writeHalfSel 
       << "    WrSetBitPwr: " << RRAM.wrSetPwr << endl;
  cout << "NotPwr: " << RRAM.notPwr << "    NotLat: " << RRAM.notLat << "    NotHalfSelPwr: " << RRAM.notHalfSel << endl;
  cout << "OrPwr:  " << RRAM.orPwr << "    OrLat:  " << RRAM.orLat << "    OrHalfSelPwr:  " << RRAM.orHalfSel << endl;
  cout << "No. of par arrays:    " << RRAM.numParArr<<
  cout << RESET << endl;
    #endif
}

/**
 * Function to parse config file. Adapted from ramulator parser
 * Empty line detection currently doesn't work. Sample config file in sample.cfg
 **/
void parse_config(const std::string& fname, RRAMspec &RRAM) {
  ifstream file(fname);
    assert(file.good() && "Bad or missing config file");
    string line;
    //std::map<std::string, std::string> options;
    while (getline(file, line)) {
        char delim[] = " \t=";
        vector<string> tokens;

        while (true) {
            size_t start = line.find_first_not_of(delim);
            if (start == string::npos) 
                break;

            size_t end = line.find_first_of(delim, start);
            if (end == string::npos) {
                tokens.push_back(line.substr(start));
                break;
            }

            tokens.push_back(line.substr(start, end - start));
            line = line.substr(end);
        }

        // empty line
        if (!tokens.size())
            continue;

        // comment line
        if (tokens[0][0] == '#')
            continue;

        // parameter line
        assert(tokens.size() == 2 && "Only allow two tokens in one line");

        //options[tokens[0]] = tokens[1];

        string topString = "";

        if (tokens[0] == "topology") {
          topString = (tokens[1].c_str());
        } else if (tokens[0] == "numRows") {
          RRAM.numRows = atoi(tokens[1].c_str());
        } else if (tokens[0] == "numCols") {
          RRAM.numCols = atoi(tokens[1].c_str());
        } else if (tokens[0] == "numBanks") {
          RRAM.numBanks = atoi(tokens[1].c_str());
        } else if (tokens[0] == "numParArr") {
          RRAM.numParArr = atoi(tokens[1].c_str());
        } else if (tokens[0] == "readEnergy") {
          RRAM.readEnergy = stof(tokens[1].c_str());
        } else if (tokens[0] == "writeEnergy") {
          RRAM.writeEnergy = stof(tokens[1].c_str());
        } else if (tokens[0] == "readHalfSel") {
          RRAM.readHalfSel = stof(tokens[1].c_str());
        } else if (tokens[0] == "writeHalfSel") {
          RRAM.writeHalfSel = stof(tokens[1].c_str());
        } else if (tokens[0] == "readLat") {
          RRAM.readLat = stof(tokens[1].c_str());
        } else if (tokens[0] == "writeLat") {
          RRAM.writeLat = stof(tokens[1].c_str());
        } else if (tokens[0] == "TIAEnergy") {
          RRAM.TIAEnergy = stof(tokens[1].c_str());
        } else if (tokens[0] == "TIALat") {
          RRAM.TIALat = stof(tokens[1].c_str());
        } else if (tokens[0] == "decoderEnergy") {
          RRAM.decoderEnergy = stof(tokens[1].c_str());
        } else if (tokens[0] == "decoderLat") {
          RRAM.decoderLat = stof(tokens[1].c_str());
        }

        //convert parsed granularity string into enum
        if(topString.compare("BITWISE")) {
          RRAM.top = BITWISE;
        }
        else if (topString.compare("ROWWISE")) {
          RRAM.top = ROWWISE;
        }
        else if (topString.compare("ROWCOL")) {
          RRAM.top = ROWCOL;
        }
        else {
          assert(0 && "Invalid or missing granularity setting");
        }

    }
    file.close();
}


int main(int argc, char * argv[]){

  RRAMspec RRAM;

  //Check for proper arguments
  if(argc != 5) {
    cout << "error: invalid arguments" << endl;
    cout << "usage: ./crosspoint -c <config_file_name> -f <trace_file_name>" << endl;
    exit(0);
  }

  //Parse command line for trace and config
  parse_args(argc, argv, RRAM);

  //Parse config file
  std::string config_file(RRAM.config);
  parse_config(config_file, RRAM);


  cout << "No. of Banks:         " << RRAM.numBanks    << endl;
  cout << "No. of Rows:          " << RRAM.numRows     << endl;
  cout << "No. of Cols:          " << RRAM.numCols     << endl;
  cout << "No. of Par Arrays:    " << RRAM.numParArr   << endl;
  cout << "Topology:             " << RRAM.top        << endl;
  cout << "Read Energy           " << RRAM.readEnergy  << endl;
  cout << "Write Energy          " << RRAM.writeEnergy << endl;
  cout << "Read Half Select      " << RRAM.readHalfSel << endl;
  cout << "Write Half Select     " << RRAM.writeHalfSel << endl;
  cout << "Read Latency          " << RRAM.readLat     << endl;
  cout << endl;

  //initialize rest of values & allocate memory
  RRAM.set_values();

  //parse trace file, requests will be processed as they are read in
  RRAM.parse_trace();

  //Finished servicing requests, free allocated memory
  //RRAM.free_memory();
  
  //Print outputs
  #ifdef DEBUG
    RRAM.show_stats();
  #else
    RRAM.show_csv(); //prints final output stats in csv format 
    //outputs: total energy, average energy, average power, total latency, average latency
  #endif

  return 0; 
}