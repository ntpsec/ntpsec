"""tlscheck - Helper for checking SSL library bits."""
import ctypes
import ctypes.util
import sys
ver, vers = 0, []
try:
    tls = ctypes.CDLL(ctypes.util.find_library('ssl'))
except OSError:
    sys.stderr.write('Could not find SSL library.\n')
    sys.exit(1)

tls.OpenSSL_version_num.restype = ctypes.c_ulong
tls.OpenSSL_version.argtypes = [ctypes.c_int]
tls.OpenSSL_version.restype = ctypes.c_char_p

ver = tls.OpenSSL_version_num()  # unsigned long OpenSSL_version_num();

_ = '%08x' % ver
# OPENSSL_VERSION_NUMBER is a numeric release version identifier:
# MNNFFPPS: major minor fix patch status
for a, b in ((0, 1), (1, 3), (3, 5), (5, 7), (7, 8)):
    vers.append(int(_[a:b], 16))

polystr = str
if str is not bytes:
    def polystr(string):
        """Convert bytes into a string."""
        return str(string, encoding='latin-1')


def ver_to_int(*va):
    """Split the version number into parts."""
    return int('%x%02x%02x%02x%x' % va, 16)


def verstr():
    """Return SSL library version string."""
    return polystr(tls.OpenSSL_version(0))


if __name__ == '__main__':
    if vers[0] > 2:  # If notionally OpenSSL 3
        sys.exit(0)
    elif vers[0] == 2:  # If notionally OpenSSL 2
        sys.exit(1)
    # OPENSSL_VERSION_NUMBER is a numeric release version identifier:
    # major minor fix patch status
    # Check if version is earlier than 1.1.1b
    if ver <= ver_to_int(1, 1, 1, 2, 15):
        sys.exit(1)
    sys.exit(0)
