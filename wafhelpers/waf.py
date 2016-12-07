from waflib.Configure import conf
from waflib.TaskGen import feature, before_method
from waflib.Task import Task
import os

@before_method('apply_incpaths')
@feature('bld_include')
def insert_blddir(self):
	bldnode = self.bld.bldnode.parent.abspath()
	self.includes += [bldnode]

@before_method('apply_incpaths')
@feature('src_include')
def insert_srcdir(self):
	srcnode = self.bld.srcnode.abspath()
	self.includes += ["%s/include" % srcnode]


@before_method('apply_incpaths')
@feature('libisc_include')
def insert_libiscdir(self):
	srcnode = self.bld.srcnode.abspath()
	self.includes += ["%s/libisc/include/" % srcnode]


@before_method('apply_incpaths')
@feature('libisc_pthread_include')
def insert_libiscpthreaddir(self):
	srcnode = self.bld.srcnode.abspath()
	self.includes += ["%s/libisc/pthreads/include/" % srcnode]


# Create version.c
class version(Task):
	vars = ['NTPSEC_VERSION_STRING', 'TARGET']

	def run(self):
		self.outputs[0].write("""
const char *Version = "%s " __DATE__ " " __TIME__;
const char *VVersion = "CFLAGS=%s LDFLAGS=%s";
""" % (self.env.NTPSEC_VERSION_STRING, "Need-CFLAGS", "Need-LDFLAGS"))

# Use in features= to generate a version.c for that target.
# This uses the target name for the version string.
@before_method('process_source')
@feature('ntp_version')
def ntp_version(self):
	n = self.path.find_or_declare('version.c')
	tsk = self.create_task('version', [], [n])
	tsk.env.TARGET = self.target
	self.source.extend([n])


def manpage_subst_fun(task, text):
	return text.replace("include::../docs/", "include::../../../docs/")

@conf
def manpage(ctx, section, source):

	if ctx.env.MANDIR:
		manprefix = ctx.env.MANDIR
	elif os.path.isdir("/usr/man"):
		manprefix = "/usr/man"
	else:
		manprefix = "/usr/share/man"
	if not manprefix.endswith("/"):
		manprefix += "/"

        #ctx.install_files(manprefix + "man%s/" % section, source.replace("-man.txt", ".%s" % section))

	if not ctx.env.ENABLE_DOC or ctx.env.DISABLE_MANPAGE:
		return

	ctx(
		features    = "subst",
		source      = source,
		target      = source + '.man-tmp',
		subst_fun   = manpage_subst_fun
	)

	ctx(source=source + '.man-tmp', section=section)


@conf
def ntp_test(ctx, **kwargs):
	bldnode = ctx.bldnode.abspath()
	tg = ctx(**kwargs)

	args = ["%s/tests/%s" % (bldnode, tg.target), "-v"]

	if hasattr(tg, "test_args"):
		args += tg.test_args

	tg.ut_exec = args
