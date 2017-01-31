# Versions older than 0.9.7d were deemed incompatible in NTP Classic.
OPENSSL_FRAG = """
%s
int main(void) {
#if OPENSSL_VERSION_NUMBER < 0x0090704fL
#error OpenSSL is too old.
#endif
    OpenSSL_add_all_digests();
    return 0;
}
"""


def configure_openssl(ctx):

    # Some of these headers may not be needed.  Check the build on Debian sid
    # and other older systems before removing any, though; incautious removals
    # have caused build breakage.
    headers = (
        "openssl/asn1_mac.h",
        "openssl/bn.h",
        "openssl/err.h",
        "openssl/evp.h",
        "openssl/pem.h",
        "openssl/rand.h",
        "openssl/objects.h",
        "openssl/x509v3.h",
        "openssl/ssl.h",
    )

    ctx.check_cc(lib="crypto", mandatory=True)
    ctx.check_cc(
        fragment=OPENSSL_FRAG % "\n".join(["#include <%s>" % x
                                           for x in headers]),
        execute=True,
        use="CRYPTO",
        msg="Checking if OpenSSL works",
        mandatory=True,
        comment="OpenSSL support"
    )

