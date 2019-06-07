
#include "helper.h"


/*Initialising stats */
void stats::init_values(){
  
  total_energy    = 0;
  average_energy  = 0;
  average_power   = 0;
  total_latency   = 0;
  average_latency = 0;
  count           = 0;
  halfSelCount    = 0;
  fullSelCount    = 0;
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

void stats::update_sel_count (int par, requestType request, granSel sel, int numRows, int numCols,
                              float rdPwr, float rdHalfSel, float wrPwr, float wrHalfSel){
  if (request == READ){
    if(par == 0) { //non parallel
      if(sel == ROW) {
        fullSelCount += numCols;
        halfSelCount += numCols * (numRows-1);
        halfsel_energy += (rdHalfSel * numCols * (numRows-1));
        fullsel_energy += (numCols * rdPwr);
        //requestEnergy  = ((numCols * rdPwr)  + (rdHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        fullSelCount += numRows;
        halfSelCount += numRows * (numCols-1);
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1 + numCols-1);
      }
      else { //read one matrix element (64 bits)
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1) + (numCols-64);
      }
    }
    else { //par arrays in lockstep
      if(sel == ROW) {
        fullSelCount += par * numCols;
        halfSelCount += par * numCols * (numRows-1);
      }
      else if(sel == COL) {
        fullSelCount += par * numRows;
        halfSelCount += par * numRows * (numCols-1);
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1 + numCols-1);
      }
      else { //read one matrix element (64 bits)
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1) + (numCols-64);
      }
    }
  }
  else if (request == WRITE){
    if(par == 0) { //non parallel
      if(sel == ROW) { 
        fullSelCount += numCols;
        halfSelCount += numCols * (numRows-1);
        //requestEnergy  = ((numCols * rdPwr)  + (rdHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        fullSelCount += numRows;
        halfSelCount += numRows * (numCols-1);
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1 + numCols-1);
      }
      else { //read one matrix element (64 bits)
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1) + (numCols-64);
      }
    }
    else { // par arrays in lockstep
      if(sel == ROW) {
        fullSelCount += par * numCols;
        halfSelCount += par * numCols * (numRows-1);
      }
      else if(sel == COL) {
        fullSelCount += par * numRows;
        halfSelCount += par * numRows * (numCols-1);
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1 + numCols-1);
      }
      else { //read one matrix element (64 bits)
        fullSelCount += 64;
        halfSelCount += 64 * (numRows-1) + (numCols-64);
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

/* Sets parameters for RRAM class to default values*/
/* Overwrite these by calling parse_args*/
void RRAMspec::set_defaults(){
  numBanks    = NUM_BANKS;
  numRows     = NUM_ROWS;
  numCols     = NUM_COLS;
  rdPwr       = RD_PWR;
  rdLat       = RD_LAT;
  rdHalfSel   = RD_HALF_SEL_PWR;
  wrPwr       = WR_PWR;
  wrSetPwr    = WR_SET_BIT_PWR;
  wrLat       = WR_LAT;
  wrHalfSel   = WR_HALF_SEL_PWR;
  notPwr      = NOT_PWR;
  notLat      = NOT_LAT;
  notHalfSel  = NOT_HALF_SEL_PWR;
  orPwr       = OR_PWR;
  orLat       = OR_LAT;
  orHalfSel   = OR_HALF_SEL_PWR;
  traceFilename = FILENAME;
  par         = 0;
}

/* Defining RRAMspec class Functions */
/* Init function for RRAM class */
void RRAMspec::set_values(){
  
  int bank, row, col;

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

}




/* Free RRAM memory */
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




float calculate_request_energy(int par, requestType request, granSel sel, float requestTime, int numRows, int numCols, 
          float rdPwr, float rdHalfSel, float wrPwr, float wrHalfSel, float notPwr, float notHalfSel, float orPwr, float orHalfSel){

  float requestEnergy = 0;
  
  if (request == READ){ // have to add 64* at the front of row/col r/w for parallel version
    if(par == 0) {
      if(sel == ROW) {
        requestEnergy  = ((numCols * rdPwr)  + (rdHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        requestEnergy  = ((numRows * rdPwr)  + (rdHalfSel * numRows * (numCols-1)));
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        requestEnergy  = 64 * ((rdPwr)  + (rdHalfSel * (numRows-1 + numCols-1)));
      }
      else { //read one matrix element (64 bits)
        requestEnergy  = (64 * rdPwr )  + (64 * rdHalfSel * (numRows-1)) + (rdHalfSel * (numCols-64));
      }
    }
    else {
      if(sel == ROW) {
        requestEnergy  = par * ((numCols * rdPwr)  + (rdHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        requestEnergy  = par * ((numRows * rdPwr)  + (rdHalfSel * numRows * (numCols-1)));
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        requestEnergy  = 64 * ((rdPwr)  + (rdHalfSel * (numRows-1 + numCols-1)));
      }
      else { //read one matrix element (64 bits)
        requestEnergy  = (64 * rdPwr )  + (64 * rdHalfSel * (numRows-1)) + (rdHalfSel * (numCols-64));
      }
    }
  }
  else if (request == WRITE){
    if(par == 0) {
      if(sel == ROW) {
        requestEnergy  = ((numCols * wrPwr)  + (wrHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        requestEnergy  = ((numRows * wrPwr )  + (wrHalfSel  * numRows * (numCols-1)));
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        requestEnergy  = 64 * ((wrPwr )  + (wrHalfSel* (numRows-1 + numCols-1)));
      }
      else {
        requestEnergy  = (64 * wrPwr )  + (64 * wrHalfSel * (numRows-1)) + (wrHalfSel * (numCols-64));
      }
    }
    else {
      if(sel == ROW) {
        requestEnergy  = par * ((numCols * wrPwr)  + (wrHalfSel * numCols * (numRows-1)));
      }
      else if(sel == COL) {
        requestEnergy  = par * ((numRows * wrPwr )  + (wrHalfSel  * numRows * (numCols-1)));
      }
      else if (sel == CELL) { //64 single cell operations in parallel
        requestEnergy  = 64 * ((wrPwr )  + (wrHalfSel* (numRows-1 + numCols-1)));
      }
      else {
        requestEnergy  = (64 * wrPwr )  + (64 * wrHalfSel * (numRows-1)) + (wrHalfSel * (numCols-64));
      }
    }
  }
  else if (request == NOT){ //TODO fix not/col with granularity settings
    if(sel == ROW) {
      requestEnergy  = (notPwr * requestTime) + (notHalfSel * requestTime * (numCols * (numRows-2)));
    }
    else if(sel == COL) {
      requestEnergy  = (notPwr * requestTime) + (notHalfSel * requestTime * (numRows * (numCols-2)));
    }
    else if(sel == CELL) {
      requestEnergy  = (notPwr * requestTime) + (notHalfSel * requestTime * (numCols-2 + 2*(numRows-2)));
    }
    else { //sel == CELL_SAMECOL
      requestEnergy  = (notPwr * requestTime) + (notHalfSel * requestTime * (numRows-2 + 2*(numCols-2)));
    }
  }
  else { //OR
    if(sel == ROW) {
      requestEnergy  = (orPwr * requestTime)  + (orHalfSel * requestTime * (numCols * (numRows-2)));
    }
    else if(sel == COL) {
      requestEnergy  = (orPwr * requestTime)  + (orHalfSel * requestTime * (numRows * (numCols-2)));
    }
  }

  return requestEnergy;
}



/* Function to find and add energy, latency consumed by a read/write request. Returns 0 if sucessful.
   Else returns -1*/

int RRAMspec::service_readwrite_request(requestType request, granSel sel, int bank, int row, int col, unsigned int data[DATA_SIZE]){
  if (verify_readwrite_request(request, bank, row, col) == -1){
       return -1;
  } 
  
  if (request == READ){ //TODO add sel casing - need to change buffer implementation & likely prints to make this work
    /*if(sel == ROW) {
      for(int c = 0; c < numCols; c++) {
        for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++) {
          data[fourbytes] = memory[bank][row][c].data[fourbytes];
        }
      }
    }
    else if(sel == COL) {

    }
    else {

    }*/
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
    return -1; //invalid request

  float rt = 0.5 * numRows * 2;
  float ct = 20 * numRows;
  float lat = (rt*ct/2)*(50000 + rt/3)/(1000000*(50000 + rt));

  if(request == READ) {
    requestTime = lat;
  }  
  if(request == WRITE) {
    requestTime = lat;
  }
  if(request == NOT) requestTime = notLat;
  if(request == OR) requestTime = orLat;

  requestEnergy = calculate_request_energy(par, request, sel, requestTime, numRows, numCols, 
          rdPwr, rdHalfSel, wrPwr, wrHalfSel, notPwr, notHalfSel, orPwr, orHalfSel);

  if (request == WRITE){
    requestEnergy += requestTime * wrSetPwr * noSetBits(data);
  }
  
  requestPower = requestEnergy/requestTime;
  requestStats.update_sel_count(par, request, sel, numRows, numCols);
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




/* Function to find and add energy, latency consumed by a bitwise NOT request. 
   Returns 0 if sucessful. Else returns -1*/

int RRAMspec::service_not_request(requestType request, granSel sel, int bank, int rowOne, int colOne,
         int rowTwo, int colTwo){

  if (verify_not_request(request, sel, bank, rowOne, colOne, rowTwo, colTwo) == -1){
      return -1;
  }
  
  if(sel == ROW) {
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
  }
 
  requestTime   = notLat;
  requestEnergy = calculate_request_energy(par, request, sel, requestTime, numRows, numCols, 
          rdPwr, rdHalfSel, wrPwr, wrHalfSel, notPwr, notHalfSel, orPwr, orHalfSel);

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
   Else returns -1*/

int RRAMspec::service_or_request(requestType request, granSel sel, int bank, int rowOne, int colOne, int rowTwo, int colTwo){

  if (verify_or_request(request, sel, bank, rowOne, colOne, rowTwo, colTwo) == -1){
    return -1;
  }

  if(sel == ROW) {
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
  }

  requestTime   = orLat;
  requestEnergy = calculate_request_energy(par, request, sel, requestTime, numRows, numCols, 
          rdPwr, rdHalfSel, wrPwr, wrHalfSel, notPwr, notHalfSel, orPwr, orHalfSel);

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



/*Function to ensure r/w request is valid. Returns 0 if successful, else returns -1*/
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



/*Function to ensure NOT request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_not_request(requestType request, granSel sel, int bank, int rowOne, int colOne,
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



/*Function to ensure OR request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_or_request(requestType request, granSel sel, int bank, int rowOne, int colOne,
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
int RRAMspec::parse(){

  FILE * fp;
  unsigned long long bytes_read;
  int  select, request, bank, row1, col1, row2, col2;
  requestType REQ;
  granSel SEL;
  
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
      0 = Row (two rows)
      1 = Col (two cols)
      2 = Cell (cell-level, either same column or same row)

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
    else if(select == 3)
      SEL = ELEMENT;


    
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


/*
 *** COMMAND LINE FORMAT ***
  ./crosspoint -f <trace_file_name> -b <numBanks> -r <numRows> -c <numCols> -rdp <rdPwr> -rdl <rdLat> -rdh <rdHalfSel>
               -wrp <wrPwr> -wrs <wrSetPwr>-wrl <wrLat> -wrh <wrHalfSel> -np <notPwr> -nl <notLat> -nh <notHalfSel>
               -op <orPwr> -ol <orLat> -oh <orHalfSel>
*/
void parse_args(int argc, char * argv[], RRAMspec &RRAM) {
  for(int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-p") == 0) {
        RRAM.par = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "-f") == 0) {
        RRAM.traceFilename = argv[i+1];
    }
    if (strcmp(argv[i], "-b") == 0) {
        RRAM.numBanks = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "-r") == 0) {
        RRAM.numRows = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "-c") == 0) {
        RRAM.numCols = atoi(argv[i+1]);
    }
    if (strcmp(argv[i], "-rdp") == 0) {
        RRAM.rdPwr = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-rdl") == 0) {
        RRAM.rdLat = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-rdh") == 0) {
        RRAM.rdHalfSel = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-wrp") == 0) {
        RRAM.wrPwr = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-wrs") == 0) {
        RRAM.wrSetPwr = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-wrl") == 0) {
        RRAM.wrLat = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-wrh") == 0) {
        RRAM.wrHalfSel = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-np") == 0) {
        RRAM.notPwr = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-nl") == 0) {
        RRAM.notLat = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-nh") == 0) {
        RRAM.notHalfSel = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-op") == 0) {
        RRAM.orPwr = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-ol") == 0) {
        RRAM.orLat = (float) atof(argv[i+1]);
    }
    if (strcmp(argv[i], "-oh") == 0) {
        RRAM.orHalfSel = (float) atof(argv[i+1]);
    }

  }
    #ifdef DEBUG
  cout << RED << endl << "SETTING UP RRAM VALUES " << BLUE << endl;
  cout << "Trace Name: " << RRAM.traceFilename << endl;
  cout << "No. of Banks:         " << RRAM.numBanks    << endl;
  cout << "No. of Rows:          " << RRAM.numRows     << endl;
  cout << "No. of Cols:          " << RRAM.numCols     << endl;
  cout << endl;
  cout << "RdPwr:  " << RRAM.rdPwr << "    RdLat:  " << RRAM.rdLat << "    RdHalfSelPwr:  " << RRAM.rdHalfSel << endl;
  cout << "WrPwr:  " << RRAM.wrPwr << "    WrLat:  " << RRAM.wrLat << "    WrHalfSelPwr:  " << RRAM.wrHalfSel 
       << "    WrSetBitPwr: " << RRAM.wrSetPwr << endl;
  cout << "NotPwr: " << RRAM.notPwr << "    NotLat: " << RRAM.notLat << "    NotHalfSelPwr: " << RRAM.notHalfSel << endl;
  cout << "OrPwr:  " << RRAM.orPwr << "    OrLat:  " << RRAM.orLat << "    OrHalfSelPwr:  " << RRAM.orHalfSel << endl;
  cout << "Num of parallel arrays: " << RRAM.par <<
  cout << RESET << endl;
    #endif
}


int main(int argc, char * argv[]){

  RRAMspec RRAM;

  //initialize rram parameters to defaults first
  RRAM.set_defaults();
  //parse command line arguments, overwrite parameter values as necessary
  parse_args(argc, argv, RRAM);
  //initialize rest of values & allocate memory
  RRAM.set_values();

  RRAM.parse();
  RRAM.free_memory();
  #ifdef DEBUG
    RRAM.show_stats();
  #else
    RRAM.show_csv(); //prints stats in csv format 
    //total energy, average energy, average power, total latency, average latency
  #endif

  return 0; 
}

