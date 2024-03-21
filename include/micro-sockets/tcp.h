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
#include "ccms/sized_memory.h"
#include "micro-sockets/sock_addr_any.h"
#include "micro-sockets/socket.h"

//
//
// ------------------------- CONNECTION -------------------------
//
//

/**
 * @brief Typedef for the TcpConnection struct.
 */
typedef struct TcpConnection TcpConnection;

/**
 * @brief Structure representing a TCP connection.
 *
 * @var SockFd TcpConnection::fd
 * The socket descriptor.
 *
 * @var SockAddrAny TcpConnection::sa_addr
 * The socket address.
 *
 * @var socklen_t TcpConnection::sa_len
 * The length of the socket address.
 */
struct TcpConnection {
  SockFd fd;
  SockAddrAny sa_addr;
  socklen_t sa_len;
};

/**
 * @brief Creates a new TcpConnection.
 *
 * This function creates a new TcpConnection with the specified socket file
 * descriptor, socket address, and socket address length.
 *
 * @param fd The socket file descriptor for the new TcpConnection.
 * @param sa_addr The socket address for the new TcpConnection.
 * @param sa_len The socket address length for the new TcpConnection.
 *
 * @return A new TcpConnection with the specified parameters.
 */
__MICRO_SOCKETS__INLINE
TcpConnection tcp_connection__ctor(SockFd fd, SockAddrAny sa_addr,
                                   socklen_t sa_len) {
  return (TcpConnection){fd, sa_addr, sa_len};
}

/**
 * @brief Closes a TCP connection.
 *
 * @param conn The TCP connection to close.
 *
 * @return 0 on success, or -1 on error.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_connection__close(TcpConnection* conn) {
  return _sock__close(conn->fd);
}

/**
 * @brief Sends a message on a TCP connection.
 *
 * @param conn The TCP connection.
 * @param data The data to send.
 *
 * @return The number of bytes sent, or -1 on error.
 */
__MICRO_SOCKETS__INLINE
ssize_t tcp_connection__send(TcpConnection* conn, Box data) {
  return _sock__send(conn->fd, data);
}

/**
 * @brief Receives a message from a TCP connection.
 *
 * @param conn The TCP connection.
 * @param buf The buffer to store the received data.
 * @param flags Flags for receiving message.
 *
 * @return The length of the message received, or -1 on error.
 */
__MICRO_SOCKETS__INLINE
ssize_t tcp_connection__recv(TcpConnection* conn, SizedMemory* buf,
                             size_t flags) {
  return _sock__recv(conn->fd, buf, flags);
}

//
//
// ------------------------- SERVER -------------------------
//
//

/**
 * @brief Macro used as an optional flag for the tcp_server__new function.
 *
 * When this macro is set, the tcp_server__new function will create a TcpServer
 * that operates in IPv6 mode.
 */
#define TCP_SERVER_INET6 1

/**
 * @brief Typedef for the TcpServer struct.
 */
typedef struct TcpServer TcpServer;

/**
 * @brief Structure representing a TCP server.
 *
 * @var SockFd TcpServer::sock
 * The socket descriptor for the server.
 *
 * @var SizedMemory* TcpServer::buf
 * The buffer used for receiving data.
 *
 * @var size_t TcpServer::flags
 * Flags used for server configuration, such as TCP_SERVER_INET6.
 */
struct TcpServer {
  SockFd sock;
  SizedMemory* buf;
  size_t flags;
};

/**
 * @brief Frees the resources associated with a TcpServer.
 *
 * This function frees the buffer associated with the TcpServer, if it exists,
 * and then frees the TcpServer itself.
 *
 * @param self The TcpServer to free.
 */
__MICRO_SOCKETS__INLINE
void tcp_server__free(TcpServer* self) {
  if (self->buf != NULL) sized_memory__free(self->buf);
  _M_free(self);
}

/**
 * @brief Creates a new TcpServer.
 *
 * This function creates a new TcpServer that listens on the specified address
 * and port. The flags parameter can be used to configure the server, such as
 * setting it to operate in IPv6 mode.
 *
 * @param addr The address that the server should listen on.
 * @param port The port that the server should listen on.
 * @param flags Flags used for server configuration, such as TCP_SERVER_INET6.
 * Use 0 (no flags) as default.
 *
 * @return A pointer to the new TcpServer, or NULL if an error occurred.
 */
__MICRO_SOCKETS__INLINE
TcpServer* tcp_server__new(const char* addr, const uint16_t port,
                           const size_t flags) {
  // Allocate memory for a new TcpServer
  TcpServer* self = _M_new(TcpServer);
  // Zero out the memory of the new TcpServer
  memset(self, 0, sizeof(TcpServer));

  // Determine the protocol based on TCP_SERVER_INET6 flag
  int32_t ipv = (flags & TCP_SERVER_INET6) ? AF_INET6 : AF_INET;

  // Construct the socket address
  SockAddrAny sa = sock_addr_any__ctor(ipv, addr, port);

  // Initialize the flags
  self->flags = flags;

  // Create the socket
  self->sock = _sock__new(&sa, ipv, IPPROTO_TCP);

  // If socket creation failed, free the TcpServer and return NULL
  if (self->sock < 0) {
    tcp_server__free(self);
    return NULL;
  }

  if (_sock__bind(self->sock, &sa, ipv) != 0) {
    tcp_server__free(self);
    return NULL;
  }

  // Return the new TcpServer
  return self;
}

/**
 * @brief Attaches a buffer to a TcpServer.
 *
 * This function attaches a given buffer to a TcpServer for receiving data.
 * If the TcpServer already has a buffer attached, the function will return
 * -1.
 *
 * @param self The TcpServer to which the buffer should be attached.
 * @param buf The buffer to attach to the TcpServer.
 *
 * @return 0 if the buffer was successfully attached, or -1 if the TcpServer
 * already had a buffer.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_server__attach_buf(TcpServer* self, SizedMemory* buf) {
  if (self->buf != NULL) return -1;

  self->buf = buf;
  return 0;
}

/**
 * @brief Detaches a buffer from a TcpServer.
 *
 * This function detaches the current buffer from a TcpServer and returns it.
 * If the TcpServer does not have a buffer attached, the function will return
 * NULL.
 *
 * @param self The TcpServer from which the buffer should be detached.
 *
 * @return The buffer that was detached from the TcpServer, or NULL if no
 * buffer was attached.
 */
__MICRO_SOCKETS__INLINE
SizedMemory* tcp_server__dettach_buf(TcpServer* self) {
  SizedMemory* result = self->buf;
  self->buf = NULL;

  return result;
}

/**
 * @brief Starts the TcpServer listening for incoming connections.
 *
 * This function starts the TcpServer listening for incoming connections.
 * If the TcpServer has no buffer attached, the function will return 1.
 * If the listen operation fails, the function will return -1.
 *
 * @param self The TcpServer that should start listening.
 * @param n The maximum length of the queue of pending connections.
 *
 * @return 0 if the server successfully started listening, 1 if the TcpServer
 * has a buffer attached, or -1 if the listen operation failed.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_server__listen(TcpServer* self, const int32_t n) {
  if (self->buf != NULL) return 1;
  return listen(self->sock, n);
}

/**
 * @brief Accepts a new connection on a TcpServer.
 *
 * This function accepts a new connection on a TcpServer and returns a
 * TcpConnection representing the new connection. The function will use IPv6
 * if the TCP_SERVER_INET6 flag is set on the TcpServer.
 *
 * @param self The TcpServer that should accept the new connection.
 *
 * @return A TcpConnection representing the new connection.
 */
__MICRO_SOCKETS__INLINE
TcpConnection tcp_server__accept(TcpServer* self) {
  struct sockaddr* sender_sa;

  // Initialize a new TcpConnection
  TcpConnection conn;
  memset(&conn, 0, sizeof(TcpConnection));

  // If the server is set to use IPv6, set the sender_sa to the IPv6 address
  // Otherwise, set it to the IPv4 address
  if (self->flags & TCP_SERVER_INET6)
    sender_sa = _M_cast(struct sockaddr*, _M_addr(conn.sa_addr.inet6));
  else
    sender_sa = _M_cast(struct sockaddr*, _M_addr(conn.sa_addr.inet));

  // Accept a new connection on the server's socket and store the file
  // descriptor in the TcpConnection
  conn.fd = accept(self->sock, sender_sa, &conn.sa_len);

  // Return the new TcpConnection
  return conn;
}

/**
 * @brief Receives data from a TcpConnection on a TcpServer.
 *
 * This function receives data from a TcpConnection into the TcpServer's
 * buffer. If the receive operation fails, the function will return an empty
 * Box. Otherwise, it will return a Box containing the received data.
 *
 * @param self The TcpServer that should receive the data.
 * @param conn The TcpConnection from which the data should be received.
 *
 * @return A Box containing the received data, or an empty Box if the receive
 * operation failed.
 */
__MICRO_SOCKETS__INLINE
Box tcp_server__recv(TcpServer* self, TcpConnection* conn) {
  // Receive data from the TcpConnection into the server's buffer
  ssize_t size = tcp_connection__recv(conn, self->buf, RECV_BUF_AUTOTRUNC);

  // If the receive operation failed, return an empty Box
  if (size < 0) return box__ctor(NULL, 0);

  // Otherwise, return a Box containing the received data
  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}
/**
 * @brief Shuts down a TcpServer.
 *
 * This function shuts down a TcpServer by closing its socket.
 * The return value is the result of the close operation.
 *
 * @param self The TcpServer that should be shut down.\
 *
 * @return 0 if the shutdown was successful, or -1 if an error occurred.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_server__shutdown(TcpServer* self) {
  return _sock__close(self->sock);
}

//
//
// ------------------------- CLIENT -------------------------
//
//

/**
 * @brief Macro used as an optional flag for the tcp_client__new function.
 *
 * When this macro is set, the tcp_client__new function will create a TcpClient
 * that operates in IPv6 mode.
 */
#define TCP_CLIENT_INET6 1

/**
 * @brief Typedef for the TcpClient struct.
 */
typedef struct TcpClient TcpClient;

/**
 * @brief Represents a TCP client.
 *
 * This struct represents a TCP client, which includes the server address,
 * the length of the server address, the protocol used (IPv4 or IPv6),
 * the socket used for communication, and a buffer for receiving data.
 *
 * @var SockAddrAny _server_addr
 * The server address to which the client is connected.
 *
 * @var size_t _server_addr_len
 * The length of the server address in bytes.
 *
 * @var int32_t _proto
 * The protocol used by the client (IPv4 or IPv6).
 *
 * @var SockFd sock
 * The socket used by the client for communication with the server.
 *
 * @var SizedMemory *buf
 * The buffer used by the client for receiving data.
 */
struct TcpClient {
  SockAddrAny _server_addr;
  size_t _server_addr_len;
  int32_t _proto;

  SockFd sock;
  SizedMemory* buf;
};

/**
 * @brief Frees a TcpClient.
 *
 * This function frees the memory used by a TcpClient.
 * If the TcpClient has a buffer attached, the buffer is also freed.
 *
 * @param self The TcpClient that should be freed.
 */
__MICRO_SOCKETS__INLINE
void tcp_client__free(TcpClient* self) {
  if (self->buf != NULL) sized_memory__free(self->buf);
  free(self);
}

/**
 * @brief Creates a new TcpClient.
 *
 * This function creates a new TcpClient and initializes it with the given
 * address, port, and flags. If the TCP_CLIENT_INET6 flag is set, the client
 * will use IPv6. Otherwise, it will use IPv4. If the socket creation fails,
 * the function will free the TcpClient and return NULL.
 *
 * @param addr The address to which the client should connect.
 * @param port The port on which the client should connect.
 * @param flags The flags to use when creating the client.
 *
 * @return A pointer to the new TcpClient, or NULL if the socket creation
 * failed.
 */
__MICRO_SOCKETS__INLINE
TcpClient* tcp_client__new(const char* addr, const uint16_t port,
                           const size_t flags) {
  // Allocate memory for a new TcpClient
  TcpClient* self = _M_new(TcpClient);
  // Zero out the memory of the new TcpClient
  memset(self, 0, sizeof(TcpClient));

  // If the protocol is IPv6
  if (flags & TCP_CLIENT_INET6) {
    self->_proto = AF_INET6;
    self->_server_addr_len = sizeof(struct sockaddr_in6);
  }
  // If the protocol is IPv4
  else {
    self->_proto = AF_INET;
    self->_server_addr_len = sizeof(struct sockaddr_in);
  }
  // Initialize the server address
  self->_server_addr = sock_addr_any__ctor(self->_proto, addr, port);

  self->sock = _sock__new(self->_proto, IPPROTO_TCP);
  // If socket creation fails, free the TcpClient and return NULL
  if (self->sock < 0) {
    tcp_client__free(self);
    return NULL;
  }

  // Return the new TcpClient
  return self;
}

/**
 * @brief Attaches a buffer to a TcpClient.
 *
 * This function attaches a buffer to a TcpClient for receiving data.
 * If the TcpClient already has a buffer attached, the function will return
 * -1.
 *
 * @param self The TcpClient to which the buffer should be attached.
 * @param buf The buffer to attach to the TcpClient.
 *
 * @return 0 if the buffer was successfully attached, or -1 if the TcpClient
 * already has a buffer.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_client__attach_buf(TcpClient* self, SizedMemory* buf) {
  if (self->buf != NULL) return -1;

  self->buf = buf;
  return 0;
}

/**
 * @brief Detaches a buffer from a TcpClient.
 *
 * This function detaches a buffer from a TcpClient and returns it.
 * After the function is called, the TcpClient will no longer have a buffer
 * attached.
 *
 * @param self The TcpClient from which the buffer should be detached.
 * @return The buffer that was detached from the TcpClient.
 */
__MICRO_SOCKETS__INLINE
SizedMemory* tcp_client__dettach_buf(TcpClient* self) {
  SizedMemory* result = self->buf;
  self->buf = NULL;

  return result;
}

/**
 * @brief Connects a TcpClient to a server.
 *
 * This function connects a TcpClient to the server specified in the TcpClient's
 * _server_addr field. The protocol used for the connection is determined by the
 * TcpClient's _proto field.
 *
 * @param self The TcpClient that should be connected to the server.
 *
 * @return 0 if the connection was successful, or -1 if the connection failed.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_client__connect(TcpClient* self) {
  struct sockaddr* sa_ref;

  if (self->_proto == AF_INET)
    sa_ref = _M_cast(struct sockaddr*, _M_addr(self->_server_addr.inet));
  else
    sa_ref = _M_cast(struct sockaddr*, _M_addr(self->_server_addr.inet6));

  return connect(self->sock, sa_ref, self->_server_addr_len);
}

/**
 * @brief Closes a TcpClient's connection.
 *
 * This function closes the connection of a TcpClient by closing its socket.
 *
 * @param self The TcpClient whose connection should be closed.
 *
 * @return 0 if the connection was successfully closed, or -1 if an error
 * occurred.
 */
__MICRO_SOCKETS__INLINE
int32_t tcp_client__close(TcpClient* self) {
  return _sock__close(self->sock);
}

/**
 * @brief Converts a TcpClient to a TcpConnection.
 *
 * This function creates a TcpConnection from a TcpClient by using the
 * TcpClient's socket, server address, and server address length.
 *
 * @param self The TcpClient that should be converted to a TcpConnection.
 *
 * @return A TcpConnection that represents the same connection as the TcpClient.
 */
__MICRO_SOCKETS__INLINE
TcpConnection tcp_client__as_tcp_connection(const TcpClient* self) {
  return tcp_connection__ctor(self->sock, self->_server_addr,
                              self->_server_addr_len);
}

/**
 * @brief Sends data from a TcpClient to a server.
 *
 * This function sends data from a TcpClient to the server it is connected to.
 * The data is sent over the TcpClient's socket.
 *
 * @param self The TcpClient that should send the data.
 * @param data The data that should be sent.
 *
 * @return The number of bytes that were sent, or -1 if an error occurred.
 */
__MICRO_SOCKETS__INLINE
ssize_t tcp_client__send(TcpClient* self, Box data) {
  return _sock__send(self->sock, data);
}

/**
 * @brief Receives data from a server to a TcpClient.
 *
 * This function receives data from the server connected to the TcpClient.
 * The data is received over the TcpClient's socket and stored in its buffer.
 *
 * @param self The TcpClient that should receive the data.
 *
 * @return A Box containing the received data and its size. If an error
 * occurred, the Box's pointer will be NULL and its size will be 0.
 */
__MICRO_SOCKETS__INLINE
Box tcp_client__recv(TcpClient* self) {
  ssize_t size = _sock__recv(self->sock, self->buf, RECV_BUF_AUTOTRUNC);

  if (size < 0) return box__ctor(NULL, 0);

  return box__ctor(self->buf->ptr, _M_cast(size_t, size));
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS__TCP__H
