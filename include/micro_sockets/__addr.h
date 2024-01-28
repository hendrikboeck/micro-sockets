#ifndef __MICRO_SOCKETS_ADDR_H
#define __MICRO_SOCKETS_ADDR_H

#include <assert.h>
#include <string.h>

#ifdef _WIN32 // -- WINDOWS
#include <winsock.h>

#else // -- UNIX/Linux
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

typedef union {
  struct sockaddr_in inet;
  struct sockaddr_in6 inet6;
} __sockaddr_any_t;

extern inline __sockaddr_any_t __sockaddr_any_from_inet(struct sockaddr_in sa) {
  return (__sockaddr_any_t){.inet = sa};
}

extern inline __sockaddr_any_t
__sockaddr_any_from_inet6(struct sockaddr_in6 sa) {
  return (__sockaddr_any_t){.inet6 = sa};
}

extern inline struct sockaddr_in __sockaddr_inet_ctor(const char *addr,
                                                      uint16_t port) {
  struct sockaddr_in sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET, addr, &(sa.sin_addr));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(port);

  return sa;
}

extern inline struct sockaddr_in6 __sockaddr_inet6_ctor(const char *addr,
                                                        uint16_t port) {
  struct sockaddr_in6 sa;

  memset(&sa, 0, sizeof(sa));
  inet_pton(AF_INET6, addr, &(sa.sin6_addr));
  sa.sin6_family = AF_INET6;
  sa.sin6_port = htons(port);

  return sa;
}

extern inline __sockaddr_any_t
__sockaddr_any_ctor(sa_family_t proto, const char *addr, uint16_t port) {
  assert(proto == AF_INET || proto == AF_INET6);

  if (proto == AF_INET)
    return __sockaddr_any_from_inet(__sockaddr_inet_ctor(addr, port));

  return __sockaddr_any_from_inet6(__sockaddr_inet6_ctor(addr, port));
}

#endif // __MICRO_SOCKETS_ADDR_H