OBJS = crosspoint.cpp helper.cpp crosspoint.h 
CC = g++
DEBUG = -g
CFLAGS = -Wall -c $(DEBUG)
LFLAGS = -Wall $(DEBUG)


make: $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o crosspoint

clean:
	rm -rf crosspoint *~ 

