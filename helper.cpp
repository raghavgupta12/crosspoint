#include "helper.h"


void stats::print_stats(void){

  cout << RED << endl;
  cout << "Total Energy: "      << setw(6)  << total_energy;
  cout << " Average Energy: "   << setw(6)  << average_energy;
  cout << " Average Power: "    << setw(6)  << average_power;
  cout << " Total Latency: "    << setw(6)  << total_latency;
  cout << " Average Latency: "  << setw(6)  << average_latency; 
  cout << endl;
}


unsigned int noSetBits(unsigned int data[DATA_SIZE]){

  unsigned int count = 0;

  for (int fourbytes = 0; fourbytes < DATA_SIZE; fourbytes++){
    count += __builtin_popcount(data[fourbytes]);
  }
  return count;
}


void RRAMspec::show_stats(void){

  /* All Energy Statistics will be printed */

  cout << "ENERGY STATS"                    << endl;
  requestStats.print_stats();
  
}

void stats::print_csv(void){
  cout << total_energy << "," << average_energy << "," << average_power << "," << total_latency << "," << average_latency;
  cout << endl;
}

void RRAMspec::show_csv(void){
  requestStats.print_csv();
}
                 

