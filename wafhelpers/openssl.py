SNIP_LIBSSL_TLS13_CHECK = """
#include <openssl/tls1.h>

#ifndef TLS1_3_VERSION
#error OpenSSL must have support for TLSv1.3
#endif

int main(void) {
    return 0;
}
"""


def check_libssl_tls13(ctx):
    ctx.check_cc(
      fragment=SNIP_LIBSSL_TLS13_CHECK,
      use="SSL CRYPTO",
      msg="Checking for OpenSSL with TLSv1.3 support",
    )


SNIP_OPENSSL_BAD_VERSION_CHECK = """
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_NUMBER == 0x1010101fL
#error OpenSSL version must not be 1.1.1a
#endif

int main(void) {
    return 0;
}
"""


def check_openssl_bad_version(ctx):
    ctx.check_cc(
      fragment=SNIP_OPENSSL_BAD_VERSION_CHECK,
      use="SSL CRYPTO",
      msg="Checking for OpenSSL != 1.1.1a",
    )
