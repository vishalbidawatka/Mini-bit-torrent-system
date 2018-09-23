CC = g++
CFLAGS = -std=c++11 -pthread -lcrypto
all: tracker client
tracker: tracker.cpp
			$(CC) -std=c++11 -pthread -lcrypto -c -o $@ $^
client: client.cpp
			$(CC) -std=c++11 -pthread -lcrypto -c -o $@ $^



