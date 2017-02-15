/*
 * Definitions for the PPS driver and its friends
 */

struct refclock_ppsctl {
	pps_handle_t handle;
	pps_params_t pps_params;
	struct timespec ts;
	unsigned long sequence;
};

typedef enum {
  PPS_OK,
  PPS_SETUP,  /* PPS not setup */
  PPS_KERNEL, /* PPS error from kernel */
  PPS_NREADY  /* PPS not ready */
} pps_status;

extern	bool	refclock_ppsapi(int, struct refclock_ppsctl *);
extern	bool	refclock_params(int, struct refclock_ppsctl *);
extern pps_status refclock_catcher(struct peer *, struct refclock_ppsctl *, int);
