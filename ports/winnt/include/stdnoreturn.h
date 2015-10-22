/*
 * A substitute for ISO C11 <stdnoreturn.h>
 *
 * Written by Paul Eggert.
 */
#ifndef noreturn

/* ISO C11 <stdnoreturn.h> for platforms that lack it.

   References:
   ISO C11 (latest free draft
   <http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf>)
   section 7.23 */

/* The definition of _Noreturn is copied here.  */
#if !defined _Noreturn && __STDC_VERSION__ < 201112
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
      || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif

#if 1200 <= _MSC_VER
/* Standard include files on this platform contain declarations like
   "__declspec (noreturn) void abort (void);".  "#define noreturn
   _Noreturn" would cause this declaration to be rewritten to the
   invalid "__declspec (__declspec (noreturn)) void abort (void);".
   Instead, define noreturn to empty, so that such declarations are
   rewritten to "__declspec () void abort (void);", which is
   equivalent to "void abort (void);"; this gives up on noreturn's
   advice to the compiler but at least it is valid code.  */
# define noreturn /*empty*/
#else
# define noreturn _Noreturn
#endif

/* Did he ever return?
   No he never returned
   And his fate is still unlearn'd ...
     -- Steiner J, Hawes BL.  M.T.A. (1949)  */

#endif /* noreturn */
