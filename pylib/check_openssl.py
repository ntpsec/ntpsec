from waflib.Configure import conf
from util import msg, msg_setting

from posix_thread import posix_thread_version

import os

OPENSSL_FRAG = """
%s
int main () {
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms();
	return 0;
}
"""

def configure_ssl(ctx):

	OPENSSL_HEADERS=True
	OPENSSL_LIB=True

	headers = (
		"openssl/asn1_mac.h",
		"openssl/bn.h",
		"openssl/err.h",
		"openssl/evp.h",
		"openssl/pem.h",
		"openssl/rand.h",
		"openssl/objects.h",
		"openssl/x509v3.h",
	)

	for hdr in headers:
		if not ctx.check_cc(header_name=hdr, mandatory=False):
			OPENSSL_HEADERS=False

	libs = ["ssl", "crypto"]

	for lib in libs:
		if not ctx.check_cc(lib=lib, mandatory=False):
			OPENSSL_LIB=False


	if OPENSSL_HEADERS and OPENSSL_LIB:
		ctx.check_cc(
			fragment	= OPENSSL_FRAG % "\n".join(["#include <%s>" % x for x in headers]),
			define_name = "HAVE_OPENSSL",
			execute     = True,
			define_ret  = False,
			mandatory	= False,
			use			= "SSL CRYPTO",
			msg			= "Checking if OpenSSL works"
		)

