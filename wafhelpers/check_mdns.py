MDNS_FRAG = """
# include <dns_sd.h>

int main(void) {
    DNSServiceRef mdns;
    DNSServiceRefDeallocate(mdns);
    return 0;
}
"""


def check_mdns_header(ctx):
    ctx.check_cc(lib="dns_sd", libpath=ctx.env.PLATFORM_LIBPATH,
                 mandatory=False)
    ctx.check_cc(header_name="dns_sd.h", includes=ctx.env.PLATFORM_INCLUDES,
                 uselib_store="DNS_SD_INCLUDES", mandatory=False)

    if ctx.get_define("HAVE_DNS_SD_H") and ctx.env.LIB_LIBDNS_SD:
        ctx.env.DNS_SD_HEADER = True


def check_mdns_run(ctx):
    if ctx.env.ENABLE_CROSS:  # XXX Remove when variant builds exist
        if ctx.env.DNS_SD_HEADER:
            ctx.define("HAVE_MDNS", 1, comment="multicast dns support")
        return

    ctx.check_cc(
        fragment=MDNS_FRAG,
        define_name="HAVE_MDNS",
        features="c",
        includes=ctx.env.PLATFORM_INCLUDES,
        libpath=ctx.env.PLATFORM_LIBPATH,
        export_includes=ctx.env.PLATFORM_INCLUDES,
        msg="Checking if mDNSResponder works",
        name="MDNS_INCLUDES",
        mandatory=False,
        comment="Multicast DNS support"
    )

    if ctx.get_define("HAVE_MDNS"):
        ctx.define("ENABLE_MDNS_REGISTRATION", 1,
                   comment="Multicast DNS support")
