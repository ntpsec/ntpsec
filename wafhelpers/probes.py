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
            comment="<%s> header" % header,
            define_name=have_name,
            fragment=src,
            includes=ctx.env.PLATFORM_INCLUDES,
            mandatory=False,
            msg="Checking for header %s" % header,
            use=use or [],
        )
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
            comment="Whether %s() exists" % function,
            define_name=have_name,
            fragment=src,
            includes=ctx.env.PLATFORM_INCLUDES,
            mandatory=False,
            msg="Checking for function %s" % function,
            use=use or [],
        )
        return ctx.get_define(have_name)

# end
