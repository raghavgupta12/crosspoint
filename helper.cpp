#include "crosspoint.h"



void stats::print_stats(void){

  cout << RED << endl;
  cout << "Total Energy: "      << setw(6)  << total_energy;
  cout << " Average Energy: "   << setw(6)  << average_energy;
  cout << " Average Power: "    << setw(6)  << average_power;
  cout << " Total Latency: "    << setw(6)  << total_latency;
  cout << " Average Latency: "  << setw(6)  << average_latency; 
  cout << endl;
}


void RRAMspec::show_stats(void){

  /* All Energy Statistics will be printed */

  cout << "ENERGY STATS"                    << endl;
  requestStats.print_stats();
  
}
                 
