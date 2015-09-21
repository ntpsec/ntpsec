PCAP_FRAG = """
# include <sys/capability.h>

int main(void) {
    cap_t caps;
    char *txt_caps;

    caps = cap_from_text("cap_chown+e");
    cap_free(caps);

    return 0;
}
"""


def check_cap(ctx):
	ctx.check_cc(
		fragment	= PCAP_FRAG,
		define_name = "HAVE_CAPABILITY",
		features	= "c",
		use		= "CAP",
		msg         = "Checking if libcap works",
		mandatory	= False
	)
