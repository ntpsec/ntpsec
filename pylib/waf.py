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
