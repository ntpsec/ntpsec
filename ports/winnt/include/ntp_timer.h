#if !defined(__NTP_TIMER_H_)
#define __NTP_TIMER_H_

extern	void		timer_clr_stats			(void);

#if defined(SYS_WINNT)
extern	HANDLE	get_timer_handle	(void);
#endif

#endif
