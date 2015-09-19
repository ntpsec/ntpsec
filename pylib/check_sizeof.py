from waflib.Configure import conf

SIZE_FRAG = """
%s
#include <stdio.h>
int main () {
	printf("%%lu", sizeof(%s));
	return 0;
}
"""

@conf
def check_sizeof(ctx, header, sizeof, mandatory=True):
	sizeof_ns = sizeof.replace(" ", "_")
	name = "SIZEOF_%s" % sizeof_ns.upper()

	header_snippet = ""
	if header:
		ctx.start_msg("Checking sizeof %s (%s)" % (sizeof, header))
		header_snippet = "#include <%s>" % header
	else:
		ctx.start_msg("Checking sizeof %s" % (sizeof))

	ctx.check_cc(
		fragment	= SIZE_FRAG % (header_snippet, sizeof),
		define_name = name,
		execute     = True,
		define_ret  = True,
		quote		= False,
		mandatory	= mandatory,
	)
	ctx.end_msg(ctx.get_define(name))
