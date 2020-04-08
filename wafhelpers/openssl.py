SNIP_OPENSSL_VERSION_CHECK = """
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER <= 0x1010101fL
#error OpenSSL must be at least 1.1.1b
#error  1.1.1 needed for TLSv1.3
#error  1.1.1a has a fatal bug
#endif

int main(void) {
    return 0;
}
"""


def check_SSL_version(ctx):
    ctx.check_cc(
      comment="OpenSSL support",
      fragment=SNIP_OPENSSL_VERSION_CHECK,
      use="SSL",
      msg="Checking for OpenSSL > 1.1.1a",
     )
