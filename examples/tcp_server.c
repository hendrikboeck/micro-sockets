#include "micro_sockets/__macros.h"
#include "micro_sockets/box.h"
#include "micro_sockets/tcp.h"
#include <stdint.h>

int32_t main(void) {
  tcp_server_t *server = tcp_server_new("0.0.0.0", 4040, 0);
  tcp_server_attach_buf(server, sized_mem_new(KiB(4) + 1));
  tcp_server_listen(server, 5);

  tcp_connection_t conn = tcp_server_accept(server);
  box_t data = tcp_server_recv(server, &conn);
  printf("[server] received: '%s'\n", __c_cast(char *, data.ptr));

  const char *resp = "hello from server!";
  tcp_connection_send(&conn, box_ctor(__c_cast(uint8_t *, resp), strlen(resp)));
  printf("[server] send: '%s'\n", resp);
  tcp_connection_close(&conn);

  tcp_server_shutdown(server);
  tcp_server_free(server);
  return 0;
}
