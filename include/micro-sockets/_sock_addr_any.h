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

#ifndef __MICRO_SOCKETS__SOCK_ADDR_ANY__H
#define __MICRO_SOCKETS__SOCK_ADDR_ANY__H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off
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
#endif

#include "ccms/_macros.h"

/**
 * @brief Typedef for the _SockAddrAny union.
 */
typedef union _SockAddrAny _SockAddrAny;

/**
 * @brief Union to hold either a sockaddr_in or sockaddr_in6 structure.
 *
 * @var struct sockaddr_in _SockAddrAny::inet
 * IPv4 socket address
 *
 * @var struct sockaddr_in6 _SockAddrAny::inet6
 * IPv6 socket address
 */
union _SockAddrAny {
  struct sockaddr_in inet;
  struct sockaddr_in6 inet6;
};

/**
 * @brief Converts a sockaddr_in structure to a _SockAddrAny union.
 *
 * @param sa The sockaddr_in structure to convert.
 *
 * @return The converted _SockAddrAny union.
 */
__MICRO_SOCKETS__INLINE
_SockAddrAny _sock_addr_any__from_sockaddr_in(struct sockaddr_in sa) {
  return (_SockAddrAny){.inet = sa};
}

/**
 * @brief Converts a sockaddr_in6 structure to a _SockAddrAny union.
 *
 * @param sa The sockaddr_in6 structure to convert.
 *
 * @return The converted _SockAddrAny union.
 */
__MICRO_SOCKETS__INLINE
_SockAddrAny _sock_addr_any__from_sockaddr_in6(struct sockaddr_in6 sa) {
  return (_SockAddrAny){.inet6 = sa};
}

/**
 * @brief Creates a sockaddr_in structure from an address and port.
 *
 * @param addr The address to use.
 * @param port The port to use.
 *
 * @return The created sockaddr_in structure.
 */
__MICRO_SOCKETS__INLINE
struct sockaddr_in sockaddr_in__from_addr_port(const char* addr,
                                               const uint16_t port) {
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET, addr, &(sa.sin_addr));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);

  return sa;
}

/**
 * @brief Creates a sockaddr_in6 structure from an address and port.
 *
 * @param addr The address to use.
 * @param port The port to use.
 *
 * @return The created sockaddr_in6 structure.
 */
__MICRO_SOCKETS__INLINE
struct sockaddr_in6 sockaddr_in6__from_addr_port(const char* addr,
                                                 const uint16_t port) {
  struct sockaddr_in6 sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET6, addr, &(sa.sin6_addr));
  sa.sin6_family = AF_INET6;
  sa.sin6_port = htons(port);

  return sa;
}

/**
 * @brief Constructs a _SockAddrAny union from a protocol, address, and port.
 *
 * @param proto The protocol to use (AF_INET or AF_INET6).
 * @param addr The address to use.
 * @param port The port to use.
 *
 * @return The constructed _SockAddrAny union.
 */
__MICRO_SOCKETS__INLINE
_SockAddrAny _sock_addr_any__ctor(const sa_family_t proto, const char* addr,
                                  const uint16_t port) {
  assert(proto == AF_INET || proto == AF_INET6);

  if (proto == AF_INET)
    return _sock_addr_any__from_sockaddr_in(
        sockaddr_in__from_addr_port(addr, port));

  return _sock_addr_any__from_sockaddr_in6(
      sockaddr_in6__from_addr_port(addr, port));
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS_ADDR_H