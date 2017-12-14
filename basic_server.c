#include "pipe_networking.h"

char rot13(char orig) {
  if (tolower(orig) >= 'a' && tolower(orig) <= 'm') {
    return orig + 13;
  }
  if (tolower(orig) >= 'n' && tolower(orig) <= 'z') {
    return orig - 13;
  }
  return orig;
}

void transform_text(char* str) {
  size_t len = strlen(str);
  for (size_t i = 0; i < len; i++) {
    str[i] = rot13(str[i]);
  }
}

int main() {

  int to_client;
  int from_client;

  from_client = server_handshake( &to_client );

  char client_buf[256];

  // TODO: This goes in an erratic loop when the client quits.

  while (1) {
    printf("Waiting for client request...\n");
    ssize_t from_client_amt = read(from_client, client_buf, sizeof(client_buf) - 1);
    client_buf[from_client_amt] = '\0';
    printf("Client sent: %s\n", client_buf);
    transform_text(client_buf);
    printf("Sending response: %s\n", client_buf);
    write(to_client, client_buf, strlen(client_buf));
  }
}
