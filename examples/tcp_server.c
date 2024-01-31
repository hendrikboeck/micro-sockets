#include <stdint.h>
#include <stdlib.h>

#include "ccms/_macros.h"
#include "ccms/box.h"

#include "ccms/sized_memory.h"
#include "micro-sockets/tcp.h"

int32_t main(void) {
  TcpServer *server = tcp_server__new("0.0.0.0", 4040, 0);
  tcp_server__attach_buf(server, sized_memory__new(KiB(4) + 1));
  tcp_server__listen(server, 5);

  TcpConnection conn = tcp_server__accept(server);
  Box data = tcp_server__recv(server, &conn);
  printf("[server] received: '%s'\n", _M_cast(char *, data.ptr));

  const char *resp = "hello from server!";
  tcp_connection__send(&conn, box__ctor(_M_cast(uint8_t *, resp), strlen(resp)));
  printf("[server] send: '%s'\n", resp);
  tcp_connection__close(&conn);

  tcp_server__shutdown(server);
  tcp_server__free(server);
  return EXIT_SUCCESS;
}
