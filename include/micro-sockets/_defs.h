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

#ifndef __MICRO_SOCKETS___DEFS__H
#define __MICRO_SOCKETS___DEFS__H

#if defined(_WIN32) || defined(_WIN64)
#define __MICRO_SOCKETS__IS_WINDOWS 1
#else
#define __MICRO_SOCKETS__IS_WINDOWS 0
#endif

#ifdef __MICRO_SOCKETS__EXTERN
#define __MICRO_SOCKETS__INLINE extern inline
#else
#define __MICRO_SOCKETS__INLINE static inline
#endif

#endif  // __MICRO_SOCKETS___DEFS__H