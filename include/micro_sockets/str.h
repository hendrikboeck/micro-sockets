#ifndef __MICRO_SOCKETS_STR_H
#define __MICRO_SOCKETS_STR_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "micro_sockets/__macros.h"
#include "micro_sockets/box.h"

typedef struct {
  char *ptr;
  size_t size;
} str_t;

extern inline str_t *str_new(const size_t size) {
  str_t *self = __c_cast(str_t *, malloc(sizeof(str_t) + size * sizeof(char)));

  self->ptr = __c_cast(char *, __c_cast(uint8_t *, self) + sizeof(str_t));
  self->size = size;

  return self;
}

extern inline str_t *str_from(const char *s) {
  size_t len = strlen(s);
  str_t *self = str_new(len + 1);
  memcpy(self->ptr, s, len + 1);
  return self;
}

extern inline void str_free(str_t *self) { free(self); }

extern inline void str_reset(str_t *self) { self->ptr[0] = 0; }

extern inline size_t str_len(const str_t *self) { return strlen(self->ptr); }

extern inline str_t *str_clone(const str_t *self) {
  str_t *other =
      __c_cast(str_t *, malloc(sizeof(str_t) + self->size * sizeof(char)));

  memcpy(other, self, sizeof(str_t) + self->size * sizeof(char));
  other->ptr = __c_cast(char *, __c_cast(uint8_t *, other) + sizeof(str_t));

  return other;
}

typedef struct {
  char *ptr;
  size_t len;
} strview_t;

extern inline strview_t strview_ctor(char *ptr, size_t len) {
  return (strview_t){ptr, len};
}

extern inline strview_t str_as_ref(const str_t *self) {
  return strview_ctor(self->ptr, str_len(self));
}

extern inline str_t *strview_to_str(const strview_t self) {
  str_t *new_str = str_new(self.len + 1);

  memcpy(new_str->ptr, self.ptr, self.len);
  new_str->ptr[self.len] = 0;

  return new_str;
}

extern inline strview_t strview_from_boxview_ascii(const box_t box) {
  return strview_ctor(__c_cast(char *, box.ptr), box.size);
}

#endif // __MICRO_SOCKETS_STR_H