#ifndef NTP_TIMER_H
#define NTP_TIMER_H

extern	void	timer_clr_stats(void);

#ifdef SYS_WINNT
extern	HANDLE	get_timer_handle(void);
#endif

#endif	/* NTP_TIMER_H */
