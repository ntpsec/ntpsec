import re
from waflib.Logs import pprint


def msg(str):
        pprint("YELLOW", str)


def msg_setting(name, val):
        pprint("NORMAL", "  %-30s: " % name, sep="")
        pprint("YELLOW", val)

# Borrowed from waf/docs/book/wscript in the Waf Project.
re_xi = re.compile('''^(include|image)::(.*?.(txt|\\{PIC\\}))\[''', re.M)


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


def parse_version(config):
        with open("VERSION", "r") as f:
                version_string = f.read().split(" ")[0].strip()
                [major, minor, rev] = version_string.split(".")
                # "NTPS" for NTPSec
                # this avoids any naming collisions
                map = {"NTPSEC_VERSION_MAJOR": int(major),
                       "NTPSEC_VERSION_MINOR": int(minor),
                       "NTPSEC_VERSION_REV": int(rev)}

        config.update(map)
