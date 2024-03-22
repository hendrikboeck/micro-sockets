/******************************************************************************/
/* micro-sockets - A lightweight, header-only C library for simplified        */
/* network socket programming.                                                */
/* Copyright (C) 2024, Hendrik Boeck <hendrikboeck.dev@protonmail.com>        */
/*                                                                            */
/* This program is free software: you can redistribute it and/or modify  it   */
/* under the terms of the GNU General Public License as published by the Free */
/* Software Foundation, either version 3 of the License, or (at your option)  */
/* any later version.                                                         */
/*                                                                            */
/* This program is distributed in the hope that it will be useful, but        */
/* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY */
/* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License   */
/* for more details.                                                          */
/*                                                                            */
/* You should have received a copy of the GNU General Public License along    */
/* with this program.  If not, see <https://www.gnu.org/licenses/>.           */
/******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "ccms/_macros.h"
#include "ccms/box.h"
#include "ccms/sized_memory.h"
#include "micro-sockets/tcp.h"

int32_t main(void) {
  // Create a new TCP server listening on 0.0.0.0:4040
  tcp_server_t* server = tcp_server__new(AF_INET, "0.0.0.0", 4040);

  // Attach a buffer of size 4KiB to the server for receiving data
  tcp_server__attach_buf(server, buf__new(KiB(4)));

  // Start listening for incoming connections, with a backlog of 5
  tcp_server__listen(server, 5);

  // Accept a new connection from a client
  tcp_connection_t conn = tcp_server__accept(server);

  // Receive data from the client
  Box data = tcp_server__recv(server, &conn);
  printf("[server] received: '%s'\n", buf__str(server->buf));

  // Prepare a response message
  const char* msg = "hello from server!";
  Box resp = {.ptr = _M_cast(uint8_t*, msg), .size = strlen(msg)};

  // Send the response to the client
  tcp_connection__send(&conn, resp);
  printf("[server] send: '%s'\n", resp);

  // Close the connection
  tcp_connection__close(&conn);

  // Shutdown and free the server
  tcp_server__shutdown(server);
  tcp_server__free(server);

  return EXIT_SUCCESS;
}
