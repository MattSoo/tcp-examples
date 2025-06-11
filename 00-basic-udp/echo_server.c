#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {

  char *host = NULL;
  char *port = "12345";

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM; // UDP
  hints.ai_protocol = IPPROTO_UDP;
  hints.ai_flags = AI_PASSIVE; // Use my IP address

  struct addrinfo *bind_address;
  if (getaddrinfo(host, port, &hints, &bind_address)) {
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

  char ap[100];
  const int family_size = bind_address->ai_family ==
    AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
  
  getnameinfo(bind_address->ai_addr, family_size,
    ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
  printf("Successfully bind to %s:%s\n", ap, port);

  freeaddrinfo(bind_address);

  while(1) {
    struct sockaddr_storage client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[1024];

    ssize_t bytes_received = recvfrom(socket_listen, buffer, sizeof(buffer), 0,
      (struct sockaddr *)&client_address, &client_address_len);
    if (bytes_received < 0) {
      perror("recvfrom() failed");
      close(socket_listen);
      return 1;
    }

    buffer[bytes_received-1] = 0;

    char client_ip[INET6_ADDRSTRLEN];
    getnameinfo((struct sockaddr *)&client_address, client_address_len,
      client_ip, sizeof(client_ip), 0, 0, NI_NUMERICHOST);
    printf("Received %ld bytes from %s\n", bytes_received, client_ip);
    printf("Echo Message: %s\n", buffer);

    ssize_t bytes_sent = sendto(socket_listen, buffer, bytes_received, 0,
      (struct sockaddr *)&client_address, client_address_len);
    if (bytes_sent < 0) {
      perror("sendto() failed");
      close(socket_listen);
      return 1;
    } else if (bytes_sent != bytes_received){
      perror("sendto() did not send all bytes");
      close(socket_listen);
      return 1;
    }
    
  }

  close(socket_listen);
  return 0;

}