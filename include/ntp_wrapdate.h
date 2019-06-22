/* calendar / date helpers for GPS devices */

/*
 * NMEA gps week/time information
 * This record contains the number of weeks since 1980-01-06 modulo
 * 1024, the seconds elapsed since start of the week, and the number of
 * leap seconds that are the difference between GPS and UTC time scale.
 */
typedef struct {
	uint32_t 	wt_time;	/* seconds since weekstart */
	unsigned short	wt_week;	/* week number */
	short		wt_leap;	/* leap seconds */
} gps_weektm;


void	wrapdate_init	(void);
bool	unfold_day	(struct calendar * jd, uint32_t rec_ui);
bool	unfold_century	(struct calendar * jd, uint32_t rec_ui);
bool	gpsfix_century	(struct calendar * jd, const gps_weektm * wd,
				 unsigned short * ccentury);
l_fp    eval_gps_time	(const char *, const struct calendar * gpst,
			 const struct timespec * gpso, bool trustl, short *epoch_wrap, const l_fp * xrecv);
// end
