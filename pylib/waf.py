from waflib.Configure import conf
from waflib.TaskGen import feature, before, after, extension, after_method, before_method
from waflib.Task import Task


@after_method('apply_incpaths')
@feature('bld_include')
def insert_blddir(self):
	srcnode = self.bld.bldnode.abspath()
	self.env.prepend_value('INCPATHS', srcnode)

@after_method('apply_incpaths')
@feature('src_include')
def insert_srcdir(self):
	srcnode = self.bld.srcnode.abspath()
	self.env.prepend_value('INCPATHS', ["%s/include" % srcnode])


@after_method('apply_incpaths')
@feature('libisc_include')
def insert_libiscdir(self):
	srcnode = self.bld.srcnode.abspath()
	self.env.prepend_value('INCPATHS', ["%s/lib/isc/include/" % srcnode, "%s/lib/isc/unix/include/" % srcnode])


@after_method('apply_incpaths')
@feature('libisc_pthread_include')
def insert_libiscpthreaddir(self):
	srcnode = self.bld.srcnode.abspath()
	self.env.prepend_value('INCPATHS', ["%s/lib/isc/pthreads/include/" % srcnode])


@after_method('apply_incpaths')
@feature('libopts_include')
def insert_libopts(self):
	srcnode = self.bld.srcnode.abspath()
	self.env.prepend_value('INCPATHS', ["%s/sntp/libopts/" % srcnode])


class version(Task):
	vars = ['VERSION_FULL', 'TARGET']
	def run(self):
		self.outputs[0].write("const char *Version = \"%s %s\";" % (self.env.TARGET, self.env.VERSION_FULL))

@before_method('process_source')
@feature('ntp_version')
def ntp_version(self):
	n = self.path.find_or_declare('version.c')
	tsk = self.create_task('version', [], [n])
	tsk.env.TARGET = self.target
	self.source.extend([n])
