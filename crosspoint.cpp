
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




float calculate_request_energy(requestType request, float requestTime, int sameRowLOP, int numRows, int numCols, 
          float rdPwr, float rdHalfSel, float wrPwr, float wrHalfSel, float notPwr, float notHalfSel, float orPwr, float orHalfSel){

  float requestEnergy = 0;
  
  if (request == READ){
    requestEnergy  = (rdPwr * requestTime)  + (rdHalfSel * requestTime * (numRows-1 + numCols-1));
  }
  else if (request == WRITE){
    requestEnergy  = (wrPwr * requestTime)  + (wrHalfSel * requestTime * (numRows-1 + numCols-1));
  }
  else if (request == NOT){
    requestEnergy  = (notPwr * requestTime) + (notHalfSel * requestTime * (numRows-1 + numCols-1));
  }
  else { //OR
    requestEnergy  = (orPwr * requestTime)  + (orHalfSel * requestTime * (numRows-1 + numCols-1));
    requestEnergy += (sameRowLOP) ? (orHalfSel * requestTime * (numCols-1)) : (orHalfSel * requestTime * (numRows-1));
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

    
  requestTime   = (request == READ) ? rdLat : wrLat;
  requestEnergy = calculate_request_energy(request, requestTime, 0, numRows, numCols, 
          rdPwr, rdHalfSel, wrPwr, wrHalfSel, notPwr, notHalfSel, orPwr, orHalfSel);

  if (request == WRITE){
    requestEnergy += requestTime * wrSetPwr * noSetBits(data);
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




/* Function to find and add energy, latency consumed by a bitwise NOT request on a cell. 
   Returns 0 if sucessful. Else returns -1*/

int RRAMspec::service_not_request(requestType request, int bank, int row, int col){

  if (verify_not_request(request, bank, row, col) == -1){
      return -1;
  }
  
  for(int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
      memory[bank][row][col].data[fourbytes] = ~memory[bank][row][col].data[fourbytes];
  }
 
  requestTime   = notLat;
  requestEnergy = calculate_request_energy(request, requestTime, 0, numRows, numCols, 
          rdPwr, rdHalfSel, wrPwr, wrHalfSel, notPwr, notHalfSel, orPwr, orHalfSel);

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
  cout << endl << hex;
  cout <<  " DATA:";
  cout << setfill('0');
  for (int fourbytes = 0; fourbytes < DATA_SIZE-1; fourbytes++){
    cout << setw(8) << memory[bank][row][col].data[fourbytes]  << "_";
  }
  cout << setw(8) << memory[bank][row][col].data[DATA_SIZE-1];
  cout << dec << RESET << endl << endl;
  cout << setfill(' ');
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

  requestTime   = orLat;
  requestEnergy = calculate_request_energy(request, requestTime, (rowOne == rowTwo), numRows, numCols, 
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
int RRAMspec::verify_not_request(requestType request, int bank, int row, int col){

  if (request != NOT)
    return -1;
  if ((bank > numBanks) || (bank < 0))
    return -1;
  if ((row > numRows) || (row < 0))
    return -1;
  if ((col > numCols) || (col < 0))
    return -1;

  return 0;
}



/*Function to ensure OR request is valid. Returns 0 if successful, else returns -1*/
int RRAMspec::verify_or_request(requestType request, int bank, int rowOne, int colOne,
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

  return 0;
}


/* Parses trace and services requests */
int RRAMspec::parse(){

  FILE * fp;
  unsigned long long bytes_read;
  int  request, bank, row1, col1, row2, col2;
  requestType REQ;
  
  unsigned int data[DATA_SIZE];
  
  fp = fopen(traceFilename, "rb");
  
  if (fp == NULL){
    cout << RED << "TRACE NOT FOUND" << RESET << endl;
    return -1;
  }
  
  while (fread((&bytes_read), sizeof(unsigned long long), 1, fp) != 0){

    /*  
        MSB
        2 bits 0
        3 bits REQ (REQ = 0 -> READ, REQ = 1 -> WRITE, REQ = 2 -> NOT, REQ = 3 -> OR)
        9 bits bank
        15 bits row2
        10 bits col2
        15 bits row1
        10 bits col1 
        LSB
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


/*
 *** COMMAND LINE FORMAT ***
  ./crosspoint -f <trace_file_name> -b <numBanks> -r <numRows> -c <numCols> -rdp <rdPwr> -rdl <rdLat> -rdh <rdHalfSel>
               -wrp <wrPwr> -wrs <wrSetPwr>-wrl <wrLat> -wrh <wrHalfSel> -np <notPwr> -nl <notLat> -nh <notHalfSel>
               -op <orPwr> -ol <orLat> -oh <orHalfSel>
*/
void parse_args(int argc, char * argv[], RRAMspec &RRAM) {
  for(int i = 1; i < argc; i++) {
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
  cout << RED << endl << "SETTING UP RRAM VALUES " << BLUE << endl;
  cout << "No. of Banks:         " << RRAM.numBanks    << endl;
  cout << "No. of Rows:          " << RRAM.numRows     << endl;
  cout << "No. of Cols:          " << RRAM.numCols     << endl;
  cout << RESET << endl;
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
  RRAM.show_stats();

  return 0; 
}

