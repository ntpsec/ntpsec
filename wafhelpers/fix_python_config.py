"""Work around waf bugs related to Python config setup."""

from waflib import Utils  # pylint: disable=import-error

# NOTE:  Everything in this file relates to waf bugs.  It can go away once
# the waf bugs are fixed.

# The waf code for setting up PYTHONDIR and PYTHONARCHDIR is broken, because
# get_python_lib() only returns the proper library locations when the 'prefix'
# argument is absent or None, but waf insists on feeding it the PREFIX value.
# To make matters worse, the design of waf's ConfigSet object makes it
# impossible to get it to return a value of None for any item, so merely
# temporarily patching PREFIX doesn't work.  Thus, the most straightforward
# workaround is to duplicate the relevant code with the properly omitted
# 'prefix'.
#
# In principle, there might be some value in allowing a prefix to be optionally
# supplied (separately from PREFIX), but given that both values are already
# overridable, there's little need for an additional option.
#
# Naturally, the fix doesn't override user-supplied values, but the fixup runs
# after the version check, using the captured original settings to determine
# whether to override.


class FixConfig(object):
    """Methods and state for working around waf's python-config bugs."""
    def __init__(self, conf):
        """Initialize state from conf object."""
        self.conf = conf
        self.opts = None

    def get_options(self):
        """Capture values after option processing."""
        self.opts = self.conf.env.derive().detach()

    def fix_python_libs(self):
        """Fix up library install paths."""
        if Utils.is_win32:
            return  # No fixups supported on Windows
        if not ('PYTHONDIR' in self.opts or 'PYTHONDIR' in self.conf.environ):
            (pydir,) = self.conf.get_python_variables(
                ["get_python_lib(plat_specific=0)"]
                )
            self.conf.env.PYTHONDIR = pydir
        if not ('PYTHONARCHDIR' in self.opts
                or 'PYTHONARCHDIR' in self.conf.environ):
            (pyarchdir,) = self.conf.get_python_variables(
                ["get_python_lib(plat_specific=1)"]
                )
            self.conf.env.PYTHONARCHDIR = pyarchdir or pydir

    def load(self, *args, **kwargs):
        """Do the load and capture the options."""
        self.conf.load(*args, **kwargs)
        self.get_options()

    def check_python_version(self, *args, **kwargs):
        """Check version and fix up install paths."""
        self.conf.check_python_version(*args, **kwargs)
        self.fix_python_libs()
