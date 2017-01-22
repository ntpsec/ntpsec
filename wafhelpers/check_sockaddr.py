SA_LEN_FRAG = """
#include <sys/types.h>
#include <sys/socket.h>
int main(void) {
  extern struct sockaddr *ps;
  return ps->sa_len;
}
"""


def check_sockaddr(ctx):
    ctx.check_cc(
        fragment=SA_LEN_FRAG,
        define_name="ISC_PLATFORM_HAVESALEN",
        features="c",
        msg="Checking for sockaddr->sa_len",
        mandatory=False,
        comment="Whether sockaddr.sa_len exists"
    )
