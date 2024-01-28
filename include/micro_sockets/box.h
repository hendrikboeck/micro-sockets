#ifndef __MICRO_SOCKETS_BOX_H
#define __MICRO_SOCKETS_BOX_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t *ptr;
  size_t size;
} box_t;

extern inline box_t box_ctor(uint8_t *ptr, size_t size) {
  return (box_t){ptr, size};
}

#endif // __MICRO_SOCKETS_BOX_H