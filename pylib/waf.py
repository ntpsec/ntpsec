from waflib.Configure import conf
from waflib.TaskGen import feature, before, after, extension, after_method, before_method
from waflib.Task import Task


@before_method('apply_incpaths')
@feature('bld_include')
def insert_blddir(self):
	bldnode = self.bld.bldnode.abspath()
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
	self.includes += ["%s/lib/isc/include/" % srcnode, "%s/lib/isc/unix/include/" % srcnode]


@before_method('apply_incpaths')
@feature('libisc_pthread_include')
def insert_libiscpthreaddir(self):
	srcnode = self.bld.srcnode.abspath()
	self.includes += ["%s/lib/isc/pthreads/include/" % srcnode]

class version(Task):
	vars = ['NTPS_VERSION_STRING', 'TARGET']
	def run(self):
		self.outputs[0].write("""
const char *Version = "%s " __DATE__ " " __TIME__;
""" % self.env.NTPS_VERSION_STRING)

@before_method('process_source')
@feature('ntp_version')
def ntp_version(self):
	n = self.path.find_or_declare('version.c')
	tsk = self.create_task('version', [], [n])
	tsk.env.TARGET = self.target
	self.source.extend([n])

