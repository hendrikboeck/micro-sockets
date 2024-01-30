#include <stdint.h>
#include <stdlib.h>

#include "ccms/_macros.h"
#include "ccms/sized_memory.h"

#include "micro-sockets/tcp.h"

int32_t main(void) {
  TcpClient *client = tcp_client__new("127.0.0.1", 4040, 0);
  tcp_client__connect(client);

  const char *msg = "hello from client!";
  tcp_client__send(client, box__ctor(_M_cast(uint8_t *, msg), strlen(msg)));
  printf("[client] send: '%s'\n", msg);

  tcp_client__attach_buf(client, sized_memory__new(KiB(4) + 1));
  Box resp = tcp_client__recv(client);
  printf("[client] received: '%s'\n", _M_cast(char *, resp.ptr));

  tcp_client__close(client);
  tcp_client__free(client);
  return EXIT_SUCCESS;
}
