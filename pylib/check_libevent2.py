from waflib.Logs import pprint
from tool import check_sanity

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


def check_libevent2_header(ctx):
	ctx.check(header_name="event2/event.h", includes=ctx.env.PLATFORM_INCLUDES, mandatory = False)
	ctx.check(header_name="event2/thread.h", includes=ctx.env.PLATFORM_INCLUDES, mandatory = False)
	ctx.check(feature="c cshlib", lib="event_core", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_CORE", mandatory = False)
	ctx.check(feature="c cshlib", lib="event_pthreads", libpath=ctx.env.PLATFORM_LIBPATH, uselib_store="LIBEVENT_PTHREADS", use="LIBEVENT_CORE", mandatory = False)

	if ctx.get_define("HAVE_EVENT2_THREAD_H") and ctx.get_define("HAVE_EVENT2_EVENT_H") and ctx.env.LIB_LIBEVENT_PTHREADS and ctx.env.LIB_LIBEVENT_CORE:
		ctx.env.EVENT2_HEADER = True

def check_libevent2_run(ctx):
	if ctx.env.ENABLE_CROSS:
		if ctx.env.EVENT2_HEADER: # XXX Remove when variant builds exist
			ctx.define("HAVE_LIBEVENT2", 1)
			ctx.env.LIBEVENT2_ENABLE = True
	return

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

	check_sanity(ctx, ctx.env.EVENT2_HEADER, "libevent2")

	if not ctx.get_define("HAVE_LIBEVENT2"):
		print("")
		pprint("RED", "Warning libevent2 does not work")
		pprint("RED", "This means ntpdig will not be built")
		pprint("RED", "While not necessary you will lose 'ntpdate' functionality.")
		print("")
	else:
		ctx.env.LIBEVENT2_ENABLE = True
		ctx.define("HAVE_LIBEVENT2", 1)
