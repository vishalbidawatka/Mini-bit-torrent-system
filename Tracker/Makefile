# RollNo : 2018201033  Name : Darshan Kansagara
CC = g++
CFLAGS = -Wall 
DEPS = trackerheader.h 
OBJ = tracker.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -lpthread

tracker: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lpthread

clean:
	rm -rf *o tracker