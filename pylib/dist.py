from os import path
from shutil import copyfile
from os.path import exists
import time

from .util import parse_version

def dist_cmd(ctx, config):
		files_man = []

		parse_version(config)

		if not ctx.options.build_snapshot and not config["NTPS_RELEASE"]:
			ctx.fatal('Please supply --build-snapshot or set config["NTPS_RELEASE"] = True')

		if not config["NTPS_RELEASE"]:
			ctx.fatal('config["NTPS_RELEASE"] must be set to True')

		if exists("build/c4che/host_cache.py"):
			from waflib.ConfigSet import ConfigSet
			cset = ConfigSet()
			cset.load("build/c4che/host_cache.py")
		else:
			ctx.fatal("Please run 'waf configure' first.")

		rev = cset.NTPS_REVISION[:7]
		timestamp = time.strftime("%Y-%m-%d_%H%M%S")

		# XXX: Redo to not use globs.
		bldnode = ctx.path.make_node(config["out"])
		for section in [1, 5, 8]:
			files_man += bldnode.ant_glob("**/*.%d" % section)

		# Need a more reliable check.
		if not files_man:
			ctx.fatal("You must configure and build first with NTPS_RELEASE set to false")

		for man in files_man:
			src = man.abspath()
			dst = src.replace("%s/main/" % bldnode.abspath(), "")
			print("Copying %s -> %s" % (src, dst))
			copyfile(src, dst)

		files = [
			("build/host/ntpd/ntp_parser.tab.c", "ntpd/ntp_parser.tab.c"),
			("build/host/ntpd/ntp_parser.tab.h", "ntpd/ntp_parser.tab.h")
		]

		for src, dst in files:
			if not path.exists(src):
				ctx.fatal("%s doesn't exist please configure and build first.  NTPS_RELEASE must be set to False" % src)
			print("Copying %s -> %s" % (src, dst))
			copyfile(src, dst)

		version = "%d.%d.%d" % (config["NTPS_VERSION_MAJOR"], \
						config["NTPS_VERSION_MINOR"], \
						config["NTPS_VERSION_REV"])


		if ctx.options.build_snapshot:
			ctx.base_name = "ntpsec-%s-%s-%s-snap" % (timestamp, version, rev)
		else:
			ctx.base_name = "ntpsec-%s" % version

		if ctx.options.build_version_tag:
			ctx.base_name = "%s-%s" % (ctx.base_name, ctx.options.build_version_tag)

		if ctx.options.build_snapshot:
			with open("VERSION", "w") as fp:
				fp.write("%s %s %s\n" % (version, timestamp, rev))
