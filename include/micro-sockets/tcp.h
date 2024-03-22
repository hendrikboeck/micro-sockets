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

#ifndef __MICRO_SOCKETS__TCP__H
#define __MICRO_SOCKETS__TCP__H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off

// Pin to the top of file, becuase it is used in the definitions of include
// macros like __MICRO_SOCKETS__IS_WINDOWS, DO NOT MOVE THIS INCLUDE !!!
#include "micro-sockets/_defs.h"

// clang-format on

#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if __MICRO_SOCKETS__IS_WINDOWS
// WINDOWS specific includes
#include <io.h>
#include <winsock.h>

#else
// UNIX/Linux specific includes
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "ccms/_macros.h"
#include "ccms/box.h"
#include "micro-sockets/sock.h"
#include "micro-sockets/sockaddr.h"

//
//
// ------------------------- CONNECTION -------------------------
//
//

typedef struct tcp_connection_t tcp_connection_t;

struct tcp_connection_t {
  sock_t fd;
  sockaddr_inet_t sa;
};

__MICRO_SOCKETS__INLINE
tcp_connection_t tcp_connection__ctor(sock_t fd, sockaddr_inet_t sa) {
  return (tcp_connection_t){fd, sa};
}

__MICRO_SOCKETS__INLINE
int32_t tcp_connection__close(tcp_connection_t* self) {
  return _sock__close(self->fd);
}

__MICRO_SOCKETS__INLINE
ssize_t tcp_connection__send(tcp_connection_t* self, box_t data) {
  return _sock__send(self->fd, data);
}

__MICRO_SOCKETS__INLINE
ssize_t tcp_connection__recv(tcp_connection_t* conn, buf_t* buf) {
  return _sock__recv(conn->fd, buf);
}

//
//
// ------------------------- SERVER -------------------------
//
//

typedef struct tcp_server_t tcp_server_t;

struct tcp_server_t {
  sock_t sock;
  buf_t* buf;
  sockaddr_inet_t sa;
};

__MICRO_SOCKETS__INLINE
void tcp_server__free(tcp_server_t* self) {
  if (self->buf != NULL) buf__free(self->buf);
  _M_free(self);
}

__MICRO_SOCKETS__INLINE
tcp_server_t* tcp_server__new(const sa_family_t sa_family, const char* addr,
                              const uint16_t port) {
  assert(sa_family == AF_INET || sa_family == AF_INET6);
  tcp_server_t* self = _M_new(tcp_server_t);

  self->buf = NULL;
  self->sa = sockaddr_inet__from(sa_family, addr, port);
  self->sock = _sock__new(sa_family, IPPROTO_TCP);

  if (self->sock < 0) {
    tcp_server__free(self);
    return NULL;
  }

  if (_sock__bind(self->sock, &self->sa) != 0) {
    tcp_server__free(self);
    return NULL;
  }

  return self;
}

__MICRO_SOCKETS__INLINE
void tcp_server__attach_buf(tcp_server_t* self, buf_t* buf) {
  self->buf = buf;
}

__MICRO_SOCKETS__INLINE
buf_t* tcp_server__dettach_buf(tcp_server_t* self) {
  buf_t* result = self->buf;
  self->buf = NULL;

  return result;
}

__MICRO_SOCKETS__INLINE
int32_t tcp_server__listen(tcp_server_t* self, const int32_t n) {
  if (self->buf == NULL) return -1;
  return listen(self->sock, n);
}

__MICRO_SOCKETS__INLINE
tcp_connection_t tcp_server__accept(tcp_server_t* self) {
  tcp_connection_t conn;

  memset(&conn, 0, sizeof(tcp_connection_t));
  conn.sa.family = self->sa.family;

  struct sockaddr* sa = &(conn.sa.addr.sa);
  socklen_t* sa_len = &(conn.sa.size);

  conn.fd = accept(self->sock, sa, sa_len);
  if (conn.fd < 0) {
    printf("FAILED: %s\n", strerror(errno));
  }

  return conn;
}

__MICRO_SOCKETS__INLINE
box_t tcp_server__recv(tcp_server_t* self, tcp_connection_t* conn) {
  ssize_t size = tcp_connection__recv(conn, self->buf);
  if (size < 0) return box__ctor(NULL, 0);

  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}

__MICRO_SOCKETS__INLINE
int32_t tcp_server__shutdown(tcp_server_t* self) {
  return _sock__close(self->sock);
}

//
//
// ------------------------- CLIENT -------------------------
//
//

typedef struct tcp_client_t tcp_client_t;

struct tcp_client_t {
  sockaddr_inet_t server_sa;
  sock_t sock;
  buf_t* buf;
};

__MICRO_SOCKETS__INLINE
void tcp_client__free(tcp_client_t* self) {
  if (self->buf != NULL) buf__free(self->buf);
  free(self);
}

__MICRO_SOCKETS__INLINE
tcp_client_t* tcp_client__new(sa_family_t sa_family, const char* addr,
                              uint16_t port) {
  assert(sa_family == AF_INET || sa_family == AF_INET6);
  tcp_client_t* self = _M_new(tcp_client_t);
  memset(self, 0, sizeof(tcp_client_t));

  self->server_sa = sockaddr_inet__from(sa_family, addr, port);
  self->sock = _sock__new(sa_family, IPPROTO_TCP);

  if (self->sock < 0) {
    tcp_client__free(self);
    return NULL;
  }

  return self;
}

__MICRO_SOCKETS__INLINE
void tcp_client__attach_buf(tcp_client_t* self, buf_t* buf) {
  self->buf = buf;
}

__MICRO_SOCKETS__INLINE
buf_t* tcp_client__dettach_buf(tcp_client_t* self) {
  buf_t* result = self->buf;
  self->buf = NULL;

  return result;
}

__MICRO_SOCKETS__INLINE
int32_t tcp_client__connect(tcp_client_t* self) {
  return connect(self->sock, _M_addr(self->server_sa.addr.sa),
                 self->server_sa.size);
}

__MICRO_SOCKETS__INLINE
int32_t tcp_client__close(tcp_client_t* self) {
  return _sock__close(self->sock);
}

__MICRO_SOCKETS__INLINE
tcp_connection_t tcp_client__as_tcp__connection(const tcp_client_t* self) {
  return tcp_connection__ctor(self->sock, self->server_sa);
}

__MICRO_SOCKETS__INLINE
ssize_t tcp_client__send(tcp_client_t* self, box_t data) {
  return _sock__send(self->sock, data);
}

__MICRO_SOCKETS__INLINE
box_t tcp_client__recv(tcp_client_t* self) {
  ssize_t size = _sock__recv(self->sock, self->buf);
  if (size < 0) return box__ctor(NULL, 0);

  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS__TCP__H
