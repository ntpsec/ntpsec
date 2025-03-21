#! @PYSHEBANG@
# -*- coding: utf-8 -*-
#
# ntpq - query an NTP server using mode 6 commands
#
# Freely translated from the old C ntpq code by ESR.  The idea was to
# cleanly separate ntpq-that-was into a thin front-end layer handling
# mainly command interpretation and a back-end that presents the take
# from ntpd as objects that can be re-used by other front
# ends. Reusable pieces live in pylib.

# Copyright the NTPsec project contributors
#
# SPDX-License-Identifier: BSD-2-Clause

from __future__ import print_function, division

import cmd
import collections
import getopt
import os
import re
import resource
import socket
import sys
import time

try:
    import ntp.control
    import ntp.ntpc
    import ntp.packet
    import ntp.util
    import ntp.poly
except ImportError as e:
    sys.stderr.write(
        "ntpq: can't find Python NTP library -- check PYTHONPATH.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)

# This used to force UTF-8 encoding, but that breaks the readline system.
# Unfortunately sometimes sys.stdout.encoding lies about the encoding,
# so expect random false positives.
ntp.util.check_unicode()

version = ntp.util.stdversion()

# Flags for forming descriptors.
OPT = 0x80        # this argument is optional, or'd with type */
NO = 0x0
NTP_STR = 0x1     # string argument
NTP_UINT = 0x2    # unsigned integer
NTP_INT = 0x3     # signed integer
NTP_ADD = 0x4     # IP network address
IP_VERSION = 0x5  # IP version
NTP_ADP = 0x6     # IP address and port
NTP_LFP = 0x7     # NTP timestamp
NTP_MODE = 0x8    # peer mode
NTP_2BIT = 0x9    # leap bits
NTP_FLOAT = 0xa   # Float value
NTP_UPTIME = 0xb  # uptime in daysD H:M:S (usually no frac)
NTP_PACKETS = 0xc # packet counts


class Ntpq(cmd.Cmd):
    "ntpq command interpreter"

    def __init__(self, session):
        cmd.Cmd.__init__(self)
        self.session = session
        self.prompt = "ntpq> " if os.isatty(0) else ""
        self.interactive = False        # set to True when we should prompt
        # self.auth_keyid   = 0# Keyid used for authentication.
        # self.auth_keytype = "NID_md5"# MD5 (FIXME: string value is a dummy)
        # self.auth_hashlen = 16# MD5
        # I do not know if the preceding are there for a specific reason
        #  so I am leaving them, and possibly duplicating them.
        self.rawmode = False            # Flag which indicates raw mode output.
        self.directmode = False         # Flag for direct MRU output.
        self.showhostnames = 1          # If & 1, display names
        self.showunits = False          # If False, show old style float
        self.auth_delay = 20            # delay time (default 20msec)
        self.wideremote = False         # show wide remote names?
        self.ccmds = []                 # Queued commands
        self.chosts = []                # Command-line hosts
        self.peers = []                 # Data from NTP peers.
        self.debug = 0
        self.logfp = sys.stderr
        self.pktversion = ntp.magic.NTP_OLDVERSION + 1
        self.uservars = collections.OrderedDict()
        self.ai_family = socket.AF_UNSPEC
        self.termwidth = ntp.util.termsize()[0]

    def get_names(self):
        # Overrides a method in Cmd
        return [x.replace('hot_', ':').replace('config_from_file',
                                               'config-from-file')
                for x in dir(self.__class__)]

    def emptyline(self):
        "Called when an empty line is entered in response to the prompt."
        pass

    def precmd(self, line):
        if line.startswith(":config"):
            line = "hot_" + line[1:]
        elif line.startswith("config-from-file"):
            line = line.replace("config-from-file", "config_from_file")
        return line

    def default(self, line):
        "Called on an input line when the command prefix is not recognized."
        cmd, arg, line = self.parseline(line)
        try:
            dotext = 'do_'+cmd
            cmdprefixlist = [a[3:] for a in self.get_names()
                             if a.startswith(dotext)]
            if len(cmdprefixlist) == 1:
                line = line.replace(cmd, cmdprefixlist[0])
                cmd = cmdprefixlist[0]
            elif len(cmdprefixlist) > 1:
                self.warn("***Command `%s' ambiguous" % cmd)
                return
            elif not cmdprefixlist:
                self.warn("***Command `%s' unknown" % cmd)
                return

            if cmd == "help" and arg:
                helptext = 'help_'+arg
                if helptext not in self.get_names():
                    argprefixlist = [a[5:] for a in self.get_names()
                                     if a.startswith(helptext)]
                    if len(argprefixlist) == 1:
                        line = line.replace(arg, argprefixlist.pop())
                    elif len(argprefixlist) > 1:
                        self.warn("Command `%s' is ambiguous" % arg)
                        return
                    elif not argprefixlist:
                        self.warn("Command `%s' is unknown" % arg)
                        return

            self.onecmd(line)
        except TypeError:
            self.warn("Command `%s' is unknown" % line)

    def do_help(self, arg):
        if arg:
            helptext = 'help_'+arg
            if helptext not in self.get_names():
                argprefixlist = [a[5:] for a in self.get_names()
                                 if a.startswith(helptext)]
                if len(argprefixlist) == 1:
                    arg = argprefixlist.pop()
                elif len(argprefixlist) > 1:
                    self.warn("Command `%s' is ambiguous." % arg)
                    return
        cmd.Cmd.do_help(self, arg)

    def say(self, msg):
        try:
            sys.stdout.write(ntp.poly.polyunicode(msg))
        except UnicodeEncodeError as e:
            self.warn("Unicode failure: %s" % str(e))
            self.warn("msg:\n" + repr(msg))
            raise e
        sys.stdout.flush()    # In case we're piping the output

    def warn(self, msg):
        sys.stderr.write(ntp.poly.polystr(msg) + "\n")

    def help_help(self):
        self.say("""\
function: tell the use and syntax of commands
usage: help [ command ]
""")

    # Unexposed helper tables and functions begin here

    def __dogetassoc(self):
        try:
            self.peers = self.session.readstat()
        except ntp.packet.ControlException as e:
            self.warn(e.message)
            return False
        except IOError as e:
            self.warn(e.strerror)
            return False

        if not self.peers:
            if self.chosts:
                self.say("server=%s " % self.session.hostname)
            self.say("No association IDs returned\n")
            return False

        if self.debug:
            self.warn("\n%d associations total" % len(self.peers))
        # sortassoc()
        return True

    def __printassoc(self, showall):
        if not self.peers:
            self.say("No association IDs in list\n")
            return
        self.say(
            "\nind assid status  conf reach auth condition  last_event cnt\n")
        self.say(
            "===========================================================\n")
        for (i, peer) in enumerate(self.peers):
            statval = ntp.control.CTL_PEER_STATVAL(peer.status)
            if (not showall and
                    (statval & (ntp.control.CTL_PST_CONFIG |
                                ntp.control.CTL_PST_REACH)) == 0):
                continue
            sw = ntp.util.PeerStatusWord(peer.status)
            display = "%3d %5u  %04x   %3.3s  %4s  %4.4s %9.9s %11s %2lu" \
                % (i + 1, peer.associd, peer.status, sw.conf, sw.reach,
                   sw.auth, sw.condition, sw.last_event, sw.event_count)
            self.say(display + "\n")

    def __dopeers(self, showall, mode):
        if not self.__dogetassoc():
            return
        report = ntp.util.PeerSummary(mode,
                                      self.pktversion,
                                      self.showhostnames,
                                      self.wideremote,
                                      self.showunits,
                                      termwidth=interpreter.termwidth,
                                      debug=interpreter.debug,
                                      logfp=self.logfp)
        try:
            maxhostlen = 0
            if len(self.chosts) > 1:
                maxhostlen = max([len(host) for (host, _af) in self.chosts])
                self.say("%-*.*s "
                         % (maxhostlen, maxhostlen+1, "server"))
            self.say(report.header() + "\n")
            if len(self.chosts) > 1:
                maxhostlen = max([len(host) for (host, _af) in self.chosts])
                self.say("=" * (maxhostlen + 1))
            self.say(("=" * report.width()) + "\n")
            for peer in self.peers:
                if (not showall and
                    not (ntp.control.CTL_PEER_STATVAL(peer.status) &
                         (ntp.control.CTL_PST_CONFIG |
                          ntp.control.CTL_PST_REACH))):
                    if self.debug:
                        self.warn("eliding [%d]\n" % peer.associd)
                    continue
                try:
                    variables = self.session.readvar(peer.associd, raw=True)
                except ntp.packet.ControlException as e:
                    self.warn(e.message)
                    return
                except IOError as e:
                    self.warn(e.strerror)
                    return
                if not variables:
                    if len(self.chosts) > 1:
                        self.warn("server=%s " % self.session.hostname)
                    self.warn("***No information returned for association %d"
                              % peer.associd)
                    continue
                if len(self.chosts) > 1:
                    self.say(ntp.util.PeerSummary.high_truncate(
                             self.session.hostname, maxhostlen) + " " *
                             (maxhostlen + 1 - len(self.session.hostname)))
                self.say(report.summary(self.session.rstatus,
                                        variables, peer.associd))
        except KeyboardInterrupt:
            pass

    def __assoc_valid(self, line, required=False):
        "Process a numeric associd or index."
        # FIXME: This does a useless call to __dogetassoc() when associd == 0
        # No big deal most of the time.  Just a useless packet exchange.
        if not line:
            if required:
                self.warn("An associd argument is required.")
                return -1
            else:
                return 0
        if not self.peers:
            self.__dogetassoc()
        if line.startswith("&"):
            try:
                idx = int(line[1:].split()[0])
            except ValueError:
                self.warn("Invalid index literal.")
                return -1
            if idx < 0 or idx >= 2**16-1:
                self.warn("%d is not a valid association number." % idx)
                return -1
            elif idx not in range(1, len(self.peers)+1):
                self.warn("No such association as %d." % idx)
                return -1
            else:
                return self.peers[idx - 1].associd
        else:
            try:
                associd = int(line.split()[0])
            except ValueError:
                self.warn("Invalid associd literal.")
                return -1
            if (associd != 0 and
                    associd not in [peer.associd for peer in self.peers]):
                self.warn("Unknown associd.")
                return -1
            else:
                return associd

    def __assoc_range_valid(self, line):
        "Try to get a range of assoc IDs."
        tokens = line.split()
        if len(tokens) < 2:
            return ()
        lo = self.__assoc_valid(tokens[0])
        hi = self.__assoc_valid(tokens[1])
        if lo < 0 or hi < 0 or hi < lo:
            return ()
        if lo == hi:
            return(lo,)
        return range(lo, hi+1)

    def printvars(self, variables, dtype, quiet):
        "Dump variables in raw (actually, semi-cooked) mode."
        if self.rawmode:
            if not quiet:
                self.say("status=0x%04x,\n" % self.session.rstatus)
            # C ntpq not only suppressed \r but tried to visibilize
            # high-half characters.  We won't do that unless somebody
            # files a bug, Mode 6 never seems to generate those in
            # variable fetches.
            text = ntp.poly.polystr(session.response.replace(
                ",\r\n", ",\n"))
        else:
            if not quiet:
                self.say("status=%04x %s,\n"
                         % (self.session.rstatus,
                            ntp.ntpc.statustoa(dtype, self.session.rstatus)))
            text = ntp.util.cook(variables, self.showunits)
        text = text.replace("'", '"')
        self.say(text)

    def __dolist(self, varlist, associd, op, type, quiet=False):
        "List variables associated with a specified peer."
        try:
            variables = self.session.readvar(associd, varlist, op, raw=True)
        except ntp.packet.ControlException as e:
            self.warn(e.message)
            return False
        except IOError as e:
            self.warn(e.strerror)
            return False
        if len(self.chosts) > 1:
            self.say("server=%s " % self.session.hostname)
        if not variables:
            if associd == 0:
                self.say("No system%s variables returned\n"
                         % " clock" if (type == ntp.ntpc.TYPE_CLOCK) else "")
            else:
                self.say("No information returned for%s association %d\n"
                         % (" clock" if (type == ntp.ntpc.TYPE_CLOCK) else "",
                            associd))
            return True
        if not quiet:
            self.say("associd=%d " % associd)
        self.printvars(variables, type, not (not varlist))
        return True

    # Unexposed helper tables and functions end here

    def do_units(self, _unused):
        "toggle unit display"
        self.showunits = not self.showunits

    def help_units(self):
        self.say("""\
function: toggle unit display
usage: units
""")

    def do_EOF(self, _unused):
        "exit ntpq"
        self.say("\n")
        return True

    def do_timeout(self, line):
        "set the primary receive time out"
        if line:
            try:
                self.session.primary_timeout = int(line)
            except ValueError:
                self.warn("What?")
        self.say("primary timeout %d ms\n" % self.session.primary_timeout)

    def help_timeout(self):
        self.say("""\
function: set the primary receive time out
usage: timeout [ msec ]
""")

    def collect_display2(self, variables):
        "Query and display a collection of variables from the system."
        try:
            queried = self.session.readvar(0,
                                           [v[0] for v in variables] +
                                           [v[0] + '_r' for v in variables],
                                           raw=True)
        except ntp.packet.ControlException as e:
            if ntp.control.CERR_UNKNOWNVAR == e.errorcode:
                self.warn("Unknown variable.  Trying one at a time.")
                varlist = [v[0] for v in variables] + \
                          [v[0] + '_r' for v in variables]
                items = []
                for var in varlist:
                    try:
                        queried = self.session.readvar(0, [var],
                                                       raw=True)
                        for (name, (value, rawvalue)) in queried.items():
                            items.append((name, (value, rawvalue)))
                    except ntp.packet.ControlException as e:
                        if ntp.control.CERR_UNKNOWNVAR == e.errorcode:
                            items.append((var, ("???", None)))
                            continue
                        raise e
                queried = collections.OrderedDict(items)
            else:
                self.warn(e.message)
                return
        except IOError as e:
            self.warn(e.strerror)
            return
        if self.rawmode:
            self.say(self.session.response)
            return
        try:
            runs, runl = None, None
            for (name, legend, fmt) in variables:
                if name not in queried:
                    continue
                value = queried[name][0]
                rawvalue = queried[name][1]
                value2 = queried[name + '_r'][0]
                rawvalue2 = queried[name + '_r'][1]
                if fmt in (NTP_UINT, NTP_INT, NTP_FLOAT):
                    if self.showunits and isinstance(rawvalue, (int, float)):
                        display = ntp.util.unitifyvar(rawvalue, name)
                    else:
                        display = value
                    if self.showunits and isinstance(rawvalue2, (int, float)):
                        display2 = ntp.util.unitifyvar(rawvalue2, name)
                    else:
                        display2 = value2
                        self.say(
                            "{0:<13} {1:>13} {2:>13}\n".format(
                                legend, display, display2)
                        )
                elif fmt == NTP_PACKETS:
                    self.say(
                        "{0:<13} {1[0]:>13} {2[0]:>13} {1[1]:>10}{1[2]:<3} {2[1]:>10}{2[2]:<3}\n".format(
                                legend,
                                ntp.util.packetize(value, runs),
                                ntp.util.packetize(value2, runl),
                        )
                    )
                elif fmt == NTP_UPTIME:
                    runs, display = ntp.util.periodize(value)
                    runl, display2 = ntp.util.periodize(value2)
                    self.say(
                        "{0:<13} {1:>13} {2:>13}\n".format(
                            legend, display, display2)
                    )
                else:
                    self.warn("unexpected vc type %s for %s, value %s %s    "
                              % (fmt, name, value, value2))
        except KeyboardInterrupt:
            self.warn("display interrupted")

    def collect_display(self, associd, variables, decodestatus):
        "Query and display a collection of variables from the system."
        try:
            queried = self.session.readvar(associd,
                                           [v[0] for v in variables],
                                           raw=True)
        except ntp.packet.ControlException as e:
            if ntp.control.CERR_UNKNOWNVAR == e.errorcode:
                self.warn("Unknown variable.  Trying one at a time.")
                varlist = [v[0] for v in variables]
                items = []
                for var in varlist:
                    try:
                        queried = self.session.readvar(associd, [var],
                                                       raw=True)
                        for (name, (value, rawvalue)) in queried.items():
                            items.append((name, (value, rawvalue)))
                    except ntp.packet.ControlException as e:
                        if ntp.control.CERR_UNKNOWNVAR == e.errorcode:
                            items.append((var, "???"))
                            continue
                        raise e
                queried = collections.OrderedDict(items)
            else:
                self.warn(e.message)
                return
        except IOError as e:
            self.warn(e.strerror)
            return
        if self.rawmode:
            self.say(self.session.response)
            return
        if decodestatus:
            if associd == 0:
                statype = ntp.ntpc.TYPE_SYS
            else:
                statype = ntp.ntpc.TYPE_PEER
            self.say("associd=%u status=%04x %s,\n"
                     % (associd, self.session.rstatus,
                        ntp.ntpc.statustoa(statype, self.session.rstatus)))
        try:
            run = 1
            for (name, legend, fmt) in variables:
                if name not in queried:
                    continue
                value = queried[name][0]
                rawvalue = queried[name][1]
                if fmt in (NTP_ADD, NTP_ADP):
                    if self.showhostnames & 1:  # if & 1, display names
                        if self.debug:
                            self.say("DNS lookup begins...")
                        value = ntp.util.canonicalize_dns(
                            value, family=self.ai_family)
                        if self.debug:
                            self.say("DNS lookup complete.")
                    self.say("%13s %13s\n" % (legend, value))
                elif fmt == NTP_STR:
                    if value:
                        self.say("%13s %13s\n" % (legend, value))
                elif fmt in (NTP_UINT, NTP_INT, NTP_FLOAT):
                    if self.showunits:
                        displayvalue = ntp.util.unitifyvar(rawvalue, name)
                    else:
                        displayvalue = value
                    self.say("%13s %13s\n" % (legend, displayvalue))
                elif fmt == NTP_LFP:
                    self.say("%13s %13s\n" % (
                        legend, ntp.ntpc.prettydate(value)))
                elif fmt == NTP_2BIT:
                    self.say("%13s %13s\n"
                             % (legend, ("00", "01", "10", "11")[value]))
                elif fmt == NTP_MODE:
                    modes = (
                        "unspec", "sym_active", "sym_passive", "client",
                        "server",
                        "broadcast", "control", "private", "bclient"
                    )
                    try:
                        self.say("%s  %s\n" % (legend, modes[value]))
                    except IndexError:
                        self.say("%s  %s%d\n" % (legend, "mode#", value))
                    except TypeError:
                        self.say("%s  mode: %s\n" % (legend, repr(value)))
                elif fmt == NTP_PACKETS:
                    self.say(
                        "{0:<13} {1[0]:>13} {1[1]:>10}{1[2]:<3}\n".format(
                            legend, ntp.util.packetize(value, run)
                        )
                    )
                elif fmt == NTP_UPTIME:
                    run, display = ntp.util.periodize(value)
                    self.say("{0:<13} {1:>13}\n".format(legend, display))
                else:
                    self.warn("unexpected vc type %s for %s, value %s"
                              % (fmt, name, value))
        except KeyboardInterrupt:
            self.warn("display interrupted")

    def do_delay(self, line):
        "set the delay added to encryption time stamps"
        if not line:
            self.say("delay %d ms\n" % self.auth_delay)
        else:
            try:
                self.auth_delay = int(line)
                if self.auth_delay < 0:
                    raise ValueError
            except ValueError:
                self.say("Huh?")

    def help_delay(self):
        self.say("""\
function: set the delay added to encryption time stamps
usage: delay [ msec ]
""")

    def do_host(self, line):
        "specify the host whose NTP server we talk to"
        if not line:
            if self.session.havehost():
                self.say("current host is %s\n" % self.session.hostname)
            else:
                self.say("no current host\n")
        else:
            tokens = line.split()
            if tokens[0] == '-4':
                session.ai_family = socket.AF_INET
                tokens.pop(0)
            elif tokens[0] == '-6':
                session.ai_family = socket.AF_INET6
                tokens.pop(0)
            try:
                if (tokens and
                        self.session.openhost(tokens[0], session.ai_family)):
                    self.say("current host set to %s\n"
                             % self.session.hostname)
                elif self.session.havehost():
                    self.say("current host remains %s\n"
                             % self.session.hostname)
                else:
                    self.say("still no current host\n")
            except KeyboardInterrupt:
                self.warn("lookup interrupted")

    def help_host(self):
        self.say("""\
function: specify the host whose NTP server we talk to
usage: host [-4|-6] [hostname]
""")

    def do_poll(self, line):
        "poll an NTP server in client mode `n' times"
        # And it's not in the C version, so we're off the hook here
        self.warn("WARNING: poll not implemented yet")

    def help_poll(self):
        self.say("""\
function: poll an NTP server in client mode `n' times
usage: poll [n] [verbose]
""")

    def do_passwd(self, line):
        "specify a password to use for authenticated requests"
        try:
            self.session.password()
        except ntp.packet.ControlException as e:
            self.warn(e.message)
        except IOError:
            self.warn("***Can't read control key from /etc/ntp.conf")

    def help_passwd(self):
        self.say("""\
function: specify a password to use for authenticated requests
usage: passwd []
""")

    def do_hostnames(self, line):
        "specify whether hostnames or net numbers are printed"
        if not line:
            pass
        elif line == "yes":
            self.showhostnames = 1
        elif line == "no":
            self.showhostnames = 0
        elif line == 'hostnum':
            self.showhostnames = 2
        elif line == 'hostname':
            self.showhostnames = 3
        else:
            self.say("What?\n")
            pass
        if self.showhostnames & 1:
            self.say('resolved hostnames being shown\n')
        else:
            self.say('resolved hostnames not being shown\n')
        if self.showhostnames & 2:
            self.say('supplied hostnames being shown\n')
        else:
            self.say('supplied hostnames not being shown\n')

    def help_hostnames(self):
        self.say("""\
function: specify whether hostnames or net numbers are printed
usage: hostnames [yes|no|hostname|hostnum]
""")

    def do_debug(self, line):
        "set/change debugging level"
        if not line:
            pass
        elif line == "more":
            self.debug += 1
        elif line == "less":
            if self.debug > 0:
                self.debug -= 1
        elif line == "no":
            self.debug = 0
        else:
            try:
                self.debug = int(line)  # C version didn't implement this
            except ValueError:
                self.warn("What?")
        self.session.debug = self.debug
        self.say("debug level is %d\n" % self.debug)

    def do_logfile(self, line):
        """view/change logfile. \"<stderr>\" will log to stderr
           instead of a file"""
        if not line:
            self.say(repr(self.logfp.name) + "\n")
            return
        if self.logfp != sys.stderr:
            self.logfp.close()
        if line == "<stderr>":
            self.logfp = self.session.logfp = sys.stderr
        else:
            try:
                logfp = open(line, "a", 1)  # 1 => line buffered
                self.logfp = self.session.logfp = logfp
                self.say("Logfile set to %s\n" % line)
            except IOError:
                self.warn("Could not open %s for logging." % line)

    def help_debug(self):
        self.say("""\
function: set/change debugging level
usage: debug [no|more|less|n]
""")

    def do_exit(self, line):
        "exit ntpq"
        return True

    def help_exit(self):
        self.say("""\
function: exit ntpq
usage: exit
""")
    do_quit = do_exit

    def help_quit(self):
        self.say("""\
function: exit ntpq
usage: quit
""")

    def do_keyid(self, line):
        "set keyid to use for authenticated requests"
        if line:
            try:
                self.session.keyid = int(line)
            except ValueError:
                self.warn("What?")
        if self.session.keyid is None:
            self.say("no keyid defined\n")
        else:
            self.say("keyid is %d\n" % self.session.keyid)

    def help_keyid(self):
        self.say("""\
function: set keyid to use for authenticated requests
usage: keyid [key#]
""")

    def do_version(self, line):
        "print version number"
        self.say(version + "\n")

    def help_version(self):
        self.say("""\
function: print version number
usage: version
""")

    def do_direct(self, line):
        "toggle direct mode output"
        self.directmode = not self.directmode
        if self.directmode:
            self.say("Direct mode is on\n")
        else:
            self.say("Direct mode is off\n")

    def help_direct(self):
        self.say("""\
function: toggle direct-mode MRU output
usage: direct
""")

    def do_raw(self, line):
        "do raw mode variable output"
        self.rawmode = True
        self.say("Output set to raw\n")

    def help_raw(self):
        self.say("""\
function: do raw mode variable output
usage: raw
""")

    def do_cooked(self, line):
        "do cooked mode variable output"
        self.rawmode = False
        self.say("Output set to cooked\n")

    def help_cooked(self):
        self.say("""\
function: do cooked mode variable output
usage: cooked
""")

    def do_authenticate(self, line):
        "always authenticate requests to this server"
        if not line:
            pass
        elif line == "yes":
            self.session.always_auth = True
        elif line == "no":
            self.session.always_auth = False
        else:
            self.warn("What?")
        if self.session.always_auth:
            self.say("authenticated requests being sent\n")
        else:
            self.say("unauthenticated requests being sent\n")

    def help_authenticate(self):
        self.say("""\
function: always authenticate requests to this server
usage: authenticate [yes|no]
""")

    def do_ntpversion(self, line):
        "set the NTP version number to use for requests"
        if not line:
            pass
        else:
            try:
                newversion = int(line)
                if (newversion >= ntp.magic.NTP_OLDVERSION and
                        newversion <= ntp.magic.NTP_VERSION):
                    self.pktversion = newversion
                else:
                    self.warn("versions %d to %d, please"
                              % (ntp.magic.NTP_OLDVERSION,
                                 ntp.magic.NTP_VERSION))
            except ValueError:
                self.warn("What?")
        self.say("NTP version being claimed is %d\n" % self.pktversion)

    def help_ntpversion(self):
        self.say("""\
function: set the NTP version number to use for requests
usage: ntpversion [version number]
""")

    def do_keytype(self, line):
        "set key type to use for authenticated requests"
        if not line:
            self.say("Keytype: %s\n" % self.session.keytype)
        elif line.upper() in ['AES', 'AES128CMAC']:
            self.session.keytype = 'AES-128'
        elif line.upper() in ['SHA-1']:
            self.session.keytype = 'SHA1'
        elif not ntp.ntpc.checkname(line.upper()):
            self.warn("Keytype %s is not supported by openSSL or ntpq.\n" % line)
        else:
            self.session.keytype = line.upper()

    def help_keytype(self):
        self.say("""\
function: set key type to use for authenticated requests, one of:
    DSA, MD4, MD5, MDC2, RIPEMD160, SHA-1, AES-CMAC
usage: keytype [digest-name]
""")

    def do_associations(self, line):
        "print list of association IDs and statuses for the server's peers"
        if self.__dogetassoc():
            self.__printassoc(showall=True)

    def help_associations(self):
        self.say("""\
function: print list of association IDs and statuses for the server's peers
usage: associations
""")

    def do_passociations(self, line):
        "print list of associations returned by last associations command"
        self.__printassoc(showall=True)

    def help_passociations(self):
        self.say("""\
function: print list of associations returned by last associations command
usage: passociations
""")

    def do_lassociations(self, line):
        "print list of associations including all client information"
        if self.__dogetassoc():
            self.__printassoc(showall=True)

    def help_lassociations(self):
        self.say("""\
function: print list of associations including all client information
usage: lassociations
""")

    def do_lpassociations(self, line):
        """\
print last obtained list of associations, including client information
"""
        self.__printassoc(showall=True)

    def help_lpassociations(self):
        self.say("""\
function: print last obtained list of associations, including
          client information
usage: lpassociations
""")

    def do_addvars(self, line):
        "add variables to the variable list or change their values"
        if not line:
            self.warn("usage: addvars name[=value][,...]\n")
            return
        vars_to_add = line.split(',')
        for add_var in vars_to_add:
            try:
                (name, val) = add_var.split("=")
            except ValueError:
                (name, val) = (add_var, "")
            self.uservars[name.strip()] = val.strip()

    def help_addvars(self):
        self.say("""\
function: add variables to the variable list or change their values
usage: addvars name[=value][,...]
""")

    def do_rmvars(self, line):
        "remove variables from the variable list"
        if not line:
            self.warn("usage: rmvars name[,...]\n")
            return
        vars_to_rm = line.split(',')
        for rm_var in vars_to_rm:
            if rm_var not in self.uservars:
                self.warn("%s is not in the variable list" % rm_var)
            else:
                del self.uservars[rm_var]

    def help_rmvars(self):
        self.say("""\
function: remove variables from the variable list
usage: rmvars name[,...]
""")

    def do_clearvars(self, line):
        "remove all variables from the variable list"
        self.uservars.clear()

    def help_clearvars(self):
        self.say("""\
function: remove all variables from the variable list
usage: clearvars
""")

    def do_showvars(self, line):
        "print variables on the variable list"
        if not self.uservars:
            self.say("No variables on list.\n")
        for (name, value) in self.uservars.items():
            if value:
                self.say("%s=%s\n" % (name, value))
            else:
                self.say(name + "\n")

    def help_showvars(self):
        self.say("""\
function: print variables on the variable list
usage: showvars
""")

    def do_readlist(self, line):
        "read the system or peer variables included in the variable list"
        associd = self.__assoc_valid(line)
        if associd >= 0:
            qtype = ntp.ntpc.TYPE_SYS if associd == 0 else ntp.ntpc.TYPE_PEER
            self.__dolist(self.uservars.keys(), associd,
                          ntp.control.CTL_OP_READVAR, qtype)

    def help_readlist(self):
        self.say("""\
function: read the system or peer variables included in the variable list
usage: readlist [assocID]
""")

    def do_rl(self, line):
        "read the system or peer variables included in the variable list"
        self.do_readlist(line)

    def help_rl(self):
        self.say("""\
function: read the system or peer variables included in the variable list
usage: rl [assocID]
""")

    def do_writelist(self, line):
        "write the system or peer variables included in the variable list"
        pass

    def help_writelist(self):
        self.say("""\
function: write the system or peer variables included in the variable list
usage: writelist [ assocID ]
""")

    def do_readvar(self, line):
        "read system or peer variables"
        associd = self.__assoc_valid(line)
        if associd >= 0:
            qtype = ntp.ntpc.TYPE_SYS if associd == 0 else ntp.ntpc.TYPE_PEER
            silence = bool(len(line.split()) >= 2)
            # Some scripts written for C ntpq need associd printed here
            self.__dolist(line.split()[1:], associd,
                          ntp.control.CTL_OP_READVAR, qtype, quiet=silence)

    def help_readvar(self):
        self.say("""\
function: read system or peer variables
usage: readvar [assocID] [varname1] [varname2] [varname3]
""")

    def do_rv(self, line):
        "read system or peer variables"
        self.do_readvar(line)

    def help_rv(self):
        self.say("""\
function: read system or peer variables
usage: rv [assocID] [varname1] [varname2] [varname3]
""")

    def do_writevar(self, line):
        "write system or peer variables"
        pass

    def help_writevar(self):
        self.say("""\
function: write system or peer variables
usage: writevar assocID name=value,[...]
""")

    def do_mreadlist(self, line):
        "read the peer variables in the variable list for multiple peers"
        if not line:
            self.warn("usage: mreadlist assocIDlow assocIDhigh\n")
            return
        idrange = self.__assoc_range_valid(line)
        if not idrange:
            return
        for associd in idrange:
            if associd != idrange[0]:
                self.say("\n")
            if not self.__dolist(self.uservars,
                                 associd, ntp.control.CTL_OP_READVAR,
                                 ntp.ntpc.TYPE_PEER):
                return

    def help_mreadlist(self):
        self.say("""\
function: read the peer variables in the variable list for multiple peers
usage: mreadlist assocIDlow assocIDhigh
""")

    def do_mrl(self, line):
        "read the peer variables in the variable list for multiple peers"
        if not line:
            self.warn("usage: mrl assocIDlow assocIDhigh")
            return
        self.do_mreadlist(line)

    def help_mrl(self):
        self.say("""\
function: read the peer variables in the variable list for multiple peers
usage: mrl assocIDlow assocIDhigh
""")

    def do_mreadvar(self, line):
        "read peer variables from multiple peers"
        if not line:
            self.warn("usage: mreadvar assocIDlow assocIDhigh  "
                      "[ name=value[,...] ]")
            return
        idrange = self.__assoc_range_valid(line)
        if not idrange:
            return
        varlist = line.split()[2:]
        for associd in idrange:
            if associd != idrange[0]:
                self.say("\n")
            if not self.__dolist(varlist, associd,
                                 ntp.control.CTL_OP_READVAR,
                                 ntp.ntpc.TYPE_PEER):
                return

    def help_mreadvar(self):
        self.say("""\
function: read peer variables from multiple peers
usage: mreadvar assocIDlow assocIDhigh [name=value[,...]]
""")

    def do_mrv(self, line):
        "read peer variables from multiple peers"
        if not line:
            self.warn(
                "usage: mrv assocIDlow assocIDhigh [name=value[,...]]")
            return
        self.do_mreadvar(line)

    def help_mrv(self):
        self.say("""\
function: read peer variables from multiple peers
usage: mrv assocIDlow assocIDhigh [name=value[,...]]
""")

    def do_clocklist(self, line):
        "read the clock variables included in the variable list"
        assoc = self.__assoc_valid(line)
        if assoc >= 0:
            self.__dolist(self.uservars.keys(),
                          assoc, ntp.control.CTL_OP_READCLOCK,
                          ntp.ntpc.TYPE_CLOCK)

    def help_clocklist(self):
        self.say("""\
function: read the clock variables included in the variable list
usage: clocklist [assocID]
""")

    def do_cl(self, line):
        "read the clock variables included in the variable list"
        self.do_clocklist(line)

    def help_cl(self):
        self.say("""\
function: read the clock variables included in the variable list
usage: cl [assocID]
""")

    def do_clockvar(self, line):
        "read clock variables"
        assoc = self.__assoc_valid(line)
        if assoc == 0:
            self.warn("This command requires the association ID of a clock.")
        elif assoc > 0:
            self.__dolist(line.split()[1:], assoc,
                          ntp.control.CTL_OP_READCLOCK, ntp.ntpc.TYPE_CLOCK)

    def help_clockvar(self):
        self.say("""\
function: read clock variables
usage: clockvar [assocID] [name=value[,...]]
""")

    def do_cv(self, line):
        "read clock variables"
        self.do_clockvar(line)

    def help_cv(self):
        self.say("""\
function: read clock variables
usage: cv [ assocID ] [ name=value[,...] ]
""")

    def do_pstats(self, line):
        "show statistics for a peer"
        pstats = (
            ("srcadr", "remote host:          ", NTP_ADD),
            ("dstadr", "local address:        ", NTP_ADD),
            ("timerec", "time last received:   ", NTP_UPTIME),
            ("timer", "time until next send: ", NTP_UPTIME),
            ("timereach", "reachability change:  ", NTP_INT),
            ("sent", "packets sent:         ", NTP_INT),
            ("received", "packets received:     ", NTP_INT),
            ("badauth", "bad authentication:   ", NTP_INT),
            ("bogusorg", "bogus origin:         ", NTP_INT),
            ("oldpkt", "duplicate:            ", NTP_INT),
            ("seldisp", "bad dispersion:       ", NTP_INT),
            ("selbroken", "bad reference time:   ", NTP_INT),
            ("candidate", "candidate order:      ", NTP_INT),
            ("ntscookies", "count of nts cookies: ", NTP_INT),
        )
        if not line:
            self.warn("usage: pstats assocID")
            return
        associd = self.__assoc_valid(line)
        if associd >= 0:
            self.collect_display(associd=associd,
                                 variables=pstats, decodestatus=True)

    def help_pstats(self):
        self.say("""\
function: show statistics for a peer
usage: pstats assocID
""")

    def do_peers(self, line):
        "obtain and print a list of the server's peers [IP version]"
        self.__dopeers(showall=True, mode="peers")

    def help_peers(self):
        self.say("""\
function: obtain and print a list of the server's peers [IP version]
usage: peers
""")

    def do_rpeers(self, line):
        "obtain and print a list of the server's peers (dextral)"
        self.__dopeers(showall=True, mode="rpeers")

    def help_rpeers(self):
        self.say("""\
function: obtain and print a list of the server's peers (dextral)
usage: rpeers
""")

    def do_apeers(self, line):
        """
obtain and print a list of the server's peers and their assocIDs [IP version]
"""
        self.__dopeers(showall=True, mode="apeers")

    def help_apeers(self):
        self.say("""\
function: obtain and print a list of the server's peers and their
          assocIDs [IP version]
usage: apeers
""")

    def do_lpeers(self, line):
        "obtain and print a list of all peers and clients [IP version]"
        self.__dopeers(showall=True, mode="peers")

    def help_lpeers(self):
        self.say("""\
function: obtain and print a list of all peers and clients [IP version]
usage: lpeers
""")

    def do_opeers(self, line):
        """
print peer list the old way, with dstadr shown rather than refid [IP version]
"""
        self.__dopeers(showall=True, mode="opeers")

    def help_opeers(self):
        self.say("""\
function: print peer list the old way, with dstadr shown rather than
          refid [IP version]
usage: opeers
""")

    def do_lopeers(self, line):
        """obtain and print a list of all peers and clients showing
        dstadr [IP version]"""
        self.__dopeers(showall=True, mode="opeers")

    def help_lopeers(self):
        self.say("""\
function: obtain and print a list of all peers and clients showing
          dstadr [IP version]
usage: lopeers
""")

    def do_hot_config(self, line):
        "send a remote configuration command to ntpd"
        try:
            self.session.password()
        except ntp.packet.ControlException as e:
            self.warn(e.message)
            return
        except IOError:
            self.warn("***Can't read control key from /etc/ntp.conf")
            return
        if self.debug > 2:
            self.warn("In Config\nKeyword = :config\nCommand = %s" % line)
        try:
            self.session.config(line)
            self.session.response = ntp.poly.polystr(self.session.response)
            m = re.match("column ([0-9]+) syntax error", self.session.response)
            if m:
                col = int(m.group(1))
                if col >= 0 and col <= len(line):
                    if self.interactive:
                        self.say("_" * (len(self.prompt) + 2 + col))
                    else:
                        self.say(line + "\n")
                    self.say("_" * (col - 1))
                self.say("^\n")
            self.say(self.session.response + "\n")
        except ntp.packet.ControlException as e:
            self.warn(e.message)

    def help_hot_config(self):
        self.say("""\
function: send a remote configuration command to ntpd
usage: config <configuration command line>
""")

    def do_config_from_file(self, line):
        "configure ntpd using the configuration filename"
        try:
            with open(line) as rfp:
                self.say("%s\n" % self.session.config(rfp.read()))
        except IOError:
            self.warn("Could not read %s" % line)

    def help_config_from_file(self):
        self.say("""\
function: configure ntpd using the configuration filename
usage: config_from_file <configuration filename>
""")

    def printdirect(self, entries):
        for entry in entries:
            self.say(self.formatter.summary(entry) + "\n")

    def do_noflake(self):
        """Disables the dropping of control packets by ntpq for testing."""
        self.session.flakey = False

    def help_noflake(self):
        """Print help for noflake."""
        self.say("""\
function: Disables the dropping of received packets by ntpq for testing.
usage: noflake
""")

    def do_doflake(self, line):
        """Drop some received packets for testing.

        Probabilities 0 and 1 should be certainly accepted
        and discarded respectively. No default, but 0.1
        should be a one in ten loss rate.
        """
        try:
            _ = float(line)
            if not 0 < _ < 1:
                raise ValueError
            self.session.flakey = _
        except ValueError:
            self.say('Flakiness must be a (positive) float less than 1.')

    def help_doflake(self):
        """Print help for doflake."""
        self.say("""\
function: Enables the dropping of control packets by
ntpq for testing. Probabilities 0 and 1 should be
certainly accepted and discarded respectively. No
default, but 0.1 should be a one in ten loss rate.
usage: doflake <probability>
""")

    def do_mrulist(self, line):
        """display the list of most recently seen source addresses,
           tags mincount=... resall=0x... resany=0x..."""
        cmdvars = {}
        for item in line.split(" "):
            if not item:
                continue
            if '=' not in item:
                cmdvars[item] = True
            else:
                eq = item.index("=")
                var = item[:eq].strip()
                val = item[eq+1:].strip()
                try:
                    val = int(val, 0)
                except ValueError:
                    try:
                        val = float(val)
                    except ValueError:
                        if val[0] == '"' and val[-1] == '"':
                            val = val[1:-1]
                cmdvars[var] = val

        if not self.directmode:
            self.say("Ctrl-C will stop MRU retrieval and display "
                     "partial results.\n")
        if self.rawmode:
            mruhook = lambda v: self.printvars(variables=v,
                                               dtype=ntp.ntpc.TYPE_SYS,
                                               quiet=True)
        else:
            mruhook = None
        try:
            formatter = ntp.util.MRUSummary(interpreter.showhostnames,
                                            wideremote=True)
            if self.directmode:
                formatter.now = None
            self.formatter = formatter
            if session.debug:
                formatter.logfp = session.logfp
                formatter.debug = session.debug
            self.session.slots = 0
            self.session.start = time.time()
            direct = self.printdirect if self.directmode else None
            span = self.session.mrulist(variables=cmdvars,
                                        rawhook=mruhook, direct=direct)
            if not self.directmode and not self.rawmode:
                if not span.is_complete():
                    self.say("mrulist retrieval interrupted by operator.\n"
                             "Displaying partial client list.\n")
                    span.now = time.time()
                try:
                    delta1 = time.time() - self.session.start
                    self.say(ntp.util.MRUSummary.header + "\n")
                    self.say(("=" * len(ntp.util.MRUSummary.header)) + "\n")
                    # reversed puts most recent entries at the top if no sort=
                    # see sort comments in pylib/packet.py
                    formatter.now = span.now
                    for entry in reversed(span.entries):
                        self.say(formatter.summary(entry) + "\n")
                    self.say("# Collected %d slots in %.3f seconds\n"
                             % (self.session.slots, delta1))
                except KeyboardInterrupt:
                    pass
            delta2 = time.time() - self.session.start
            self.say("# Processed %d slots in %.3f seconds\n"
                     % (self.session.slots, delta2))
            usage = resource.getrusage(resource.RUSAGE_SELF)
            rusage_denom = 1024.
            if sys.platform == 'darwin':
                # OSX uses bytes, while every other platform uses kilobytes
                rusage_denom = rusage_denom * rusage_denom
            self.say("# Used %d megabytes of memory\n"
                     % (usage.ru_maxrss/rusage_denom))
        except ntp.packet.ControlException as e:
            # Giving up after 8 restarts from the beginning.
            # With high-traffic NTP servers, this can occur if the
            # MRU list is limited to less than about 16 seconds' of
            # entries.  See the 'mru' ntp.conf entry.
            self.warn(e.message)

    def help_mrulist(self):
        self.say("""\
function: display the list of most recently seen source addresses,
          tags mincount=... resall=0x... resany=0x...
usage: mrulist [tag=value] [tag=value] [tag=value] [tag=value]
""")

    def do_ifstats(self, line):
        "show statistics for each local address ntpd is using"
        try:
            self.session.password()
            entries = self.session.ifstats()
            if self.rawmode:
                self.say(self.session.response + "\n")
            else:
                formatter = ntp.util.IfstatsSummary()
                self.say(ntp.util.IfstatsSummary.header)
                self.say(("=" * ntp.util.IfstatsSummary.width) + "\n")
                for (i, entry) in enumerate(entries):
                    self.say(formatter.summary(i, entry))
        except ntp.packet.ControlException as e:
            self.warn(e.message)
            return
        except IOError:
            self.warn("***Can't read control key from /etc/ntp.conf")

    def help_ifstats(self):
        self.say("""\
function: show statistics for each local address ntpd is using
usage: ifstats
""")

    def do_reslist(self, line):
        "show ntpd access control list"
        try:
            self.session.password()
            entries = self.session.reslist()
            if self.rawmode:
                self.say(self.session.response + "\n")
            else:
                formatter = ntp.util.ReslistSummary()
                self.say(ntp.util.ReslistSummary.header)
                self.say(("=" * ntp.util.ReslistSummary.width) + "\n")
                for entry in entries:
                    self.say(formatter.summary(entry))
        except ntp.packet.ControlException as e:
            self.warn(e.message)
            return
        except IOError:
            self.warn("***Can't read control key from /etc/ntp.conf")

    def help_reslist(self):
        self.say("""\
function: show ntpd access control list
usage: reslist
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_sysinfo(self, _line):
        "display system summary"
        sysinfo = (
            ("peeradr", "system peer:      ", NTP_ADP),
            ("peermode", "system peer mode: ", NTP_MODE),
            ("leap", "leap indicator:   ", NTP_2BIT),
            ("stratum", "stratum:          ", NTP_INT),
            ("precision", "log2 precision:   ", NTP_INT),
            ("rootdelay", "root delay:       ", NTP_FLOAT),
            ("rootdisp", "root dispersion:  ", NTP_FLOAT),
            ("rootdist", "root distance     ", NTP_FLOAT),
            ("refid", "reference ID:     ", NTP_STR),
            ("reftime", "reference time:   ", NTP_LFP),
            ("sys_jitter", "system jitter:    ", NTP_FLOAT),
            ("clk_jitter", "clock jitter:     ", NTP_FLOAT),
            ("clk_wander", "clock wander:     ", NTP_FLOAT),
            ("authdelay", "symm. auth. delay:", NTP_FLOAT),
        )
        self.collect_display(associd=0, variables=sysinfo, decodestatus=True)

    def help_sysinfo(self):
        self.say("""\
function: display system summary
usage: sysinfo
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_kerninfo(self, _line):
        "display kernel loop and PPS statistics"
        kerninfo = (
            ("koffset", "pll offset:          ", NTP_FLOAT),
            ("kfreq", "pll frequency:       ", NTP_FLOAT),
            ("kmaxerr", "maximum error:       ", NTP_FLOAT),
            ("kesterr", "estimated error:     ", NTP_FLOAT),
            ("kstflags", "kernel status:       ", NTP_STR),
            ("ktimeconst", "pll time constant:   ", NTP_INT),
            ("kprecis", "precision:           ", NTP_FLOAT),
            ("kfreqtol", "frequency tolerance: ", NTP_INT),
            ("kppsfreq", "pps frequency:       ", NTP_INT),
            ("kppsstab", "pps stability:       ", NTP_INT),
            ("kppsjitter", "pps jitter:          ", NTP_INT),
            ("kppscalibdur", "calibration interval ", NTP_INT),
            ("kppscalibs", "calibration cycles:  ", NTP_INT),
            ("kppsjitexc", "jitter exceeded:     ", NTP_INT),
            ("kppsstbexc", "stability exceeded:  ", NTP_INT),
            ("kppscaliberrs", "calibration errors:  ", NTP_INT),
        )
        self.collect_display(associd=0, variables=kerninfo, decodestatus=True)

    def help_kerninfo(self):
        self.say("""\
function: display kernel loop and PPS statistics
usage: kerninfo
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_sysstats(self, _line):
        "display system uptime and packet counts"
        sysstats = (
            ("ss_uptime",    "uptime:               ", NTP_UPTIME),
            ("ss_numctlreq", "control requests:     ", NTP_INT),
        )
        sysstats2 = (
            ("ss_reset",     "sysstats reset:       ", NTP_UPTIME),
            ("ss_received",  "packets received:     ", NTP_PACKETS),
            ("ss_thisver",   "current version:      ", NTP_PACKETS),
            ("ss_oldver",    "older version:        ", NTP_PACKETS),
            ("ss_ver1",      "NTPv1 total:          ", NTP_PACKETS),
            ("ss_ver1client"," NTPv1 clients:       ", NTP_PACKETS),
            ("ss_ver1zero",  " NTPv1 mode0:         ", NTP_PACKETS),
            ("ss_ver1symm",  " NTPv1 symm act:      ", NTP_PACKETS),
            ("ss_badformat", "bad length or format: ", NTP_PACKETS),
            ("ss_badauth",   "authentication failed:", NTP_PACKETS),
            ("ss_declined",  "declined:             ", NTP_PACKETS),
            ("ss_restricted","restricted:           ", NTP_PACKETS),
            ("ss_limited",   "rate limited:         ", NTP_PACKETS),
            ("ss_kodsent",   "KoD responses:        ", NTP_PACKETS),
            ("ss_processed", "processed for time:   ", NTP_PACKETS),
        )
        self.collect_display(associd=0, variables=sysstats, decodestatus=False)
        self.collect_display2(variables=sysstats2)

    def help_sysstats(self):
        self.say("""\
function: display system uptime and packet counts
usage: sysstats
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_monstats(self, _line):
        "display monitor (mrulist) counters and limits"
        monstats = (
            ("mru_enabled",     "enabled:              ", NTP_INT),
            ("mru_hashslots",   "hash slots in use:    ", NTP_INT),
            ("mru_depth",       "addresses in use:     ", NTP_INT),
            ("mru_deepest",     "peak addresses:       ", NTP_INT),
            ("mru_maxdepth",    "maximum addresses:    ", NTP_INT),
            ("mru_mindepth",    "reclaim above count:  ", NTP_INT),
            ("mru_maxage",      "reclaim maxage:       ", NTP_UPTIME),
            ("mru_minage",      "reclaim minage:       ", NTP_UPTIME),
            ("mru_mem",         "kilobytes:            ", NTP_INT),
            ("mru_maxmem",      "maximum kilobytes:    ", NTP_INT),
            ("mru_exists",      "alloc: exists:        ", NTP_INT),
            ("mru_new",         "alloc: new:           ", NTP_INT),
            ("mru_recycleold",  "alloc: recycle old:   ", NTP_INT),
            ("mru_recyclefull", "alloc: recycle full:  ", NTP_INT),
            ("mru_none",        "alloc: none:          ", NTP_INT),
            ("mru_oldest_age",  "age of oldest slot:   ", NTP_UPTIME),
        )
        self.collect_display(associd=0, variables=monstats, decodestatus=False)

    def help_monstats(self):
        self.say("""\
function: display monitor (mrulist) counters and limits
usage: monstats
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_authinfo(self, _line):
        "display symmetric authentication counters"
        authinfo = (
            ("authreset",          "time since reset:    ", NTP_UPTIME),
            ("authkeys",           "stored keys:         ", NTP_INT),
            ("authfreek",          "free keys:           ", NTP_INT),
            ("authklookups",       "key lookups:         ", NTP_INT),
            ("authknotfound",      "keys not found:      ", NTP_INT),
            ("authencrypts",       "encryptions:         ", NTP_PACKETS),
            ("authdigestencrypts", "digest encryptions:  ", NTP_PACKETS),
            ("authcmacencrypts",   "CMAC encryptions:    ", NTP_PACKETS),
            ("authdecrypts",       "decryptions:         ", NTP_PACKETS),
            ("authdigestdecrypts", "digest decryptions:  ", NTP_PACKETS),
            ("authdigestfails",    "digest failures:     ", NTP_PACKETS),
            ("authcmacdecrypts",   "CMAC decryptions:    ", NTP_PACKETS),
            ("authcmacfails",      "CMAC failures:       ", NTP_PACKETS),
            # Old variables no longer supported.
            # Interesting if looking at an old system.
            ("authkuncached",      "uncached keys:       ", NTP_INT),
            ("authkexpired",       "expired keys:        ", NTP_INT),
        )
        self.collect_display(associd=0, variables=authinfo, decodestatus=False)

    def help_authinfo(self):
        self.say("""\
function: display symmetric authentication counters
usage: authinfo
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_ntsinfo(self, _line):
        "display NTS authentication counters"
        ntsinfo = (
   ("nts_client_send",           "NTS client sends:           ", NTP_UINT),
   ("nts_client_recv_good",      "NTS client recvs good:      ", NTP_UINT),
   ("nts_client_recv_bad",       "NTS client recvs w error:   ", NTP_UINT),
   ("nts_server_recv_good",      "NTS server recvs good:      ", NTP_UINT),
   ("nts_server_recv_bad",       "NTS server recvs w error:   ", NTP_UINT),
   ("nts_server_send",           "NTS server sends:           ", NTP_UINT),
   ("nts_cookie_make",           "NTS make cookies:           ", NTP_UINT),
   ("nts_cookie_not_server",     "NTS cookies not server:     ", NTP_UINT),
   ("nts_cookie_decode_total",   "NTS decode cookies total:   ", NTP_UINT),
   ("nts_cookie_decode_current", " NTS decode cookies current:", NTP_UINT),
   ("nts_cookie_decode_old",     " NTS decode cookies old:    ", NTP_UINT),
   ("nts_cookie_decode_old2",    " NTS decode cookies old2:   ", NTP_UINT),
   ("nts_cookie_decode_older",   " NTS decode cookies older:  ", NTP_UINT),
   ("nts_cookie_decode_too_old", " NTS decode cookies too old:", NTP_UINT),
   ("nts_cookie_decode_error",   "NTS decode cookies error:   ", NTP_UINT),
  )
        ntskeinfo = (
   ("nts_ke_serves_good",        "NTS KE serves good:         ", NTP_UINT),
   ("nts_ke_serves_good_wall",   "NTS KE serves good wall:    ", NTP_FLOAT),
   ("nts_ke_serves_good_cpu",    "NTS KE serves good CPU:     ", NTP_FLOAT),
   ("nts_ke_serves_nossl",       "NTS KE serves no-TLS:       ", NTP_UINT),
   ("nts_ke_serves_nossl_wall",  "NTS KE serves no-TLS wall:  ", NTP_FLOAT),
   ("nts_ke_serves_nossl_cpu",   "NTS KE serves no-TLS CPU:   ", NTP_FLOAT),
   ("nts_ke_serves_bad",         "NTS KE serves bad:          ", NTP_UINT),
   ("nts_ke_serves_bad_wall",    "NTS KE serves bad wall:     ", NTP_FLOAT),
   ("nts_ke_serves_bad_cpu",     "NTS KE serves bad CPU:      ", NTP_FLOAT),
   ("nts_ke_probes_good",        "NTS KE client probes good:  ", NTP_UINT),
   ("nts_ke_probes_bad",         "NTS KE client probes bad:   ", NTP_UINT),
  )
        self.collect_display(associd=0, variables=ntsinfo, decodestatus=False)
        self.collect_display(associd=0, variables=ntskeinfo, decodestatus=False)

    def help_ntsinfo(self):
        self.say("""\
function: display NTS authentication counters
usage: ntsinfo
""")


    def do_mssntpinfo(self, _line):
        "display MSSNTP authentication counters"
        mssntpinfo = (
   ("mssntp_serves",              "MSSNTP requests processed:     ", NTP_UINT),
   ("mssntp_serves_no",           "MSSNTP rejected by samba:      ", NTP_UINT),
   ("mssntp_serves_err",          "MSSNTP errors talking to samba:", NTP_UINT),
   ("mssntp_serves_good",         "MSSNTP requests good:          ", NTP_UINT),
   ("mssntp_serves_good_wall",    "MSSNTP requests good wall:     ", NTP_FLOAT),
   ("mssntp_serves_good_slowest", "MSSNTP requests good slowest:  ", NTP_FLOAT),
   ("mssntp_serves_bad",          "MSSNTP requests bad:           ", NTP_UINT),
   ("mssntp_serves_bad_wall",     "MSSNTP requests bad wall:      ", NTP_FLOAT),
   ("mssntp_serves_bad_slowest",  "MSSNTP requests bad slowest:   ", NTP_FLOAT),
  )
        self.collect_display(associd=0, variables=mssntpinfo, decodestatus=False)

    def help_mssntpinfo(self):
        self.say("""\
function: display MSSNTP authentication counters
usage: mssntpinfo
""")


# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_iostats(self, _line):
        "display network input and output counters"
        iostats = (
            ("iostats_reset", "time since reset:     ", NTP_UPTIME),
            ("total_rbuf", "receive buffers:      ", NTP_INT),
            ("free_rbuf", "free receive buffers: ", NTP_INT),
            ("used_rbuf", "used receive buffers: ", NTP_INT),
            ("rbuf_lowater", "low water refills:    ", NTP_INT),
            ("io_dropped", "dropped packets:      ", NTP_PACKETS),
            ("io_ignored", "ignored packets:      ", NTP_PACKETS),
            ("io_received", "received packets:     ", NTP_PACKETS),
            ("io_sent", "packets sent:         ", NTP_PACKETS),
            ("io_sendfailed", "packet send failures: ", NTP_PACKETS),
            ("io_wakeups", "input wakeups:        ", NTP_INT),
            ("io_goodwakeups", "useful input wakeups: ", NTP_INT),
        )
        self.collect_display(associd=0, variables=iostats, decodestatus=False)

    def help_iostats(self):
        self.say("""\
function: display network input and output counters
usage: iostats
""")

# FIXME: This table should move to ntpd
#          so the answers track when ntpd is updated
    def do_timerstats(self, line):
        "display interval timer counters"
        timerstats = (
            ("timerstats_reset", "time since reset:  ", NTP_UPTIME),
            ("timer_overruns", "timer overruns:    ", NTP_INT),
            ("timer_xmts", "calls to transmit: ", NTP_INT),
        )
        self.collect_display(associd=0, variables=timerstats,
                             decodestatus=False)

    def help_timerstats(self):
        self.say("""\
function: display interval timer counters
usage: timerstats
""")


# Default values we use.
DEFHOST = "localhost"    # default host name

#
# main - parse arguments and handle options
#

usage = '''
USAGE: ntpq [-46dphinOV] [-c str] [-D lvl] [host ...]
  Flg Arg Option-Name    Description
   -4 no  ipv4            Force IPv4 DNS name resolution
                                - prohibits the option 'ipv6'
   -6 no  ipv6            Force IPv6 DNS name resolution
                                - prohibits the option 'ipv4'
   -a Num authentication  Enable authentication with the numbered key
   -c Str command         Run a command and exit
                                - may appear multiple times
   -d no  debug-level     Increase output debug message level
                                - may appear multiple times
   -l Str logfile         Logs debug messages to the provided filename
   -D Int set-debug-level Set the output debug message level
                                - may appear multiple times
   -h no  help            Print a usage message.
   -p no  peers           Print a list of the peers
   -n no  numeric         Numeric host addresses
   -k Str keyfile         Specify a keyfile. ntpq will look in this file
                          for the key specified with -a
   -V opt version         Output version information and exit
   -w no  wide            Enable wide display of addresses / hosts
                          on a separate line
   -W Num width           Force output width to this value instead of
                          querying the terminal size
   -u no  units           Display time with units.
'''

if __name__ == '__main__':
    bin_ver = "ntpsec-@NTPSEC_VERSION_EXTENDED@"
    ntp.util.stdversioncheck(bin_ver)
    try:
        (options, arguments) = getopt.getopt(
            sys.argv[1:],
            "46a:c:dD:hk:npsSVwW:ul:",
            ["ipv4", "ipv6", "authentication=",
             "command=", "debug", "set-debug-level=",
             "help", "keyfile", "numeric", "peers",
             "version", "wide", "width=", "units",
             "logfile=", "srcname", "srcnumber"])
    except getopt.GetoptError as e:
        sys.stderr.write("%s\n" % e)
        sys.stderr.write(usage)
        raise SystemExit(1)
    progname = os.path.basename(sys.argv[0])
    ntp.ntpc.setprogname(progname)

    session = ntp.packet.ControlSession()
    interpreter = Ntpq(session)

    keyid = keyfile = None
    logfp = sys.stderr

    for (switch, val) in options:
        if switch in ("-4", "--ipv4"):
            interpreter.ai_family = socket.AF_INET
        elif switch in ("-6", "--ipv6"):
            interpreter.ai_family = socket.AF_INET6
        elif switch in ("-a", "--authentication"):
            errmsg = "Error: -a parameter '%s' not a number\n"
            keyid = ntp.util.safeargcast(val, int, errmsg, usage)
        elif switch in ("-c", "--command"):
            interpreter.ccmds.append(val)
        elif switch in ("-d", "--debug"):
            interpreter.debug += 1
            session.debug += 1
        elif switch in ("-D", "--set-debug-level"):
            errmsg = "Error: -D parameter '%s' not a number\n"
            cast = ntp.util.safeargcast(val, int, errmsg, usage)
            session.debug = interpreter.debug = cast
        elif switch in ("-h", "--help"):
            sys.stderr.write(usage)
            raise SystemExit(0)
        elif switch in ("-n", "--numeric"):
            interpreter.showhostnames = 0
        elif switch in ("-p", "--peers"):
            interpreter.ccmds.append("peers")
        elif switch in ("-k", "--keyfile"):
            keyfile = val
        elif switch in ("-s", "--srcname"):
            interpreter.showhostnames = 3
        elif switch in ("-S", "--srcnumber"):
            interpreter.showhostnames = 2
        elif switch in ("-V", "--version"):
            sys.stdout.write("ntpq %s\n" % version)
            raise SystemExit(0)
        elif switch in ("-w", "--wide"):
            interpreter.wideremote = True
        elif switch in ("-W", "--width"):
            errmsg = "Error: -W parameter '%s' not a number\n"
            interpreter.termwidth = ntp.util.safeargcast(val, int,
                                                         errmsg, usage)
        elif switch in ("-u", "--units"):
            interpreter.showunits = True
        elif switch in ("-l", "--logfile"):
            if logfp != sys.stderr:
                logfp.close()
            logfp = open(val, "a", 1)  # 1 => line buffered

    session.logfp = interpreter.logfp = logfp

    if ntp.poly.forced_utf8 and interpreter.debug:
        interpreter.warn("\nforced UTF-8 output\n")

    if keyfile is not None:  # Have a -k, setup the auth
        credentials = None
        try:
            credentials = ntp.packet.Authenticator(keyfile)
        except (OSError, IOError):
            sys.stderr.write("ntpq: %s nonexistent or unreadable" % keyfile)
            raise SystemExit(1)
        if credentials:
            session.auth = credentials
    if keyid is not None:  # Have an -a
        session.keyid = keyid

    for token in arguments:
        if token.startswith("-"):
            if '4' == token[-1]:
                session.ai_family = socket.AF_INET
            elif '6' == token[-1]:
                session.ai_family = socket.AF_INET6
            else:
                interpreter.warn("%s: unexpected option-like thing."
                                 % progname)
                raise SystemExit(1)
            arguments.pop(0)
        else:
            interpreter.chosts.append((token, session.ai_family))

    if not arguments:
        interpreter.chosts.append((DEFHOST, session.ai_family))

    if (not interpreter.ccmds and
            not interpreter.interactive and
            os.isatty(0) and
            os.isatty(1)):
        interpreter.interactive = True

    try:
        if not interpreter.ccmds:
            if len(interpreter.chosts) > 1:
                interpreter.warn(
                    "ntpq can only work interactively on one host.")
                interpreter.chosts = interpreter.chosts[:1]
            session.openhost(*interpreter.chosts[0])
            interpreter.cmdloop()
        else:
            latter = False
            for ihost in interpreter.chosts:
                if session.openhost(*ihost):
                    for command in interpreter.ccmds:
                        if latter:
                            print()
                        latter = True
                        interpreter.onecmd(interpreter.precmd(command))
                    session.close()
        raise SystemExit(0)
    except BrokenPipeError:
        # https://docs.python.org/3/library/signal.html#note-on-sigpipe
        devnull = os.open(os.devnull, os.O_WRONLY)
        os.dup2(devnull, sys.stdout.fileno())
        raise SystemExit(1)
    except (KeyboardInterrupt, EOFError):
        if os.isatty(0):
            interpreter.say("\n")
    except ntp.packet.ControlException as e:
        interpreter.warn(e.message)
    except IOError:
        sys.stderr.write("Bailing out...\n")
# end
