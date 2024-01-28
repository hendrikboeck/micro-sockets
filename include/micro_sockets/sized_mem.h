#ifndef __MICRO_SOCKETS_SIZED_MEM_H
#define __MICRO_SOCKETS_SIZED_MEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "micro_sockets/__macros.h"

typedef struct {
  uint8_t *ptr;
  size_t size;
} sized_mem_t;

extern inline sized_mem_t *sized_mem_new(size_t size) {
  sized_mem_t *self = __c_cast(
      sized_mem_t *, malloc(sizeof(sized_mem_t) + sizeof(uint8_t) * size));

  self->ptr = __c_cast(uint8_t *, self) + sizeof(sized_mem_t);
  self->size = size;

  return self;
}

extern inline void sized_mem_free(sized_mem_t *self) { free(self); }

extern inline sized_mem_t *sized_mem_clone(const sized_mem_t *self) {
  sized_mem_t *other = sized_mem_new(self->size);
  memcpy(other->ptr, self->ptr, self->size);
  return other;
}

extern inline sized_mem_t sized_memview_ctor(uint8_t *ptr, const size_t size) {
  return (sized_mem_t){ptr, size};
}

#endif // __MICRO_SOCKETS_SIZED_MEM_H