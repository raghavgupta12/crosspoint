OBJS = crosspoint.cpp helper.cpp crosspoint.h helper.h
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)


make: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o crosspoint

trace: make_sample_trace.cpp
	$(CC) $(LFLAGS) make_sample_trace.cpp -o trace

clean:
	rm -rf crosspoint trace trace.bin *~ 

