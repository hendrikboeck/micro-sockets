#ifndef __MICRO_SOCKETS_TCP_H
#define __MICRO_SOCKETS_TCP_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "micro_sockets/__addr.h"
#include "micro_sockets/__macros.h"
#include "micro_sockets/box.h"
#include "micro_sockets/sized_mem.h"

#ifdef _WIN32 // -- WINDOWS
#include <io.h>
#include <winsock.h>

#else // -- UNIX/Linux
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

//
//
// ------------------------- TYPES -------------------------
//
//

#ifdef _WIN32
typedef SOCKET sock_t;
#else
typedef int32_t sock_t;
#endif

//
//
// ------------------------- RAW -------------------------
//
//

// flags for receiving message
#define TCP_BUFTRUNC 1

extern inline ssize_t __tcp_recv(sock_t fd, sized_mem_t *buf, size_t flags) {
  int32_t buf_truncate = flags & TCP_BUFTRUNC;
  size_t buf_size = buf_truncate ? buf->size - 1 : buf->size;
  ssize_t len = recv(fd, buf->ptr, buf_size, 0);

  if (len >= 0 && buf_truncate)
    buf->ptr[len] = 0;
  return len;
}

extern inline ssize_t __tcp_send(sock_t fd, box_t data) {
  return send(fd, data.ptr, data.size, 0);
}

extern inline int32_t __tcp_close(sock_t fd) {
#ifdef _WIN32
  return closesocket(fd);
#else
  return close(fd);
#endif
}

//
//
// ------------------------- CONNECTION -------------------------
//
//

typedef struct {
  sock_t fd;
  struct {
    __sockaddr_any_t addr;
    socklen_t len;
  } sa;
} tcp_connection_t;

extern inline int32_t tcp_connection_close(tcp_connection_t *conn) {
  return __tcp_close(conn->fd);
}

extern inline ssize_t tcp_connection_send(tcp_connection_t *conn, box_t data) {
  return __tcp_send(conn->fd, data);
}

extern inline ssize_t tcp_connection_recv(tcp_connection_t *conn,
                                          sized_mem_t *buf, size_t flags) {
  return __tcp_recv(conn->fd, buf, flags);
}

//
//
// ------------------------- SERVER -------------------------
//
//

#define TCP_SERVER_INET6 1

typedef struct {
  sock_t sock;
  sized_mem_t *buf;
  size_t flags;
} tcp_server_t;

extern inline void tcp_server_free(tcp_server_t *self) {
  if (self->buf != NULL)
    sized_mem_free(self->buf);
  free(self);
}

extern inline tcp_server_t *tcp_server_new(const char *addr, uint16_t port,
                                           size_t flags) {
#ifdef _WIN32
  // initialize windows specific winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  if (WSAStartup(w_version_requested, &wsa_data) != 0)
    return NULL;
#endif

  tcp_server_t *self = __c_cast(tcp_server_t *, malloc(sizeof(tcp_server_t)));
  int32_t proto = (flags & TCP_SERVER_INET6) ? AF_INET6 : AF_INET;
  size_t sa_size = (proto == AF_INET) ? sizeof(struct sockaddr_in)
                                      : sizeof(struct sockaddr_in6);
  __sockaddr_any_t sa = __sockaddr_any_ctor(proto, addr, port);
  struct sockaddr *sa_ref = (proto == AF_INET)
                                ? __c_cast(struct sockaddr *, &sa.inet)
                                : __c_cast(struct sockaddr *, &sa.inet6);

  self->buf = NULL;
  self->flags = flags;
  self->sock = socket(proto, SOCK_STREAM, IPPROTO_TCP);
  if (self->sock < 0) {
    tcp_server_free(self);
    return NULL;
  }

  if (bind(self->sock, sa_ref, sa_size) != 0) {
    tcp_server_free(self);
    return NULL; // check errno();
  }

  return self;
}

extern inline int32_t tcp_server_attach_buf(tcp_server_t *self,
                                            sized_mem_t *buf) {
  if (self->buf != NULL)
    return 1;

  self->buf = buf;
  return 0;
}

extern inline sized_mem_t *tcp_server_dettach_buf(tcp_server_t *self) {
  sized_mem_t *result = self->buf;
  self->buf = NULL;
  return result;
}

extern inline int32_t tcp_server_listen(tcp_server_t *self, const int32_t n) {
  assert(self->buf != NULL);

  if (listen(self->sock, n) == -1)
    return 1;
  return 0;
}

extern inline tcp_connection_t tcp_server_accept(tcp_server_t *self) {
  tcp_connection_t conn;

  memset(&conn, 0, sizeof(tcp_connection_t));
  struct sockaddr *sender_sa =
      (self->flags & TCP_SERVER_INET6)
          ? __c_cast(struct sockaddr *, &(conn.sa.addr.inet6))
          : __c_cast(struct sockaddr *, &(conn.sa.addr.inet));

  conn.fd = accept(self->sock, sender_sa, &conn.sa.len);

  return conn;
}

extern inline box_t tcp_server_recv(tcp_server_t *self,
                                    tcp_connection_t *conn) {
  ssize_t size = tcp_connection_recv(conn, self->buf, TCP_BUFTRUNC);

  return (size < 0) ? box_ctor(NULL, 0)
                    : box_ctor(self->buf->ptr, __c_cast(size_t, size));
}

extern inline int32_t tcp_server_shutdown(tcp_server_t *self) {
  return __tcp_close(self->sock);
}

//
//
// ------------------------- CLIENT -------------------------
//
//

#define TCP_CLIENT_INET6 1

typedef struct {
  __sockaddr_any_t __server_addr;
  size_t __server_addr_len;
  int32_t __proto;

  sock_t sock;
  sized_mem_t *buf;
} tcp_client_t;

extern inline void tcp_client_free(tcp_client_t *self) {
  if (self->buf != NULL)
    sized_mem_free(self->buf);
  free(self);
}

extern inline tcp_client_t *tcp_client_new(const char *addr, uint16_t port,
                                           size_t flags) {
#ifdef _WIN32
  // initialize windows specific winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  if (WSAStartup(w_version_requested, &wsa_data) != 0)
    return NULL;
#endif

  tcp_client_t *self = __c_cast(tcp_client_t *, malloc(sizeof(tcp_client_t)));
  memset(self, 0, sizeof(tcp_client_t));

  // private members
  self->__proto = (flags & TCP_CLIENT_INET6) ? AF_INET6 : AF_INET;
  self->__server_addr = __sockaddr_any_ctor(self->__proto, addr, port);
  self->__server_addr_len = (self->__proto == AF_INET)
                                ? sizeof(struct sockaddr_in)
                                : sizeof(struct sockaddr_in6);

  // public members
  self->buf = NULL;
  self->sock = socket(self->__proto, SOCK_STREAM, IPPROTO_TCP);
  if (self->sock < 0) {
    tcp_client_free(self);
    return NULL;
  }

  return self;
}

extern inline int32_t tcp_client_attach_buf(tcp_client_t *self,
                                            sized_mem_t *buf) {
  if (self->buf != NULL)
    return 1;

  self->buf = buf;
  return 0;
}

extern inline sized_mem_t *tcp_client_dettach_buf(tcp_client_t *self) {
  sized_mem_t *result = self->buf;
  self->buf = NULL;
  return result;
}

extern inline int32_t tcp_client_connect(tcp_client_t *self) {
  struct sockaddr *sa_ref =
      (self->__proto = AF_INET)
          ? __c_cast(struct sockaddr *, &self->__server_addr.inet)
          : __c_cast(struct sockaddr *, &self->__server_addr.inet6);

  return connect(self->sock, sa_ref, self->__server_addr_len);
}

extern inline int32_t tcp_client_close(tcp_client_t *self) {
  return __tcp_close(self->sock);
}

extern inline tcp_connection_t
tcp_client_as_tcp_connection(const tcp_client_t *self) {
  return (tcp_connection_t){self->sock,
                            {self->__server_addr, self->__server_addr_len}};
}

extern inline ssize_t tcp_client_send(tcp_client_t *self, box_t data) {
  return __tcp_send(self->sock, data);
}

extern inline box_t tcp_client_recv(tcp_client_t *self) {
  ssize_t size = __tcp_recv(self->sock, self->buf, TCP_BUFTRUNC);

  return (size < 0) ? box_ctor(NULL, 0)
                    : box_ctor(self->buf->ptr, __c_cast(size_t, size));
}

#endif // __MICRO_SOCKETS_TCP_H
