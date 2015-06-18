
#include <stdarg.h>
#ifndef   VA_START
#  define VA_START(a, f)  va_start(a, f)
#  define VA_END(a)       va_end(a)
#endif /* VA_START */
#define SNV_USING_STDARG_H

static int
snprintf(char *str, size_t n, char const *fmt, ...)
{
    va_list ap;
    int rval;

#ifdef VSPRINTF_CHARSTAR
    char *rp;
    VA_START(ap, fmt);
    rp = vsprintf(str, fmt, ap);
    VA_END(ap);
    rval = strlen(rp);

#else
    VA_START(ap, fmt);
    rval = vsprintf(str, fmt, ap);
    VA_END(ap);
#endif

    if (rval > n) {
        fprintf(stderr, "snprintf buffer overrun %d > %d\n", rval, (int)n);
        abort();
    }
    return rval;
}

static int
vsnprintf( char *str, size_t n, char const *fmt, va_list ap )
{
#ifdef VSPRINTF_CHARSTAR
    return (strlen(vsprintf(str, fmt, ap)));
#else
    return (vsprintf(str, fmt, ap));
#endif
}
