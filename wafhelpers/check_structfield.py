from waflib.Configure import conf

TYPE_FRAG = """
#include <stdint.h>
#include <sys/types.h>
%s
int main(void) {
        struct %s x;
        if (sizeof (x.%s))
                return 0;
        return 0;
}
"""


@conf
def check_structfield(ctx, fld, type, hdrs, mandatory=False):
    name = "STRUCT_%s_HAS_%s" % (type.upper(), fld.upper().replace('.', '_'))
    src = ""
    for hdr in hdrs:
        src += "#include <%s>\n" % hdr
    ctx.check_cc(
        fragment=TYPE_FRAG % (src, type, fld),
        define_name=name,
        execute=False,
        msg="Checking for %s in struct %s" % (fld, type),
        mandatory=mandatory,
        comment="Whether struct '%s' has field '%s'" % (fld, type)
    )
