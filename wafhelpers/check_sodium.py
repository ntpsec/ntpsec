import sys
from waflib.Logs import pprint

def check_sodium(ctx):
    ctx.check_cc(header_name="sodium.h", mandatory=True)
    ctx.check_cc(lib="sodium", comment="Sodium crypto library", mandatory=True)

