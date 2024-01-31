#ifndef __MICRO_SOCKETS__SOCK_ADDR_ANY__H
#define __MICRO_SOCKETS__SOCK_ADDR_ANY__H

#ifdef __cplusplus
extern "C" {
#endif

#include <assert.h>
#include <string.h>

#ifdef _WIN32 // -- WINDOWS
#include <winsock.h>

#else // -- UNIX/Linux
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#include "ccms/_macros.h"

typedef union _SockAddrAny _SockAddrAny;

union _SockAddrAny {
  struct sockaddr_in inet;
  struct sockaddr_in6 inet6;
};

static inline _SockAddrAny
_sock_addr_any__from_sockaddr_in(struct sockaddr_in sa) {
  return (_SockAddrAny){.inet = sa};
}

static inline _SockAddrAny
_sock_addr_any__from_sockaddr_in6(struct sockaddr_in6 sa) {
  return (_SockAddrAny){.inet6 = sa};
}

static inline struct sockaddr_in
sockaddr_in__from_addr_port(const char *addr, const uint16_t port) {
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET, addr, &(sa.sin_addr));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);

  return sa;
}

static inline struct sockaddr_in6
sockaddr_in6__from_addr_port(const char *addr, const uint16_t port) {
  struct sockaddr_in6 sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET6, addr, &(sa.sin6_addr));
  sa.sin6_family = AF_INET6;
  sa.sin6_port = htons(port);

  return sa;
}

static inline _SockAddrAny _sock_addr_any__ctor(const sa_family_t proto,
                                                const char *addr,
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

#endif // __MICRO_SOCKETS_ADDR_H
