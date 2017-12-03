from waflib.Configure import conf
from waflib.TaskGen import feature, before_method


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


def manpage_subst_fun(task, text):
    return text.replace("include::../docs/", "include::../../../docs/")


@conf
def manpage(ctx, section, source):

    # ctx.install_files('${MANDIR}' + "/man%s/" % section,
    #                   source.replace("-man.txt", ".%s" % section))

    if not ctx.env.ENABLE_DOC or ctx.env.DISABLE_MANPAGE:
        return

    ctx(features="subst",
        source=source,
        target=source + '.man-tmp',
        subst_fun=manpage_subst_fun)

    ctx(source=source + '.man-tmp', section=section)


@conf
def ntp_test(ctx, **kwargs):
    bldnode = ctx.bldnode.abspath()
    tg = ctx(**kwargs)

    args = ["%s/tests/%s" % (bldnode, tg.target), "-v"]

    if hasattr(tg, "test_args"):
        args += tg.test_args

    tg.ut_exec = args
