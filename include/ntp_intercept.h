/*
 * ntp_intercept.h - interecept/replay support for environment calls
 */

#if __STDC_VERSION__ < 199901L
# if __GNUC__ >= 2
#  define __func__ __FUNCTION__
# else
#  define __func__ "<unknown>"
# endif
#endif

/* Macro for declaring function with printf-like arguments. */
# if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#define PRINTF_FUNC(format_index, arg_index) \
    __attribute__((__format__(__printf__, format_index, arg_index)))
# else
#define PRINTF_FUNC(format_index, arg_indx)
#endif

void intercept_argparse(int *, char ***);
PRINTF_FUNC(1, 2) void intercept_log(const char *, ...);
void intercept_get_systime(const char *, l_fp *);
long intercept_ntp_random(const char *);
