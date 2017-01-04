from waflib.Configure import conf

TYPE_FRAG = """
int main(void) {
        if (sizeof (%s))
                return 0;
        return 0;
}
"""


@conf
def check_type(ctx, typename, headers=[], mandatory=False):
    name = "HAVE_%s" % typename.upper().replace(" ", "_")
    src = ""
    for hdr in headers:
        src += "#include <%s>\n" % hdr
    ctx.check_cc(
        fragment=src + TYPE_FRAG % (typename),
        define_name=name,
        execute=False,
        msg="Checking for type %s" % (typename),
        mandatory=mandatory,
        comment="Whether type '%s' exists." % typename
    )
