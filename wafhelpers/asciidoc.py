import re

from waflib import Task
from waflib.TaskGen import extension

# asciidoc -b html5 -a linkcss -a stylesdir=/mnt/devel/ntp/commit/docs \
#   -o asd driver32.adoc

# Borrowed from waf/docs/book/wscript in the Waf Project.
re_xi = re.compile('''^(include|image)::(.*?.(adoc|\\{PIC\\}))\[''', re.M)


def ascii_doc_scan(self):
    p = self.inputs[0].parent
    node_lst = [self.inputs[0]]
    seen = []
    depnodes = []

    while node_lst:
        nd = node_lst.pop(0)
        if nd in seen:
            continue
        seen.append(nd)

        code = nd.read()
        for m in re_xi.finditer(code):
            name = m.group(2)
            if m.group(3) == '{PIC}':

                ext = '.eps'
                if self.generator.rule.rfind('A2X') > 0:
                    ext = '.png'

                k = p.find_resource(name.replace('{PIC}', ext))
                if k:
                    depnodes.append(k)
            else:
                k = self.inputs[0].find_resource(name)
                if k:
                    depnodes.append(k)
                    node_lst.append(k)
    return [depnodes, ()]


# ASCIIDOC_FLAGS are almost always needed and need to be set by the user.
class asciidoc(Task.Task):
    color = "BLUE"
    run_str = '${BIN_ASCIIDOC} -b html5 -a linkcss ${ASCIIDOC_FLAGS} ' \
              '-o ${TGT[0].name} ${SRC[0].abspath()}'
    ext_out = ".html"


@extension('.adoc')
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
    out = "%s.%s" % (n_file.replace("-man.adoc.man-tmp", ""), self.section)
    out_n = self.bld.path.find_or_declare(out)
    self.create_task('a2x', node, out_n)
    self.bld.install_files("${MANDIR}/man%s/" % self.section, out_n)
