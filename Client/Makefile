# RollNo : 2018201033  Name : Darshan Kansagara
CC = g++
CFLAGS = -Wall 
DEPS = clientheader.h 
OBJ = log.o share.o get.o remove.o seeder.o mtorrentcreator.o client.o
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $< -lssl -lcrypto -lpthread

client: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -lssl -lcrypto -lpthread

clean:
	rm -rf *o client