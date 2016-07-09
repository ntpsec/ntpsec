from tool import check_sanity
import sys

PCAP_FRAG = """
# include <sys/capability.h>

int main(void) {
    cap_t caps;
    char *txt_caps;

    caps = cap_from_text("cap_chown+e");
    cap_free(caps);

    return 0;
}
"""

def check_cap_header(ctx):
	ctx.check_cc(header_name="sys/capability.h", mandatory=False)
	ctx.check_cc(lib="cap", comment="Capability library", mandatory=False)

	if ctx.get_define("HAVE_SYS_CAPABILITY_H") and ctx.get_define("HAVE_SYS_PRCTL_H") and ctx.env.LIB_LIBCAP:
		ctx.env.LIBCAP_HEADER = True


def check_cap_run(ctx):
	if ctx.env.ENABLE_CROSS: # XXX Remove when variant builds exist
		if ctx.env.LIBCAP_HEADER:
			ctx.define("HAVE_CAPABILITY", 1, comment="Capability support")
		return

	ctx.check_cc(
		fragment	= PCAP_FRAG,
		define_name	= "HAVE_CAPABILITY",
		features	= "c",
		use		= "CAP",
		msg		= "Checking if libcap works",
		mandatory	= sys.platform.startswith("linux"),
		comment		= "Capability support"
	)

	check_sanity(ctx, ctx.env.LIBCAP_HEADER, "libcap")


	if ctx.get_define("HAVE_CAPABILITY"):
		ctx.define("HAVE_LINUX_CAPABILITY", 1, comment="Capability support") # XXX: why two?
