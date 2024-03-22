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

#ifndef __MICRO_SOCKETS__SOCKADDR__H
#define __MICRO_SOCKETS__SOCKADDR__H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off

// Pin to the top of file, becuase it is used in the definitions of include
// macros like __MICRO_SOCKETS__IS_WINDOWS, DO NOT MOVE THIS INCLUDE !!!
#include "micro-sockets/_defs.h"

// clang-format on

#include <assert.h>
#include <string.h>

#if __MICRO_SOCKETS__IS_WINDOWS
// WINDOWS specific includes
#include <winsock.h>

#else
// UNIX/Linux specific includes
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/un.h>
#endif

#include "ccms/_macros.h"

typedef union sockaddr_union_t sockaddr_union_t;
union sockaddr_union_t {
  struct sockaddr sa;
  struct sockaddr_in in4;
  struct sockaddr_in6 in6;
  struct sockaddr_un un;
  struct sockaddr_storage storage;
};

typedef struct sockaddr_inet_t sockaddr_inet_t;
struct sockaddr_inet_t {
  sa_family_t family;
  socklen_t size;
  sockaddr_union_t addr;
};

__MICRO_SOCKETS__INLINE
sockaddr_inet_t sockaddr_inet__from(sa_family_t sa_family, const char* addr,
                                    uint16_t port) {
  assert(sa_family == AF_INET || sa_family == AF_INET6);
  sockaddr_inet_t self;

  memset(&self, 0, sizeof(sockaddr_inet_t));
  self.family = sa_family;

  if (sa_family == AF_INET) {
    self.size = sizeof(struct sockaddr_in);

    inet_pton(AF_INET, addr, _M_addr(self.addr.in4.sin_addr));
    self.addr.in4.sin_family = AF_INET;
    self.addr.in4.sin_port = htons(port);

  }

  else {
    self.size = sizeof(struct sockaddr_in6);

    inet_pton(AF_INET6, addr, _M_addr(self.addr.in6.sin6_addr));
    self.addr.in6.sin6_family = AF_INET6;
    self.addr.in6.sin6_port = htons(port);
  }

  return self;
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS__SOCKADDR__H