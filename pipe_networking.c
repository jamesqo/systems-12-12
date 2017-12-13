#include "pipe_networking.h"

#define TO_SERVER_PIPE "mario"
#define TO_CLIENT_PIPE "luigi"

#define TO_SERVER_MESSAGE_1 "Client message 1"
#define TO_CLIENT_MESSAGE   "Server message"
#define TO_SERVER_MESSAGE_2 "Client message 2"

void log_and_sleep(char* message) {
  // https://stackoverflow.com/a/5142028/4077294
  time_t rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  printf("[%s] %s\n", asctime(timeinfo), message);
  sleep(1);
}

void log_fmt_and_sleep(char* message_fmt, char* arg) {
  time_t rawtime;
  struct tm* timeinfo;

  time(&rawtime);
  timeinfo = localtime(&rawtime);

  char buf[256];
  memset(buf, 0, sizeof(buf));
  sprintf(buf, message_fmt, arg);
  printf("[%s] %s\n", asctime(timeinfo), buf);
  sleep(1);
}

/*=========================
  server_handshake
  args: int * to_client

  Perofrms the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  log_and_sleep("Making to-server fifo");
  mkfifo(TO_SERVER_PIPE, 0644);
  log_and_sleep("Making to-client fifo");
  mkfifo(TO_CLIENT_PIPE, 0644);
  log_and_sleep("Opening to-server fifo");
  int fd_read = open(TO_SERVER_PIPE, O_RDONLY);
  log_and_sleep("Opening to-client fifo");
  int fd_write = open(TO_CLIENT_PIPE, O_WRONLY);
  
  log_and_sleep("2. Server receives/checks message from client.");
  char buf[256];
  ssize_t num_read = read(fd_read, buf, sizeof(buf));
  if (strncmp(buf, TO_SERVER_MESSAGE_1, num_read) != 0) {
    fprintf(stderr, "wrong message from client\n");
    exit(1);
  }

  log_fmt_and_sleep("3. Server sends message to client. Sending: %s", TO_CLIENT_MESSAGE);
  write(fd_write, TO_CLIENT_MESSAGE, sizeof(TO_CLIENT_MESSAGE));

  log_and_sleep("6. Server receives second message from client, verifying the client can receive.");
  num_read = read(fd_read, buf, sizeof(buf));
  if (strncmp(buf, TO_SERVER_MESSAGE_2, num_read) != 0) {
    fprintf(stderr, "wrong message from client\n");
    exit(1);
  }
  
  *to_client = fd_write;
  return fd_read;
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

  log_fmt_and_sleep("1. Client sends message to server, verifying the client can transmit. Sending: %s", TO_SERVER_MESSAGE_1);
  write(write_fd, TO_SERVER_MESSAGE_1, sizeof(TO_SERVER_MESSAGE_1));

  log_and_sleep("4. Client receives message from server, verifying the server can receive/transmit.");
  char read_buf[256];
  ssize_t num_read = read(read_fd, read_buf, sizeof(read_buf));

  if (strncmp(read_buf, TO_CLIENT_MESSAGE, num_read) != 0) {
    fprintf(stderr, "Did not receive correct server message.");
    exit(1);
  }

  log_fmt_and_sleep("5. Client sends second message to server. Sending: %s", TO_SERVER_MESSAGE_2);
  write(write_fd, TO_SERVER_MESSAGE_2, sizeof(TO_SERVER_MESSAGE_2));

  *to_server = write_fd;
  return read_fd;
}
