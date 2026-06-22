# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

"""Report on OpenSSL version and features for OpenSSL."""

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
    """Report if OpenSSL supports TLS 1.3 for ./waf configure."""
    ctx.check_cc(
      fragment=SNIP_LIBSSL_TLS13_CHECK,
      use="SSL CRYPTO",
      msg="Checking for OpenSSL with TLSv1.3 support",
    )


SNIP_OPENSSL_VERSION_CHECK = """
#include <openssl/opensslv.h>

/* LibreSSL reports OPENSSL_VERSION_NUMBER as 0x2000000fL but is still a
 * supported platform, so exempt it from the OpenSSL 3.0 floor. */
#if !defined(LIBRESSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER < 0x30000000L
#error OpenSSL must be version 3.0.0 or newer
#endif

int main(void) {
    return 0;
}
"""


def check_openssl_version(ctx):
    """Require OpenSSL >= 3.0 (LibreSSL exempt) for ./waf configure."""
    ctx.check_cc(
      fragment=SNIP_OPENSSL_VERSION_CHECK,
      use="SSL CRYPTO",
      msg="Checking for OpenSSL >= 3.0 (or LibreSSL)",
    )


SNIP_OPENSSL_DUMP_VERSION = """
#include <stdio.h>
#ifdef HAVE_OPENSSL_OPENSSLV_H
#include <openssl/opensslv.h>
#else
#define OPENSSL_VERSION_TEXT "something not_OpenSSL_or_LibreSSL something"
#endif // HAVE_OPENSSL_OPENSSLV_H

int main(void) {
  printf("%s\\n", OPENSSL_VERSION_TEXT);
  return 0;
}
"""


def dump_openssl_version(ctx):
    """Report OpenSSL version to ./waf configure."""
    _ = "XXX_LIBSSL_VERSION"
    ctx.start_msg("LibSSL version")
    ret = ctx.check_cc(
        fragment=SNIP_OPENSSL_DUMP_VERSION,
        execute=True,
        define_ret=True,
        define_name=_,
        mandatory=False,
    )
    if ret:
        ctx.end_msg(' '.join(ctx.get_define(_).split()[0:2])[1:])
        ctx.undefine(_)
    else:
        ctx.end_msg("Failed")
