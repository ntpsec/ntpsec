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


SNIP_OPENSSL_DUMP_VERSION = """
#include <stdio.h>
#include <openssl/opensslv.h>

int main(void) {
  printf("%s\\n", OPENSSL_VERSION_TEXT);
  return 0;
}
"""


def dump_openssl_version(ctx):
    _ = "XXX_LIBSSL_VERSION"
    ctx.start_msg("OpenSSL version")
    ctx.check_cc(
        fragment=SNIP_OPENSSL_DUMP_VERSION,
        execute=True,
        define_ret=True,
        define_name=_,
    )
    ctx.end_msg(ctx.get_define(_).split()[1])
    ctx.undefine(_)
