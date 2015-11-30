from waflib.Logs import pprint
LIBEVENT2_FRAG = """
#include <event2/thread.h>
#include <event2/event.h>

int main(void) {
	struct event_config *   evcfg;

	evthread_use_pthreads();
	evcfg = event_config_new();
	return 0;
}
"""


def check_libevent2(ctx):
	check_libevent2.false = False

	def check(**kwargs):
		if not ctx.check_cc(**kwargs):
			check_libevent2.false = True


	check(header_name="event2/event.h", includes=ctx.env.PLATFORM_INCLUDES, mandatory = False)
	check(header_name="event2/thread.h", includes=ctx.env.PLATFORM_INCLUDES, mandatory = False)
	check(feature="c cshlib", lib="event_core", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_CORE", mandatory = False)
	check(feature="c cshlib", lib="event_pthreads", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_PTHREADS", use="LIBEVENT_CORE", mandatory = False)


	ctx.check(
		fragment	= LIBEVENT2_FRAG,
		define_name = "HAVE_LIBEVENT2",
		features	= "c",
		use			= "LIBEVENT, LIBEVENT_CORE, LIBEVENT_PTHREADS",
		msg         = "Checking if libevent2 works",
		includes        = ctx.env.PLATFORM_INCLUDES,
		export_includes = ctx.env.PLATFORM_INCLUDES,
		mandatory	= False
	)


	if check_libevent2.false:
		print("")
		pprint("RED", "Warning libevent2 does not work")
		pprint("RED", "This means ntpdig will not be built")
		pprint("RED", "While not nessicary you will lose 'ntpdate' functionality.")
		print("")
	else:
		ctx.env.LIBEVENT2_ENABLE=True
		ctx.define("HAVE_LIBEVENT2", 1)
