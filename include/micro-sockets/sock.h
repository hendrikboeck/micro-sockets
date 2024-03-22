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

#include "micro-sockets/buf.h"
#include "micro-sockets/sockaddr.h"

#if __MICRO_SOCKETS__IS_WINDOWS
typedef SOCKET sock_t;
#else
typedef int32_t sock_t;
#endif  // __MICRO_SOCKETS__IS_WINDOWS

#define RECV_BUF_AUTOTRUNC 1

__MICRO_SOCKETS__INLINE
ssize_t _sock__recv(sock_t fd, buf_t* buf) {
  ssize_t len = recv(fd, buf->ptr, buf->size, 0);
  if (len >= 0) buf->len = _M_cast(size_t, len);

  return len;
}

__MICRO_SOCKETS__INLINE
ssize_t _sock__send(sock_t fd, box_t data) {
  return send(fd, data.ptr, data.size, 0);
}

__MICRO_SOCKETS__INLINE
sock_t _sock__new(int32_t domain, int32_t proto) {
#if __MICRO_SOCKETS__IS_WINDOWS
  // Initialize Windows-specific Winsock
  WORD w_version_requested = MAKEWORD(1, 1);
  WSADATA wsa_data;

  // If WSAStartup fails, return NULL
  if (WSAStartup(w_version_requested, &wsa_data) != 0) return NULL;
#endif
  // Create the socket
  return socket(domain, SOCK_STREAM, proto);
}

__MICRO_SOCKETS__INLINE
ssize_t _sock__bind(sock_t fd, sockaddr_inet_t* sa) {
  // Bind the socket to the address
  return bind(fd, _M_addr(sa->addr.sa), sa->size);
}

__MICRO_SOCKETS__INLINE
int32_t _sock__close(sock_t fd) {
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
