#if !defined(__NTP_TIMER_H_)
#define __NTP_TIMER_H_

extern	void		timer_clr_stats			P((void));

#if defined(SYS_WINNT)
extern	HANDLE	get_timer_handle	P((void));
#endif



#endif