AC_DEFUN(hs_ULONG_CONST,
[ AC_EGREP_CPP(Circus,
 [#define ACAT(a,b)a ## b
ACAT(Cir,cus)
], AC_DEFINE([ULONG_CONST(a)], [a ## UL]),
    AC_EGREP_CPP(Reiser,
[#define RCAT(a,b)a/**/b
RCAT(Rei,ser)
], AC_DEFINE([ULONG_CONST(a)], [a/**/U]),
    AC_MSG_ERROR([How do we create an unsigned long constant?])))])
