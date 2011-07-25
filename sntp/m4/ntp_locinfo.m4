dnl ######################################################################
dnl Location information:
dnl - installation directory (*_DB for bin/, *_DL for libexec/, *_DS for sbin/)
dnl - man tag format (man or mdoc)
dnl - man section (1, 1m, 1M, 8)

AC_DEFUN([NTP_LOCINFO], [

AC_ARG_WITH(
    [locfile],
    [AS_HELP_STRING(
	[--with-locfile],
	[os-specific or "legacy"]
    )],
    [ans=$withval],
    [ans=]
)
# HMS: the following is wrong...
ntp_cv_locfile=$ans

AC_MSG_CHECKING([for installation directory, man sections, and man format])

( cd $ac_abs_confdir && scripts/genLocInfo -f "$ntp_cv_locfile" ) > genLocInfo.i 2>genLocInfo.err
. ./genLocInfo.i

case "$GENLOCINFO" in
 OK)
    AC_MSG_RESULT([found in $GENLOCINFOFILE])
    rm genLocInfo.err genLocInfo.i
    ;;
 *) AC_MSG_ERROR([Problem with genLocInfo!])
    ;;
esac

AC_SUBST(MANTAGFMT)
AC_SUBST(NTPDATE_DB)
AC_SUBST(NTPDATE_DL)
AC_SUBST(NTPDATE_DS)
AC_SUBST(NTPDATE_MS)
AC_SUBST(NTPDC_DB)
AC_SUBST(NTPDC_DL)
AC_SUBST(NTPDC_DS)
AC_SUBST(NTPDC_MS)
AC_SUBST(NTPDSIM_DB)
AC_SUBST(NTPDSIM_DL)
AC_SUBST(NTPDSIM_DS)
AC_SUBST(NTPDSIM_MS)
AC_SUBST(NTPD_DB)
AC_SUBST(NTPD_DL)
AC_SUBST(NTPD_DS)
AC_SUBST(NTPD_MS)
AC_SUBST(NTPQ_DB)
AC_SUBST(NTPQ_DL)
AC_SUBST(NTPQ_DS)
AC_SUBST(NTPQ_MS)
AC_SUBST(NTPSNMPD_DB)
AC_SUBST(NTPSNMPD_DL)
AC_SUBST(NTPSNMPD_DS)
AC_SUBST(NTPSNMPD_MS)
AC_SUBST(NTPTIME_DB)
AC_SUBST(NTPTIME_DL)
AC_SUBST(NTPTIME_DS)
AC_SUBST(NTPTIME_MS)
AC_SUBST(NTPTRACE_DB)
AC_SUBST(NTPTRACE_DL)
AC_SUBST(NTPTRACE_DS)
AC_SUBST(NTPTRACE_MS)
AC_SUBST(NTP_KEYGEN_DB)
AC_SUBST(NTP_KEYGEN_DL)
AC_SUBST(NTP_KEYGEN_DS)
AC_SUBST(NTP_KEYGEN_MS)
AC_SUBST(NTP_WAIT_DB)
AC_SUBST(NTP_WAIT_DL)
AC_SUBST(NTP_WAIT_DS)
AC_SUBST(NTP_WAIT_MS)
AC_SUBST(SNTP_DB)
AC_SUBST(SNTP_DL)
AC_SUBST(SNTP_DS)
AC_SUBST(SNTP_MS)
AC_SUBST(TICKADJ_DB)
AC_SUBST(TICKADJ_DL)
AC_SUBST(TICKADJ_DS)
AC_SUBST(TICKADJ_MS)
AC_SUBST(TIMETRIM_DB)
AC_SUBST(TIMETRIM_DL)
AC_SUBST(TIMETRIM_DS)
AC_SUBST(TIMETRIM_MS)

])dnl
dnl ======================================================================
