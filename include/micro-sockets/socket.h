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

#ifndef __MICRO_SOCKETS__SOCKET__H
#define __MICRO_SOCKETS__SOCKET__H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
#include "micro-sockets/_defs.h"
// clang-format on

#if __MICRO_SOCKETS__IS_WINDOWS
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <stdint.h>

#include "ccms/sized_memory.h"
#include "micro-sockets/sock_addr_any.h"

#if __MICRO_SOCKETS__IS_WINDOWS
typedef SOCKET SockFd;
#else
typedef int32_t SockFd;
#endif  // __MICRO_SOCKETS__IS_WINDOWS

#define RECV_BUF_AUTOTRUNC 1

__MICRO_SOCKETS__INLINE
ssize_t _sock__recv(SockFd fd, SizedMemory* buf, size_t flags) {
  int32_t buf_trunc = flags & RECV_BUF_AUTOTRUNC;
  size_t buf_size = buf_trunc ? buf->size - 1 : buf->size;
  ssize_t len = recv(fd, buf->ptr, buf_size, 0);

  if (len >= 0 && buf_trunc) buf->ptr[len] = 0x0;
  return len;
}

__MICRO_SOCKETS__INLINE
ssize_t _sock__send(SockFd fd, Box data) {
  return send(fd, data.ptr, data.size, 0);
}

__MICRO_SOCKETS__INLINE
SockFd _sock__new(int32_t ipv, int32_t proto) {
#if __MICRO_SOCKETS__IS_WINDOWS
  // Initialize Windows-specific Winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  // If WSAStartup fails, return NULL
  if (WSAStartup(w_version_requested, &wsa_data) != 0) return NULL;
#endif

  SockFd sock;

  // Create the socket
  return socket(ipv, SOCK_STREAM, proto);
}

__MICRO_SOCKETS__INLINE
ssize_t _sock__bind(SockFd fd, SockAddrAny* sa, int32_t ipv) {
  size_t sa_size;
  struct sockaddr* sa_ref;

  // If the protocol is IPv4
  if (ipv == AF_INET) {
    sa_size = sizeof(struct sockaddr_in);
    sa_ref = _M_cast(struct sockaddr*, _M_addr(sa->inet));
  }
  // If the protocol is IPv6
  else {
    sa_size = sizeof(struct sockaddr_in6);
    sa_ref = _M_cast(struct sockaddr*, _M_addr(sa->inet6));
  }

  // Bind the socket to the address
  return bind(fd, sa_ref, sa_size);
}

__MICRO_SOCKETS__INLINE
int32_t _sock__close(SockFd fd) {
#if __MICRO_SOCKETS__IS_WINDOWS
  return closesocket(fd);
#else
  return close(fd);
#endif
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS__SOCKET__H
