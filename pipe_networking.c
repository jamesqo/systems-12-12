#include "pipe_networking.h"

#define TO_SERVER_PIPE "mario"
#define TO_CLIENT_PIPE "luigi"

#define TO_SERVER_MESSAGE_1 "Client message 1"
#define TO_CLIENT_MESSAGE   "Server message"
#define TO_SERVER_MESSAGE_2 "Client message 2"

/*=========================
  server_handshake
  args: int * to_client

  Perofrms the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  mkfifo(TO_SERVER_PIPE, 0644);
  mkfifo(TO_CLIENT_PIPE, 0644);
  int fd_read = open(TO_SERVER_PIPE, O_RDONLY);
  int fd_write = open(TO_CLIENT_PIPE, O_WRONLY);
  
  char buf[256];
  read(fd_read, buf, sizeof(buf));
  if (strncmp(buf, TO_SERVER_MESSAGE_1, sizeof(TO_SERVER_MESSAGE_1))) {
    printf("wrong message from client\n");
    exit(1);
  }
  
  return 0;
}


/*=========================
  client_handshake
  args: int * to_server

  Perofrms the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int write_fd = open(TO_SERVER_PIPE, O_WRONLY);
  int read_fd = open(TO_CLIENT_PIPE, O_RDONLY);

  write(write_fd, TO_SERVER_MESSAGE_1, sizeof(TO_SERVER_MESSAGE_1));

  char read_buf[256];
  read(read_fd, read_buf, sizeof(read_buf));

  if (strncmp(read_buf, TO_CLIENT_MESSAGE, sizeof(TO_CLIENT_MESSAGE)) != 0) {
    fprintf(stderr, "Did not receive correct server message.");
    exit(1);
  }

  write(write_fd, TO_SERVER_MESSAGE_2, sizeof(TO_SERVER_MESSAGE_2));

  *to_server = write_fd;
  return read_fd;
}
