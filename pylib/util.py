from waflib.Logs import pprint

def msg(str):
	pprint("YELLOW", str)

def msg_setting(name, val):
	pprint("NORMAL", "  %-30s: " % name, sep="")
	pprint("YELLOW", val)

