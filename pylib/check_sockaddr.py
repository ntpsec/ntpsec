SOCKADDR_STORAGE_FRAG = """
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

struct sockaddr_storage n;
"""


SA_LEN_FRAG = """
#include <sys/types.h>
#include <sys/socket.h>
int main () {
  extern struct sockaddr *ps;
  return ps->sa_len;
}
"""


def check_sockaddr(ctx):
	ctx.check_cc(
		fragment	= SOCKADDR_STORAGE_FRAG,
		define_name = "HAVE_STRUCT_SOCKADDR_STORAGE",
		features	= "c",
		msg         = "Checking for type sockaddr_storage",
		mandatory	= False
	)

	ctx.check_cc(
		fragment	= SA_LEN_FRAG,
		define_name = "ISC_PLATFORM_HAVESALEN",
		features	= "c",
		msg         = "Checking for sockaddr->sa_len",
		mandatory	= False
	)

