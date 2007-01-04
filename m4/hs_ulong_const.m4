AC_DEFUN([hs_ULONG_CONST],
[ AH_TEMPLATE(ULONG_CONST, [How do we create unsigned long constants?])
 AC_DEFINE([ULONG_CONST(a)], [a ## UL])])
