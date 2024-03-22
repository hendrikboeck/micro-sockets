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

#ifndef __MICRO_SOCKETS__BUF__H
#define __MICRO_SOCKETS__BUF__H

#ifdef __cplusplus
extern "C" {
#endif

// clang-format off

// Pin to the top of file, becuase it is used in the definitions of include
// macros like __MICRO_SOCKETS__IS_WINDOWS, DO NOT MOVE THIS INCLUDE !!!
#include "micro-sockets/_defs.h"

// clang-format on

#include <stddef.h>
#include <stdint.h>

#include "ccms/_macros.h"

typedef struct buf_t buf_t;

struct buf_t {
  uint8_t* ptr;
  size_t len;
  size_t size;
};

__MICRO_SOCKETS__INLINE
buf_t* buf__new(size_t size) {
  buf_t* buf = _M_cast(buf_t*, _M_alloc(sizeof(buf_t) + size + 1));

  if (buf == NULL) {
    return NULL;
  }

  buf->ptr = _M_cast(uint8_t*, buf) + sizeof(buf_t);
  buf->len = 0;
  buf->size = size;

  return buf;
}

__MICRO_SOCKETS__INLINE
void buf__free(buf_t* buf) {
  _M_free(buf);
}

__MICRO_SOCKETS__INLINE
void buf__clear(buf_t* buf) {
  buf->size = 0;
}

__MICRO_SOCKETS__INLINE
char* buf__str(buf_t* buf) {
  buf->ptr[buf->len] = 0x0;
  return _M_cast(char*, buf->ptr);
}

#ifdef __cplusplus
}
#endif

#endif  // __MICRO_SOCKETS__BUF__H
