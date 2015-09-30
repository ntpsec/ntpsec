"""
This module exists to contin custom probe functions so they don't clutter
up the logic in the main configure.py.
"""

def probe_header_with_prerequisites(ctx, header, prerequisites):
	"Check that a header (with its prerequisites) compiles."
	src = ""
        for hdr in prerequisites + [header]:
        	src += "#include <%s>\n" % hdr
        src += "int main() { return 0; }\n"
	ctx.check_cc(
		fragment=src,
		define_name="HAVE_%s" % header.replace(".","_").replace("/","_").upper(),
		msg = "Checking for %s" % header,
		mandatory = False)

def probe_multicast(ctx, symbol, legend):
	"Probe for IP multicast capability."
	ctx.check_cc(
		fragment="""
#include <netinet/in.h>
int main() {
	struct ip_mreq ipmr;
	ipmr.imr_interface.s_addr = 0;
	return 0;
}
""",
		define_name=symbol,
		msg = legend,
		mandatory = False)
