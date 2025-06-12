#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv) {

  if (argc != 3) {
    fprintf(stderr,"Usage: %s [host] [port]\n", argv[0]);
    return 1;
  }

  char *host = argv[1];
  char *port = argv[2];

  struct addrinfo hint;
  hint.ai_family = AF_UNSPEC;
  hint.ai_socktype = SOCK_DGRAM;
  hint.ai_protocol = IPPROTO_UDP;
  hint.ai_flags = AI_PASSIVE;

  struct addrinfo *bind_address;
  if (getaddrinfo(host, port, &hint, &bind_address)) {
    perror("getaddrinfo() failed");
    freeaddrinfo(bind_address);
    return 1;
  }

  int socket_listen = socket(bind_address->ai_family, 
    bind_address->ai_socktype, bind_address->ai_protocol);
  if (socket_listen < 0) {
    perror("socket() failed");
    freeaddrinfo(bind_address);
    return 1;
  }

  if (connect(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
    perror("connect() failed");
    close(socket_listen);
    freeaddrinfo(bind_address);
    return 1;
  }
  freeaddrinfo(bind_address);

  while (1)
  {
    char message[1024];
    printf("Enter message: ");
    if (!fgets(message, sizeof(message), stdin)) break;
    message[strcspn(message, "\n")] = 0;

    send(socket_listen, message, sizeof(message), 0);
    recv(socket_listen, message, sizeof(message), 0);
    printf("Received: %s\n", message);
  }
    

  close(socket_listen);
  return 0;
}