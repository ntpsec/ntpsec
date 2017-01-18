
#ifndef sodium_utils_H
#define sodium_utils_H

#include <stddef.h>

#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SODIUM_C99
# if defined(__cplusplus) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
#  define SODIUM_C99(X)
# else
#  define SODIUM_C99(X) X
# endif
#endif

#ifdef __cplusplus
}
#endif

#endif
