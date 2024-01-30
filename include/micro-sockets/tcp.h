#ifndef __MICRO_SOCKETS__TCP__H
#define __MICRO_SOCKETS__TCP__H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

#include "ccms/_macros.h"
#include "ccms/box.h"
#include "ccms/sized_memory.h"

#include "micro-sockets/_sock_addr_any.h"

//
//
// ------------------------- TYPES -------------------------
//
//

#ifdef _WIN32
typedef SOCKET Sock;
#else
typedef int32_t Sock;
#endif

//
//
// ------------------------- RAW -------------------------
//
//

// flags for receiving message
#define TCP_BUFTRUNC 1

extern inline ssize_t _tcp_recv(Sock fd, SizedMemory *buf, size_t flags) {
  int32_t buf_truncate = flags & TCP_BUFTRUNC;
  size_t buf_size = buf_truncate ? buf->size - 1 : buf->size;
  ssize_t len = recv(fd, buf->ptr, buf_size, 0);

  if (len >= 0 && buf_truncate)
    buf->ptr[len] = 0;
  return len;
}

extern inline ssize_t _tcp_send(Sock fd, Box data) {
  return send(fd, data.ptr, data.size, 0);
}

extern inline int32_t _tcp_close(Sock fd) {
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

typedef struct TcpConnection TcpConnection;

struct TcpConnection {
  Sock fd;
  _SockAddrAny sa_addr;
  socklen_t sa_len;
};

extern inline int32_t tcp_connection__close(TcpConnection *conn) {
  return _tcp_close(conn->fd);
}

extern inline ssize_t tcp_connection__send(TcpConnection *conn, Box data) {
  return _tcp_send(conn->fd, data);
}

extern inline ssize_t tcp_connection__recv(TcpConnection *conn,
                                           SizedMemory *buf, size_t flags) {
  return _tcp_recv(conn->fd, buf, flags);
}

//
//
// ------------------------- SERVER -------------------------
//
//

#define TCP_SERVER_INET6 1

typedef struct TcpServer TcpServer;

struct TcpServer {
  Sock sock;
  SizedMemory *buf;
  size_t flags;
};

extern inline void tcp_server__free(TcpServer *self) {
  if (self->buf != NULL)
    sized_memory__free(self->buf);
  _M_free(self);
}

extern inline TcpServer *tcp_server__new(const char *addr, const uint16_t port,
                                         const size_t flags) {
#ifdef _WIN32
  // initialize windows specific winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  if (WSAStartup(w_version_requested, &wsa_data) != 0)
    return NULL;
#endif

  TcpServer *self = _M_new(TcpServer);
  int32_t proto = (flags & TCP_SERVER_INET6) ? AF_INET6 : AF_INET;
  _SockAddrAny sa = _sock_addr_any__ctor(proto, addr, port);

  size_t sa_size;
  struct sockaddr *sa_ref;

  if (proto == AF_INET) {
    sa_size = sizeof(struct sockaddr_in);
    sa_ref = _M_cast(struct sockaddr *, _M_addr(sa.inet));
  } else {
    sa_size = sizeof(struct sockaddr_in6);
    sa_ref = _M_cast(struct sockaddr *, _M_addr(sa.inet6));
  }

  self->buf = NULL;
  self->flags = flags;
  self->sock = socket(proto, SOCK_STREAM, IPPROTO_TCP);
  if (self->sock < 0) {
    tcp_server__free(self);
    return NULL;
  }

  if (bind(self->sock, sa_ref, sa_size) != 0) {
    tcp_server__free(self);
    return NULL; // check errno();
  }

  return self;
}

extern inline int32_t tcp_server__attach_buf(TcpServer *self,
                                             SizedMemory *buf) {
  if (self->buf != NULL)
    return 1;

  self->buf = buf;
  return 0;
}

extern inline SizedMemory *tcp_server__dettach_buf(TcpServer *self) {
  SizedMemory *result = self->buf;
  self->buf = NULL;
  return result;
}

extern inline int32_t tcp_server__listen(TcpServer *self, const int32_t n) {
  assert(self->buf != NULL);

  if (listen(self->sock, n) == -1)
    return 1;
  return 0;
}

extern inline TcpConnection tcp_server__accept(TcpServer *self) {
  TcpConnection conn;
  struct sockaddr *sender_sa;

  memset(&conn, 0, sizeof(TcpConnection));
  if (self->flags & TCP_SERVER_INET6)
    sender_sa = _M_cast(struct sockaddr *, _M_addr(conn.sa_addr.inet6));
  else
    sender_sa = _M_cast(struct sockaddr *, _M_addr(conn.sa_addr.inet));
  conn.fd = accept(self->sock, sender_sa, &conn.sa_len);

  return conn;
}

extern inline Box tcp_server__recv(TcpServer *self, TcpConnection *conn) {
  ssize_t size = tcp_connection__recv(conn, self->buf, TCP_BUFTRUNC);

  if (size < 0)
    return box__ctor(NULL, 0);

  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}

extern inline int32_t tcp_server__shutdown(TcpServer *self) {
  return _tcp_close(self->sock);
}

//
//
// ------------------------- CLIENT -------------------------
//
//

#define TCP_CLIENT_INET6 1

typedef struct TcpClient TcpClient;

struct TcpClient {
  _SockAddrAny _server_addr;
  size_t _server_addr_len;
  int32_t _proto;

  Sock sock;
  SizedMemory *buf;
};

extern inline void tcp_client__free(TcpClient *self) {
  if (self->buf != NULL)
    sized_memory__free(self->buf);
  free(self);
}

extern inline TcpClient *tcp_client__new(const char *addr, const uint16_t port,
                                         const size_t flags) {
#ifdef _WIN32
  // initialize windows specific winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  if (WSAStartup(w_version_requested, &wsa_data) != 0)
    return NULL;
#endif

  TcpClient *self = _M_new(TcpClient);
  memset(self, 0, sizeof(TcpClient));

  // private members
  if (flags & TCP_CLIENT_INET6) {
    self->_proto = AF_INET6;
    self->_server_addr_len = sizeof(struct sockaddr_in6);
  } else {
    self->_proto = AF_INET;
    self->_server_addr_len = sizeof(struct sockaddr_in);
  }
  self->_server_addr = _sock_addr_any__ctor(self->_proto, addr, port);

  // public members
  self->buf = NULL;
  self->sock = socket(self->_proto, SOCK_STREAM, IPPROTO_TCP);
  if (self->sock < 0) {
    tcp_client__free(self);
    return NULL;
  }

  return self;
}

extern inline int32_t tcp_client__attach_buf(TcpClient *self,
                                             SizedMemory *buf) {
  if (self->buf != NULL)
    return 1;

  self->buf = buf;
  return 0;
}

extern inline SizedMemory *tcp_client__dettach_buf(TcpClient *self) {
  SizedMemory *result = self->buf;
  self->buf = NULL;
  return result;
}

extern inline int32_t tcp_client__connect(TcpClient *self) {
  struct sockaddr *sa_ref;

  if (self->_proto == AF_INET)
    sa_ref = _M_cast(struct sockaddr *, _M_addr(self->_server_addr.inet));
  else
    sa_ref = _M_cast(struct sockaddr *, _M_addr(self->_server_addr.inet6));

  return connect(self->sock, sa_ref, self->_server_addr_len);
}

extern inline int32_t tcp_client__close(TcpClient *self) {
  return _tcp_close(self->sock);
}

extern inline TcpConnection
tcp_client__as_tcp_connection(const TcpClient *self) {
  return (TcpConnection){self->sock, self->_server_addr,
                         self->_server_addr_len};
}

extern inline ssize_t tcp_client__send(TcpClient *self, Box data) {
  return _tcp_send(self->sock, data);
}

extern inline Box tcp_client__recv(TcpClient *self) {
  ssize_t size = _tcp_recv(self->sock, self->buf, TCP_BUFTRUNC);

  if (size < 0)
    return box__ctor(NULL, 0);

  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}

#endif // __MICRO_SOCKETS__TCP__H
