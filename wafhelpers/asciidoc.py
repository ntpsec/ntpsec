from waflib import Task
from waflib.TaskGen import extension
from wafhelpers.util import ascii_doc_scan

# asciidoc -b html5 -a linkcss -a stylesdir=/mnt/devel/ntp/commit/docs \
#   -o asd driver32.txt


# ASCIIDOC_FLAGS are almost always needed and need to be set by the user.
class asciidoc(Task.Task):
    color = "BLUE"
    run_str = '${BIN_ASCIIDOC} -b html5 -a linkcss ${ASCIIDOC_FLAGS} ' \
              '-o ${TGT[0].name} ${SRC[0].abspath()}'
    ext_out = ".html"


@extension('.txt')
def run_asciidoc(self, node):
    out = node.change_ext(".html")
    tsk = self.create_task("asciidoc", node, [out])
    tsk.cwd = node.parent.get_bld().abspath()


class a2x(Task.Task):
    color = "YELLOW"
    shell = True
    run_str = '${BIN_A2X} ${A2X_FLAGS} ${SRC[0].abspath()}'
    scan = ascii_doc_scan


@extension('.man-tmp')
def run_a2x(self, node):
    n_file = node.path_from(self.bld.bldnode)
    out = "%s.%s" % (n_file.replace("-man.txt.man-tmp", ""), self.section)
    out_n = self.bld.path.find_or_declare(out)
    tsk = self.create_task('a2x', node, out_n)
    self.bld.install_files("${PREFIX}/man/man%s/" % self.section, out_n)
