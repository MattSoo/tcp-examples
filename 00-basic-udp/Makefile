CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = echo_client echo_server

all: $(TARGET)

echo_client: echo_client.o
	$(CC) $(CFLAGS) -o echo_client echo_client.o

echo_server: echo_server.o
	$(CC) $(CFLAGS) -o echo_server echo_server.o

echo_client.o: echo_client.c
	$(CC) $(CFLAGS) -c echo_client.c -o echo_client.o

echo_server.o: echo_server.c
	$(CC) $(CFLAGS) -c echo_server.c -o echo_server.o

clean:
	rm -f $(TARGET)