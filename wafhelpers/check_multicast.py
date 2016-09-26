import sys
from waflib.Logs import pprint

def check_multicast(ctx):
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
                define_name="MCAST",
                msg = "Checking for multicast capability",
                mandatory = False,
                comment = "IP multicast capability")

