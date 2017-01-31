# Versions older than 0.9.7d were deemed incompatible in NTP Classic.
OPENSSL_FRAG = """
%s
int main(void) {
#if OPENSSL_VERSION_NUMBER < 0x0090704fL
#error OpenSSL is too old.
#endif
    ERR_load_BIO_strings();
    OpenSSL_add_all_digests();
    return 0;
}
"""


def configure_openssl(ctx):

    OPENSSL_HEADERS = True
    OPENSSL_LIB = True

    headers = (
        "openssl/err.h",
        "openssl/evp.h",
        "openssl/rand.h",    # only used in tests/libntp
        "openssl/objects.h",
    )

    for hdr in headers:
        if not ctx.check_cc(header_name=hdr, mandatory=True,
                            comment="<%s> header" % hdr):
            OPENSSL_HEADERS = False

    if not ctx.check_cc(lib="crypto", mandatory=False):
        OPENSSL_LIB = False

    if OPENSSL_HEADERS and OPENSSL_LIB:
        ctx.check_cc(
            fragment=OPENSSL_FRAG % "\n".join(["#include <%s>" % x
                                               for x in headers]),
            execute=True,
            mandatory=False,
            use="CRYPTO",
            msg="Checking if OpenSSL works",
            comment="OpenSSL support"
        )

