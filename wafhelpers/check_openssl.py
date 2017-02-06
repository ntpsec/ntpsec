# Versions older than 0.9.7d were deemed incompatible in NTP Classic.
OPENSSL_FRAG = """
%s
int main(void) {
#if OPENSSL_VERSION_NUMBER < 0x0090704fL
#error OpenSSL is too old.
#endif
#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
    OpenSSL_add_all_digests();
#endif
    return 0;
}
"""


def configure_openssl(ctx):

    OPENSSL_HEADERS = True
    OPENSSL_LIB = True

    headers = (
        "openssl/evp.h",
        "openssl/rand.h",
        "openssl/objects.h",
    )

    for hdr in headers:
        if not ctx.check_cc(header_name=hdr, comment="<%s> header" % hdr,
            includes=ctx.env.PLATFORM_INCLUDES,
            ):
            OPENSSL_HEADERS = False

    if not ctx.check_cc(lib="crypto"):
        OPENSSL_LIB = False

    if OPENSSL_HEADERS and OPENSSL_LIB:
        ctx.check_cc(
            comment="OpenSSL support",
            execute=True,
            fragment=OPENSSL_FRAG % "\n".join(["#include <%s>" % x
                                               for x in headers]),
            includes=ctx.env.PLATFORM_INCLUDES,
            msg="Checking if OpenSSL works",
            use="CRYPTO",
        )

