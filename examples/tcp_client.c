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
#include <stdlib.h>

#include "ccms/_macros.h"
#include "micro-sockets/tcp.h"

int32_t main(void) {
  // Create a new TCP client and connect to the server at 127.0.0.1:4040
  tcp_client_t* client = tcp_client__new(AF_INET, "127.0.0.1", 4040);
  tcp_client__connect(client);

  // Prepare a message to send to the server
  const char* msg = "hello from client!";

  // Send the message to the server
  tcp_client__send(client, box__ctor(_M_cast(uint8_t*, msg), strlen(msg)));
  printf("[client] send: '%s'\n", msg);

  // Attach a buffer of size 4KiB to the client for receiving data
  tcp_client__attach_buf(client, buf__new(KiB(4)));

  // Receive a response from the server
  box_t resp = tcp_client__recv(client);
  printf("[client] received: '%s'\n", buf__str(client->buf));

  // Close the connection and free the client
  tcp_client__close(client);
  tcp_client__free(client);

  return EXIT_SUCCESS;
}