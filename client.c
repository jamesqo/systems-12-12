#include "pipe_networking.h"


int main() {

  int to_server;
  int from_server;

  from_server = client_handshake( &to_server );

  printf("Finished the handshake.\n");

  char to_server_buf[256];
  char from_server_buf[256];

  // TODO: Not strictly necessary
  memset(to_server_buf, 0, sizeof(to_server_buf));
  memset(from_server_buf, 0, sizeof(from_server_buf));

  while (1) {
    printf("Enter some text: ");
    fgets(to_server_buf, sizeof(to_server_buf), stdin);
    size_t to_server_amt = strlen(to_server_buf);
    printf("Sending to server...\n");
    write(to_server, to_server_buf, to_server_amt);
    printf("Waiting for server...\n");
    ssize_t from_server_amt = read(from_server, from_server_buf, sizeof(from_server_buf) - 1);
    from_server_buf[from_server_amt] = '\0';
    printf("Received from server: %s\n", from_server_buf);
  }
}
