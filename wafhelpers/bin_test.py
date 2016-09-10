import re
from os.path import exists
from waflib.Utils import subprocess
from waflib.Logs import pprint

cmd_map = {
	("main/ntpd/ntpd",				"-invalid"):			r'.*must be run as root, not uid.*',
	("main/ntpdig/ntpdig",			"time.apple.com"):		r'.*time.apple.com.*',
	("main/ntpfrob/ntpfrob",			"-h"):				r'.*illegal option.*',
	("main/ntpfrob/ntpfrob",			"-b 100000"):		r".*Bumping clock by 100000 microseconds.*",
	("main/ntpkeygen/ntpkeygen",	  "-M"):				r'.*Generating new md5 file and link.*',
	("main/ntpq/ntpq",  			  "-p"):				r'.*remote.*jitter.*',
	("main/ntptime/ntptime",		  None):				r'.*ntp_gettime\(\) returns code 0 \(OK\).*',
	("main/util/sht",				  "2:r"):				r'.*reader.*',

# XXX: Need to figure out how to test this.
#	("main/util/hist",  			  ""):  				r'',

# Perl library
#	("main/ntptrace/ntptrace",  	  ""):  				r'',
#	("main/ntpwait/ntpwait",		  ""):  				r'',
#	("main/util/ntpsweep/ntpsweep",   ""):  				r'',
}


# XXX: Needs to run in a thread with a timeout.
def run(cmd, reg):
	check = False

	if cmd[1] == None:
		cmd = [cmd[0]]

	print "running: ", " ".join(cmd),

	if not exists("build/%s" % cmd[0]):
		pprint("YELLOW", " SKIPPING (does not exist)")
		return False

	p = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, env=None, cwd="build")

	stdout, stderr = p.communicate()

	regex = re.compile(reg)

	if regex.match(stdout) or regex.match(stderr):
		check = True

	if check:
		pprint("GREEN", "  OK")
		return False
	else:
		pprint("RED", "  FAILED")
		return True


def cmd_bin_test(ctx, config):
	fail = True

	for cmd in sorted(cmd_map):
		fail = run(cmd, cmd_map[cmd])

	if fail:
		pprint("RED", "Tests failed!")
#		ctx.fatal("Failed")

#cmd_bin_test(None, None)
