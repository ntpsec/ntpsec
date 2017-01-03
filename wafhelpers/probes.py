"""
This module exists to contain custom probe functions so they don't clutter
up the logic in the main configure.py.
"""


def probe_header_with_prerequisites(ctx, header, prerequisites, use=None):
        "Check that a header (with its prerequisites) compiles."
        src = ""
        for hdr in prerequisites + [header]:
            src += "#include <%s>\n" % hdr
        src += "int main(void) { return 0; }\n"
        have_name = "HAVE_%s" \
            % header.replace(".", "_").replace("/", "_").upper()
        ctx.check_cc(
            fragment=src,
            define_name=have_name,
            msg="Checking for header %s" % header,
            use=use or [],
            mandatory=False,
            comment="<%s> header" % header)
        return ctx.get_define(have_name)


def probe_function_with_prerequisites(ctx, function, prerequisites, use=None):
        "Check that a function (with its prerequisites) compiles."
        src = ""
        for hdr in prerequisites:
                src += "#include <%s>\n" % hdr
        src += """int main(void) {
        void *p = (void*)(%s);
        return (int)p;
}
""" % function
        have_name = "HAVE_%s" % function.upper()
        ctx.check_cc(
            fragment=src,
            define_name=have_name,
            msg="Checking for function %s" % function,
            use=use or [],
            mandatory=False,
            comment="Whether %s() exists" % function)
        return ctx.get_define(have_name)

# end
