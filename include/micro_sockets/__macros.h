#ifndef __MICRO_SOCKETS__MACROS_H
#define __MICRO_SOCKETS__MACROS_H

#define __c_cast(T, expr) ((T)(expr))

#ifndef __MICRO_SOCKETS_NO_SIZE_MACROS
#define KiB(n) ((n)*1024)
#define MiB(n) (KiB(n) * 1024)
#define GiB(n) (MiB(n) * 1024)

#define KB(n) ((n)*1000)
#define MB(n) (KB(n) * 1000)
#define GB(n) (MB(n) * 1000)
#endif

#endif // __MICRO_SOCKETS__MACROS_H