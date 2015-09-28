# asciidoc -b html5 -a linkcss -a stylesdir=/mnt/devel/ntp/commit/docs -o asd driver32.txt

from waflib import Task
from waflib.TaskGen import extension

# ASCIIDOC_FLAGS are almost always needed and need to be set by the user.
class asciidoc(Task.Task):
	color	= "BLUE"
	run_str = '${BIN_ASCIIDOC} -b html5 -a linkcss ${ASCIIDOC_FLAGS} -o ${TGT[0].name} ${SRC[0].abspath()}'
	ext_out = ".html"


@extension('.txt')
def run_asciidoc(self, node):
	out = node.change_ext(".html")
	tsk = self.create_task("asciidoc", node, [out])
	tsk.cwd = node.parent.get_bld().abspath()

