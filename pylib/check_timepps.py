TIMEPPS_FRAG = """
#include <sys/types.h>

#ifdef HAVE_TIMEPPS_H
# include <timepps.h>
#else
# ifdef HAVE_SYS_TIMEPPS_H
#  include <sys/timepps.h>
# endif
#endif

int main(void) {
	pps_handle_t pps_h;
	time_pps_destroya (pps_h);

    return 0;
}
"""


def check_timepps(ctx):
	ctx.check_cc(
		fragment	= TIMEPPS_FRAG,
		define_name = "HAVE_PPSAPI",
		features	= "c",
		msg         = "Checking if PPS PAI works",
		mandatory	= False
	)

