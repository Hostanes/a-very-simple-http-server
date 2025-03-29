/*
  `http-server.c`

  HTTP server that serves HTML files based on requests
*/

#include "t-vector.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PORT 8050
#define BUFFER_SIZE 1024
#define MAX_PATH_SIZE 256
#define DEFAULT_FILE "html/index.html"

const char *http_404_response =
    "HTTP/1.1 404 Not Found\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 48\r\n"
    "\r\n"
    "<html><body><h1>404 Not Found</h1></body></html>";

const char *http_500_response =
    "HTTP/1.1 500 Internal Server Error\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 54\r\n"
    "\r\n"
    "<html><body><h1>500 Server Error</h1></body></html>";

/*
  Checks for errors 404 and 500
  Otherwise
  Sends request file in chunks
*/
void send_file_response(int client_socket, const char *filepath) {

  /*
    if file not found, send default error 404
  */
  int fd = open(filepath, O_RDONLY);
  if (fd == -1) {
    write(client_socket, http_404_response, strlen(http_404_response));
    return;
  }

  /*
    stat returns file statistics like size
    if fails, send error 500
  */
  struct stat st;
  if (fstat(fd, &st) == -1) {
    close(fd);
    write(client_socket, http_500_response, strlen(http_500_response));
    return;
  }

  /*
    construc OK header with content information:
    - OK status
    - type
    - size
  */
  char headers[512];
  int headers_len = snprintf(headers, sizeof(headers),
                             "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "Content-Length: %ld\r\n"
                             "\r\n",
                             st.st_size);

  write(client_socket, headers, headers_len); // send the headers

  /*
    read the file and write to socket if successful
    read from file and write to socket in chunks
  */
  char buffer[BUFFER_SIZE];
  ssize_t bytes_read;
  while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
    write(client_socket, buffer, bytes_read);
  }

  close(fd);
}

int handle_client(int client_socket) {

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read = read(client_socket, buffer, BUFFER_SIZE - 1);

  if (bytes_read < 0) {
    return -1;
  }

  buffer[bytes_read] = '\0'; // don't forget to add a null terminator!!
  printf("[REQUEST]:\n\n%s\n", buffer);

  /*
    convert strings to tokens, arg2 : delim
    returns NULL pointer if error
    otherwise returns pointer to first token

    modifiers buffer in place
    converts delim to \0
  */
  char *method = strtok(buffer, " "); // returns GET\0
  char *path = strtok(NULL, " "); //  returns the next token eg: /test.html\0

  if (method && path && strcmp(method, "GET") == 0) {
    char filepath[MAX_PATH_SIZE] = "./html";

    if (strcmp(path, "/") == 0) {
      strcat(filepath, DEFAULT_FILE);
    } else {
      strcat(filepath, path);
      printf("[SERVING]: %s\n", filepath);
      send_file_response(client_socket, filepath);
    }

  } else {
    write(client_socket, http_404_response, strlen(http_404_response));
  }
  return 0;
}

int main() {
  int sockfd;
  struct sockaddr_in server_addr;

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed");
    return -1;
  }

  int opt = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt failed");
    close(sockfd);
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

  if (listen(sockfd, 10) == -1) {
    perror("Listen failed");
    close(sockfd);
    return -1;
  }

  printf("Server listening on port %d...\n", PORT);

  while (1) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_sockfd =
        accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

    if (client_sockfd == -1) {
      perror("Accept failed");
      continue;
    }

    printf("Connection accepted from %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    handle_client(client_sockfd);
    close(client_sockfd);
    printf("Response sent. Closing connection.\n");
  }

  close(sockfd);
  printf("Server stopped.\n");

  return 0;
}
