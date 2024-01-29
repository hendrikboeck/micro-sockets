#include "micro_sockets/__macros.h"
#include "micro_sockets/box.h"
#include "micro_sockets/sized_mem.h"
#include "micro_sockets/tcp.h"

#include <stdint.h>
#include <stdlib.h>

int32_t main(void) {
  tcp_client_t *client = tcp_client_new("127.0.0.1", 4040, 0);
  tcp_client_connect(client);

  const char *msg = "hello from client!";
  tcp_client_send(client, box_ctor(__c_cast(uint8_t *, msg), strlen(msg)));
  printf("[client] send: '%s'\n", msg);

  tcp_client_attach_buf(client, sized_mem_new(KiB(4) + 1));
  box_t resp = tcp_client_recv(client);
  printf("[client] received: '%s'\n", __c_cast(char *, resp.ptr));

  tcp_client_close(client);
  tcp_client_free(client);
  return EXIT_SUCCESS;
}
