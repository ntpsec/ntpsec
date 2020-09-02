'''Most of the functionality for building HTML and man pages from AsciiDoc.'''

import re

from waflib import Task  # pylint: disable=import-error
from waflib.TaskGen import extension  # pylint: disable=import-error


def options(opt):
    'Add command line options for AsciiDoc processing.'
    grp = opt.add_option_group('NTP documentation configure options')
    grp.add_option('--disable-doc', action='store_true',
                   default=False, help='Disable HTML document building.')
    grp.add_option('--enable-doc', action='store_true',
                   default=False, help='Enable HTML document building.')
    grp.add_option('--disable-manpage', action='store_true',
                   default=False, help='Disable manpage building.')
    grp.add_option('--enable-manpage', action='store_true',
                   default=False, help='Enable manpage building.')


def configure(ctx):
    'Set options from the extended environment and command line arguments.'

    if ctx.options.disable_doc and ctx.options.enable_doc:
        ctx.fatal('--disable-doc and --enable-doc conflict.')
    if ctx.options.disable_manpage and ctx.options.enable_manpage:
        ctx.fatal('--disable-manpage and --enable-manpage conflict.')

    ctx.env.BUILD_DOC = False
    ctx.env.BUILD_MAN = False
    if ctx.options.disable_doc and ctx.options.disable_manpage:
        ctx.msg('AsciiDoc processor', 'unnecessary')
        return

    # asciidoctor versions < 1.5.8 throw warnings for manpages and driver_shm.
    # asciidoc versions 8.6.5-8.6.7 throw warnings for warp.adoc and versions
    # < 8.6.4 have no HTML5 backend.
    # asciidoc3 versions < 3.0.2 throw errors.
    adoc_list = [['asciidoctor', (1, 5, 8)],
                 ['asciidoc', (8, 6, 8)],
                 ['asciidoc3', (3, 0, 2)],
                 ]
    for progname, asciidocminver in adoc_list:
        if 'BIN_ASCIIDOC' not in ctx.env or ctx.env.BIN_ASCIIDOC == []:
            # used to make man and HTML pages
            ctx.find_program(progname, var='BIN_ASCIIDOC', mandatory=False)
            # make sure asciidoc is new enough.
            # based on check_python_version() from waf

            if ctx.env.BIN_ASCIIDOC:
                # https://lists.ntpsec.org/pipermail/devel/2016-July/001778.html
                # Get asciidoc version string
                cmd = ctx.env.BIN_ASCIIDOC + ['--version']
                # example output: asciidoc 8.6.9
                version_string = ctx.cmd_and_log(cmd).split()[1]
                match = re.match(r'^(\d+)\.(\d+)\.(\d+)', version_string)
                if match:
                    version_tuple = tuple(map(int, match.groups()))
                    if version_tuple >= asciidocminver:
                        if progname == 'asciidoc':
                            ctx.find_program('a2x', var='BIN_A2X', mandatory=False)
                            ctx.find_program('xsltproc', var='BIN_XSLTPROC',
                                             mandatory=False)
                        elif progname == 'asciidoc3':
                            ctx.find_program('a2x3', var='BIN_A2X', mandatory=False)
                            ctx.find_program('xsltproc', var='BIN_XSLTPROC',
                                             mandatory=False)

                if version_tuple >= asciidocminver:
                    color = 'GREEN'
                else:
                    color = 'YELLOW'
                    ctx.env.BIN_ASCIIDOC = []
                ctx.msg('Checking for %s version >= %s' %
                        (progname, '%d.%d.%d' % asciidocminver),
                        version_string, color=color)

    if not ctx.env.BIN_ASCIIDOC:
        if not (ctx.options.enable_doc or ctx.options.enable_manpage):
            # The user did not require either, so this is fine.
            return

        error = 'no AsciiDoc processor qualified'
        if ctx.options.enable_doc:
            error += ', remove --enable-doc'
        if ctx.options.enable_manpage:
            error += ', remove --enable-manpage'
        ctx.fatal(error)

    ctx.env.BUILD_DOC = not ctx.options.disable_doc
    ctx.env.BUILD_MAN = not ctx.options.disable_manpage

    if 'asciidoctor' in ctx.env.BIN_ASCIIDOC[0]:
        ctx.env.ARGS_DOC = [
            ctx.env.BIN_ASCIIDOC[0],
            '-a', 'attribute-missing=warn',
            # To eliminate compat-mode:
            #   'italics' needs to be changed to _italics_
            #   +monospace+ needs to be changed to `monospace`
            # https://asciidoctor.org/docs/migration/#migration-cheatsheet
            '-a', 'compat-mode',
            ]
        ctx.env.ARGS_MAN = ctx.env.ARGS_DOC + [
            '-b', 'manpage',
            ]
        ctx.env.ARGS_DOC += [
            '-b', 'xhtml5', '-a', 'linkcss',
            '-a', 'stylesheet=asciidoc.css',
            ]
    elif 'asciidoc' in ctx.env.BIN_ASCIIDOC[0]:
        ctx.env.ARGS_DOC = [
            ctx.env.BIN_ASCIIDOC[0],
            '-a', 'attribute-missing=warn',
            '-b', 'html5', '-a', 'linkcss',
            '-a', 'stylesheet=asciidoc.css',
            ]
        if ctx.env.BIN_A2X and ctx.env.BIN_XSLTPROC:
            ctx.env.ARGS_MAN = [
                ctx.env.BIN_A2X[0],
                '-a', 'attribute-missing=warn',
                '-f', 'manpage', '--no-xmllint',
                ]
        else:
            if ctx.options.enable_manpage:
                ctx.fatal('a2x/xsltproc not found, remove --enable-manpage')
            ctx.env.BUILD_MAN = False


def build(ctx):
    'Set processor noise level and set HTML pages to build.'
    from waflib.Logs import verbose  # pylint: disable=import-error
    if verbose > 1:  # Pass verbosity to AsciiDoc toolchain
        if ctx.env.ARGS_DOC:
            ctx.env.ARGS_DOC += ['-v']
        if ctx.env.ARGS_MAN:
            ctx.env.ARGS_MAN += ['-v']
    if ctx.env.BUILD_DOC and ctx.variant == 'main':
        ctx.recurse('docs')


class html(Task.Task):
    'Define HTML build process.'
    # Optional weight to tune the priority for task instances.
    # The higher, the earlier. The weight only applies to single task objects.
    weight = 3  # set arbitrarily high to be first as to not slow down later tasks
    run_str = '${ARGS_DOC} -o ${TGT[0].name} ${SRC[0].abspath()}'
    ext_out = '.html'


class man(Task.Task):
    'Define manpage build process.'
    weight = 2  # set arbitrarily high to be second as to not slow down later tasks (Failed)
    run_str = '${ARGS_MAN} ${SRC[0].abspath()}'


@extension('.adoc')
def run_html(self, node):
    'Add HTML build caller function.'
    out = node.change_ext('.html')
    tsk = self.create_task('html', node, [out])
    tsk.cwd = node.parent.get_bld().abspath()


@extension('.man-tmp')
def run_manpage(self, node):
    'Add manpage build caller function.'
    n_file = node.path_from(self.bld.bldnode)
    out = '%s.%s' % (n_file.replace('-man.adoc.man-tmp', ''), self.section)
    out_n = self.bld.path.find_or_declare(out)
    self.create_task('man', node, out_n)
    self.bld.install_files('${MANDIR}/man%s/' % self.section, out_n)
    #weight = 3
