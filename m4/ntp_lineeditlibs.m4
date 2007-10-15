AC_DEFUN([NTP_LINEEDITLIBS], [
  AC_CACHE_VAL([ntp_cv_lib_lineedit], [
    # Ralf Wildenhues: either unset ... or cache EDITLINE_LIBS
    unset ntp_cv_lib_lineedit
    ORIG_LIBS="$LIBS"
    AC_ARG_WITH([lineeditlibs],
        [AC_HELP_STRING([--with-lineeditlibs], [edit,editline,readline])],
        [use_lineeditlibs="$withval"],
	[use_lineeditlibs="edit,editline"])

    for lineedit_lib in `echo $use_lineeditlibs | sed -e 's/,/ /'`; do
      for term_lib in "" termcap curses ncurses; do
        case "$term_lib" in
         '') TRY_LIB="-l$lineedit_lib" ;;
         *)  TRY_LIB="-l$lineedit_lib -l$term_lib" ;;
        esac
        LIBS="$ORIG_LIBS $TRY_LIB"
	AC_MSG_CHECKING([for readline() with $TRY_LIB])
        AC_TRY_LINK_FUNC([readline], [ntp_cv_lib_lineedit="$TRY_LIB"])
        case "$ntp_cv_lib_lineedit" in
	 '')
	    AC_MSG_RESULT([no])
	    ;;
	 *) # Use readline()
	    AC_MSG_RESULT([yes])
	    break
	    ;;
        esac
	AC_MSG_CHECKING([for el_gets() with $TRY_LIB])
        AC_TRY_LINK_FUNC([el_gets], [ntp_cv_lib_lineedit="$TRY_LIB"])
        case "$ntp_cv_lib_lineedit" in
	 '')
	    AC_MSG_RESULT([no])
	    ;;
	 *) # Use el_gets()
	    AC_MSG_RESULT([yes])
	    break
	    ;;
        esac
      done
      case "$ntp_cv_lib_lineedit" in
       '') ;;
       *)  break ;;
      esac
    done
    LIBS="$ORIG_LIBS"
    case "$ntp_cv_lib_lineedit" in
     '')
	ntp_cv_lib_lineedit="no"
	;;
     *) EDITLINE_LIBS="$ntp_cv_lib_lineedit"
	AC_SUBST(EDITLINE_LIBS)
	;;
    esac
  ])

  case "$ntp_cv_lib_lineedit" in
   no) ;;
   -ledit)
	AC_DEFINE(HAVE_LIBEDIT, 1,
              [Define if you have libedit])
	# we want to also check for readline.h
	AC_CHECK_HEADERS(histedit.h)
	;;
   -leditline)
	AC_MSG_WARN([editline is not yet supported])
	;;
   *)
	AC_DEFINE(HAVE_LIBREADLINE, 1,
              [Define if you have a readline compatible library])
	AC_CHECK_HEADERS(readline.h readline/readline.h)
	AC_CACHE_CHECK([whether readline supports history],
                   ntp_cv_lib_lineedit_history, [
	    ntp_cv_lib_lineedit_history="no"
	    AC_TRY_LINK_FUNC(add_history, ntp_cv_lib_lineedit_history="yes")
	])
	case "$ntp_cv_lib_lineedit_history" in
	 yes)
		AC_DEFINE(HAVE_READLINE_HISTORY, 1,
                    [Define if your readline library has \`add_history'])
		AC_CHECK_HEADERS(history.h readline/history.h)
		;;
    	esac
	;;
  esac
])dnl
