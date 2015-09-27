from waflib.Configure import conf

TYPE_FRAG = """
#include <stdint.h>
#include <sys/types.h>
#include <%s>
int main () {
	struct %s x;
	if (sizeof (x.%s))
		return 0;
	return 0;
}
"""

@conf
def check_structfield(ctx, fld, type, hdr, mandatory=False):
	name = "STRUCT_%s_HAS_%s" % (type.upper(), fld.upper())

	ctx.check_cc(
		fragment    = TYPE_FRAG % (hdr, type, fld),
		define_name = name,
		execute     = False,
		msg         = "Checking for %s in struct %s" % (fld, type),
		mandatory   = mandatory
	)
