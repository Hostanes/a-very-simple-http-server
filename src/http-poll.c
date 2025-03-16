

/*
  `http-poll.c`

  http server that accepts one Connection request
  and returns a single default http response, then closes.
*/

#include "t-vector.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define PORT 8050
#define BUFFER_SIZE 1024

/* default http response */
const char *http_response = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/plain\r\n"
                            "Content-Length: 13\r\n"
                            "\r\n"
                            "Hello, World!";

void handle_client(int client_socket) {
  char buffer[BUFFER_SIZE];
  ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);
  if (bytes_read > 0) {
    buffer[bytes_read] = '\0';
    printf("Received request:\n%s\n", buffer);

    write(client_socket, http_response, strlen(http_response));
  }
  close(client_socket);
}

int main() {

  int sockfd;
  struct sockaddr_in server_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    return -1;
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("Bind failed");
    close(sockfd);
    return -1;
  }

  struct dynArray_t *clients = create_Array(2);

  if (listen(sockfd, 1) == -1) {
    perror("Listen failed");
    close(sockfd);
    return -1;
  }

  printf("Server listening on port %d...\n", PORT);

  int client_sockfd;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);

  client_sockfd =
      accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (client_sockfd == -1) {
    perror("Accept failed");
    close(sockfd);
    return -1;
  }

  printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr),
         ntohs(client_addr.sin_port));

  handle_client(client_sockfd);

  // Close the client socket
  close(client_sockfd);
  printf("Response sent. Closing connection.\n");

  // Close the server socket
  close(sockfd);
  printf("Server stopped.\n");

  return 0;
}
