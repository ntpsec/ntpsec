/*
 * Define malloc and friends.
 */
#ifndef NTP_MALLOC_H
#define NTP_MALLOC_H

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#else
# ifdef HAVE_MALLOC_H
#  include <malloc.h>
# endif
#endif

#endif	/* NTP_MALLOC_H */
