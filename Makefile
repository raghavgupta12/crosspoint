OBJS = crosspoint.cpp helper.cpp crosspoint.h helper.h
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG) 
LFLAGS = -Wall -std=c++11 $(DEBUG)


make: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o crosspoint

trace: make_sample_trace.cpp
	$(CC) $(LFLAGS) make_sample_trace.cpp -o trace

mmul: mmul.cpp
	$(CC) $(LFLAGS) mmul.cpp -o mmul

dram: dram.cpp
	$(CC) $(LFLAGS) dram.cpp -o dram

clean:
	rm -rf crosspoint trace trace.bin *~ 

