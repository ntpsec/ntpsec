/*
 * Configuration file name
 */
#ifndef CONFIG_FILE
# ifndef SYS_WINNT
#  define	CONFIG_FILE "/etc/ntp.conf"
# else /* SYS_WINNT */
#  define	CONFIG_FILE	"%windir%\\system32\\drivers\\etc\\ntp.conf"
#  define	ALT_CONFIG_FILE "%windir%\\ntp.conf"
# endif /* SYS_WINNT */
#endif /* not CONFIG_FILE */
