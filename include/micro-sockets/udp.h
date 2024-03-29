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

#ifndef __MICRO_SOCKETS_UDP_H
#define __MICRO_SOCKETS_UDP_H

#include <stdint.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off

#include "micro-sockets/_defs.h"

// clang-format on

#if __MICRO_SOCKETS__IS_WINDOWS
#include <io.h>
#include <winsock.h>

#else
#include <sys/socket.h>
#endif

#include "ccms/sized_memory.h"

#if __MICRO_SOCKETS__IS_WINDOWS
typedef SOCKET sock_t;
#else
typedef int32_t sock_t;
#endif

#define UDP_BUFTRUNC 1

__MICRO_SOCKETS__INLINE
ssize_t _udp_recv(sock_t fd, SizedMemory* buf, size_t flags) {
  int32_t buf_trunc = flags & UDP_BUFTRUNC;
  size_t buf_size = buf_trunc ? buf->size - 1 : buf->size;
  ssize_t len = recv(fd, buf->ptr, buf_size, 0);

  if (len >= 0 && buf_trunc) buf->ptr[len] = 0x0;
  return len;
}

#ifdef __cplusplus
}
#endif

// TODO: Implement UDP socket functions

#endif  // __MICRO_SOCKETS_UDP_H
