#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,"Usage: %s [host] [port]\n", argv[0]);
    fprintf(stderr,"Usage: %s [port]\n", argv[0]);
    return 1;
  }

  char *host = NULL;
  char *port = argv[1];

  if (argc == 2) {
    port = argv[1];
  } else if (argc == 3)
  {
    host = argv[1];
    port = argv[2];
  }

  struct addrinfo hint;
  hint.ai_family = AF_UNSPEC;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_protocol = IPPROTO_TCP;
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

  if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen) < 0) {
    perror("bind() failed");
    close(socket_listen);
    freeaddrinfo(bind_address);
    return 1;
  }

  if (listen(socket_listen, 10) < 0) {
    perror("listen() failed");
    close(socket_listen);
    freeaddrinfo(bind_address);
    return 1;
  }

  char ap[100];
  const int family_size = bind_address->ai_family ==
    AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
  
  getnameinfo(bind_address->ai_addr, family_size,
    ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
  printf("Listening to %s:%s\n", ap, port);

  freeaddrinfo(bind_address);

  fd_set master;
  FD_ZERO(&master);
  FD_SET(socket_listen, &master);
  int max_socket = socket_listen; // socket

  while(1) {
    fd_set reads;
    reads = master;
    if (select(max_socket+1, &reads, 0, 0, 0) < 0) {
      perror("select() failed");
      return 1;
    }

    int i; // socket
    for(i = 1; i <= max_socket; ++i) {
      if (FD_ISSET(i, &reads)) {

        if (i == socket_listen) {
          struct sockaddr_storage client_address;
          socklen_t client_len = sizeof(client_address);
          int socket_client = accept(socket_listen,
                  (struct sockaddr*) &client_address,
                  &client_len);
          if (socket_client < 0) {
            perror("accept() failed");
            return 1;
          }

          FD_SET(socket_client, &master);
          if (socket_client > max_socket)
              max_socket = socket_client;

          char address_buffer[100];
          getnameinfo((struct sockaddr*)&client_address,
                  client_len,
                  address_buffer, sizeof(address_buffer), 0, 0,
                  NI_NUMERICHOST);
          printf("New connection from %s\n", address_buffer);

        } else {
          char read[1024];
          int bytes_received = recv(i, read, 1024, 0);
          if (bytes_received <= 0) {
            FD_CLR(i, &master);
            close(i);
            continue;
          } else {
            read[bytes_received-1] = 0;
            printf("Received: %s\n", read);
            if (send(i, read, bytes_received, 0) != bytes_received){
              perror("send() failed");
              close(i);
              return 1;
            }
          }
        }
      }
    }
  }

  return 0;
}