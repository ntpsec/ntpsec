#!/usr/bin/env python
# -*- coding: utf-8 -*-

# SPDX-License-Identifier: BSD-2-clause
'''\
Any keystroke causes a poll and update. Keystroke commands:

'a': Change peer display to apeers mode, showing association IDs.
'd': Toggle detail mode (some peer will be reverse-video highlighted when on).
'h': Display helpscreen.
'j': Select next peer (in select mode); arrow down also works.
'k': Select previous peer (in select mode); arrow up also works.
'm': Disable peers display, showing only MRU list
'n': Toggle display of hostnames (vs. IP addresses).
'o': Change peer display to opeers mode, showing destination address.
'p': Change peer display to default mode, showing refid.
'q': Cleanly terminate the program.
's': Toggle display of only reachable hosts (default is all hosts).
'u': Toggle display of units.
'w': Toggle wide mode.
'x': Cleanly terminate the program.
' ': Rotate through a/n/o/p display modes.
'+': Increase debugging level.  Output goes to ntpmon.log
'-': Decrease debugging level.
'?': Display helpscreen.
'''

from __future__ import print_function, division

import getopt
import re
import sys
import time

try:
    import ntp.control
    import ntp.magic
    import ntp.ntpc
    import ntp.packet
    import ntp.util
except ImportError as e:
    sys.stderr.write(
        "ntpmon: can't load Python NTP libraries -- check PYTHONPATH.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)

# This used to force UTF-8 encoding, but that breaks the readline system.
# Unfortunately sometimes sys.stdout.encoding lies about the encoding,
# so expect random false positives.
# LANG=C or LANG=POSIX refuse unicode when combined with curses
disableunicode = ntp.util.check_unicode()


try:
    import locale
except ImportError as e:
    sys.stderr.write(
        "ntpmon: can't find Python locale library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)

try:
    import curses
except ImportError as e:
    sys.stderr.write(
        "ntpmon: can't find Python curses library.\n")
    sys.stderr.write("%s\n" % e)
    sys.exit(1)

stdscr = None


def iso8601(t):
    "ISO8601 string from Unix time."
    return time.strftime("%Y-%m-%dT%H:%M:%S", time.localtime(t))


def statline(_peerlist, _mrulist, nyquist):
    "Generate a status line"
    # We don't use stdversion here because the presence of a date is confusing
    leader = sysvars['version'][0]
    leader = re.sub(r" \([^\)]*\)", "", leader)
    if span.entries:
        trailer = "Updated: %s (%s)" \
                  % (iso8601(int(ntp.ntpc.lfptofloat(span.entries[0].last))),
                     ntp.util.PeerSummary.prettyinterval(nyquist))
    else:
        trailer = ""
    spacer = ((peer_report.termwidth - 1) - len(leader) - len(trailer)) * " "
    return leader + spacer + trailer


def peer_detail(variables, showunits=False):
    "Show the things a peer summary doesn't, cooked slightly differently"
    # All of an rv display except refid, reach, delay, offset, jitter.
    # One of the goals here is to emit field values at fixed positions
    # on the 2D display, so that changes in the details are easier to spot.
    vcopy = {}
    vcopyraw = {}
    vcopy.update(variables)
    width = ntp.util.termsize().width - 2
    # Need to separate the casted from the raw
    for key in vcopy.keys():
        vcopyraw[key] = vcopy[key][1]
        vcopy[key] = vcopy[key][0]
    vcopy["leap"] = ("no-leap", "add-leap", "del-leap",
                     "unsync")[vcopy["leap"]]
    for fld in ('xmt', 'rec', 'reftime'):
        if fld not in vcopy:
            vcopy[fld] = "***missing***"
        else:
            vcopy[fld] = ntp.util.rfc3339(ntp.ntpc.lfptofloat(vcopy[fld]))
    if showunits:
        for name in ntp.util.MS_VARS:
            if name in vcopy:
                vcopy[name] = ntp.util.unitify(vcopyraw[name],
                                               ntp.util.UNIT_MS,
                                               width=None)
        for name in ntp.util.PPM_VARS:
            if name in vcopy:
                vcopy[name] = ntp.util.unitify(vcopyraw[name],
                                               ntp.util.UNIT_PPM,
                                               width=None)
        for name in ntp.util.S_VARS:
            if name in vcopy:
                vcopy[name] = ntp.util.unitify(vcopyraw[name],
                                               ntp.util.UNIT_S,
                                               width=None)
        vcopy['filtdelay'] = ntp.util.stringfiltcooker(vcopyraw['filtdelay'])
        vcopy['filtoffset'] = ntp.util.stringfiltcooker(vcopyraw['filtoffset'])
        vcopy['filtdisp'] = ntp.util.stringfiltcooker(vcopyraw['filtdisp'])
    else:
        vcopy['filtdelay'] = ntp.util.stringfilt(vcopyraw['filtdelay'])
        vcopy['filtoffset'] = ntp.util.stringfilt(vcopyraw['filtoffset'])
        vcopy['filtdisp'] = ntp.util.stringfilt(vcopyraw['filtdisp'])
    # annotate IPv6, to stand out from :port
    if ':' in vcopy['srcadr']:
        vcopy['srcadr'] = '[' + vcopy['srcadr'] + ']'
    if ':' in vcopy['dstadr']:
        vcopy['dstadr'] = '[' + vcopy['dstadr'] + ']'
    vcopy['adr'] = "dstadr=%(dstadr)s:%(dstport)s " \
                   "srcadr=%(srcadr)s:%(srcport)d" % vcopy
    if len(vcopy['adr']) > width:
        # too long, break the line
        vcopy['adr'] = vcopy['adr'].replace(" ", "\n")

    peerfmt = """\
%(adr)s
reftime=%(reftime)s\tleap=%(leap)s\trootdelay=%(rootdelay)s
    rec=%(rec)s\tstratum=%(stratum)2d\trootdisp=%(rootdisp)s
    xmt=%(xmt)s\tprecision=%(precision)3d\tdispersion=%(dispersion)s
unreach=%(unreach)d hmode=%(hmode)d pmode=%(pmode)d hpoll=%(hpoll)d \
ppoll=%(ppoll)d headway=%(headway)s flash=%(flash)s keyid=%(keyid)s
filtdelay  = %(filtdelay)s
filtoffset = %(filtoffset)s
filtdisp   = %(filtdisp)s
"""
    str = peerfmt % vcopy
    return str.expandtabs()


class Fatal(Exception):
    "Unrecoverable error."

    def __init__(self, msg):
        Exception.__init__(self)
        self.msg = msg

    def __str__(self):
        return self.msg


class OutputContext:
    def __enter__(self):
        "Begin critical region."
        if (sys.version_info[0] < 3) and (disableunicode is False):
            # This appears to only be needed under python 2, it is only
            # activated when we already have UTF-8. Otherwise we drop
            # down to non-unicode versions.
            locale.setlocale(locale.LC_ALL, "")
        global stdscr
        stdscr = curses.initscr()
        try:
            curses.curs_set(0)
        except curses.error:
            # VT100 terminal emulations can barf here.
            pass
        curses.cbreak()
        curses.noecho()
        stdscr.keypad(True)
        # Design decision: The most important info is nearer the
        # top of the display. Therefore, prevent scrolling.
        stdscr.scrollok(False)

    def __exit__(self, extype_unused, value_unused, traceback_unused):
        curses.endwin()


usage = '''
USAGE: ntpmon [-dhnuV] [-D lvl] [-l logfile] [host]
  Flg Arg Option-Name    Description
   -d no  debug-level     Increase output debug message level
                                - may appear multiple times
   -D Int set-debug-level Set the output debug message level
                                - may appear multiple times
   -h no  help            Print a usage message.
   -l Str logfile         Logs debug messages to the provided filename
   -n no  numeric         Numeric host addresses
   -u no  units           Display time with units.
   -V opt version         Output version information and exit
'''

if __name__ == '__main__':
    try:
        (options, arguments) = getopt.getopt(sys.argv[1:],
                                             "dD:hl:nuV",
                                             ["debug", "help", "logfile=",
                                              "numeric", "units",
                                              "set-debug-level=", "version"])
    except getopt.GetoptError as e:
        sys.stderr.write("%s\n" % e)
        sys.stderr.write(usage)
        raise SystemExit(1)
    progname = sys.argv[0]

    showhostnames = True
    wideremote = False
    showall = True
    showpeers = True
    showunits = False
    nyquist = 1
    debug = 0
    logfp = None
    defaultlog = "ntpmon.log"

    for (switch, val) in options:
        if switch in ("-d", "--debug"):
            debug += 1
        elif switch in ("-D", "--set-debug-level"):
            errmsg = "Error: -D parameter '%s' not a number\n"
            debug = ntp.util.safeargcast(val, int, errmsg, usage)
        elif switch in ("-h", "--help"):
            print(usage)
            raise SystemExit(0)
        elif switch in ("-l", "--logfile"):
            if logfp is not None:
                logfp.close()
            logfp = open(val, "a", 1)  # 1 => line buffered
        elif switch in ("-n", "--numeric"):
            showhostnames = False
        elif switch in ("-u", "--units"):
            showunits = True
        elif switch in ("-V", "--version"):
            print("ntpmon %s" % ntp.util.stdversion())
            raise SystemExit(0)

    if (logfp is None) and (debug > 0):
        logfp = open(defaultlog, "a", 1)

    poll_interval = 1
    helpmode = selectmode = detailmode = False
    selected = -1
    peer_report = ntp.util.PeerSummary(displaymode="peers",
                                       pktversion=ntp.magic.NTP_VERSION,
                                       showhostnames=showhostnames,
                                       wideremote=wideremote,
                                       showunits=showunits,
                                       termwidth=80,
                                       debug=debug,
                                       logfp=logfp)
    mru_report = ntp.util.MRUSummary(showhostnames,
                                     wideremote=wideremote,
                                     debug=debug, logfp=logfp)
    try:
        session = ntp.packet.ControlSession()
        session.debug = debug
        session.logfp = logfp
        session.openhost(arguments[0] if arguments else "localhost")
        sysvars = session.readvar(raw=True)
        with OutputContext() as ctx:
            while True:
                stdscr.clear()
                stdscr.addstr(0, 0, u"".encode('UTF-8'))
                if helpmode:
                    stdscr.addstr(__doc__.encode('UTF-8'))
                    tempStr = u"\nPress any key to resume monitoring"
                    stdscr.addstr(tempStr.encode('UTF-8'))
                    stdscr.refresh()
                    stdscr.timeout(-1)
                else:
                    if showpeers:
                        try:
                            peers = session.readstat()
                        except ntp.packet.ControlException as e:
                            raise Fatal(e.message)
                        except IOError as e:
                            raise Fatal(e.strerror)
                        strconvert = peer_report.header() + "\n"
                        stdscr.addstr(strconvert.encode('UTF-8'),
                                      curses.A_BOLD)
                    else:
                        peer_report.polls = [1]  # Kluge!
                        peers = []
                    if showpeers and len(peers) == 0:
                        raise Fatal("no peers reported")
                    try:
                        initphase = False
                        for (i, peer) in enumerate(peers):
                            if (not showall and not (
                                    ntp.control.CTL_PEER_STATVAL(peer.status) &
                                    (ntp.control.CTL_PST_CONFIG |
                                     ntp.control.CTL_PST_REACH))):
                                continue
                            try:
                                variables = session.readvar(peer.associd,
                                                            raw=True)
                            except ntp.packet.ControlException as e:
                                if e.errorcode == ntp.control.CERR_BADASSOC:
                                    # Probable race condition due to pool
                                    # dropping an associaton during refresh;
                                    # ignore. (GitLab issue #374.)
                                    break
                                raise Fatal(e.message + "\n")
                            except IOError as e:
                                raise Fatal(e.strerror)
                            except IndexError:
                                raise Fatal(
                                    "no 'hpoll' variable in peer response")
                            if not variables:
                                continue
                            if selectmode and selected == i:
                                retained = variables
                                hilite = curses.A_REVERSE
                            else:
                                hilite = curses.A_NORMAL
                            data = peer_report.summary(session.rstatus,
                                                       variables,
                                                       peer.associd)
                            data = data.encode('UTF-8')
                            stdscr.addstr(data, hilite)
                            if ('refid' in variables and
                                    'INIT' in variables['refid']):
                                initphase = True

                        # Now the MRU report
                        limit = stdscr.getmaxyx()[0] - len(peers)
                        span = session.mrulist(variables={'recent': limit})
                        mru_report.now = time.time()

                        # After init phase use Nyquist-interval
                        # sampling - half the smallest poll interval
                        # seen in the last cycle, rounded up to 1
                        # second.
                        if not initphase:
                            nyquist = int(min(peer_report.intervals()) / 2)
                            nyquist = 1 if nyquist == 0 else nyquist
                            ntp.util.dolog(logfp,
                                           "nyquist is %d\n" % nyquist,
                                           debug, 1)
                        # The status line
                        sl = statline(peer_report, mru_report, nyquist)
                        strconvert = sl + "\n"
                        stdscr.addstr(strconvert.encode('UTF-8'),
                                      curses.A_REVERSE | curses.A_DIM)
                        if detailmode:
                            if ntp.util.PeerSummary.is_clock(retained):
                                dtype = ntp.ntpc.TYPE_CLOCK
                            else:
                                dtype = ntp.ntpc.TYPE_PEER
                            sw = ntp.ntpc.statustoa(dtype,
                                                    peers[selected].status)
                            strconvert = "assoc=%d: %s\n"
                            stdscr.addstr(strconvert
                                          % (peers[selected].associd, sw))
                            strconvert = peer_detail(retained, showunits)
                            stdscr.addstr(strconvert.encode('UTF-8'))
                            try:
                                clockvars = session.readvar(
                                    peers[selected].associd,
                                    opcode=ntp.control.CTL_OP_READCLOCK,
                                    raw=True)
                                strconvert = ntp.util.cook(clockvars,
                                                           showunits, " ")
                                stdscr.addstr(strconvert.encode('UTF-8'))
                            except ntp.packet.ControlException as e:
                                pass
                        elif span.entries:
                            strconvert = ntp.util.MRUSummary.header + "\n"
                            stdscr.addstr(strconvert.encode('UTF-8'),
                                          curses.A_BOLD)
                            for entry in reversed(span.entries):
                                strcon = mru_report.summary(entry) + "\n"
                                stdscr.addstr(strcon.encode('UTF-8'))
                    except curses.error:
                        # An addstr overran the screen, no worries
                        pass
                    # Display all
                    stdscr.refresh()
                    stdscr.timeout(nyquist * 1000)
                try:
                    helpmode = False
                    key = stdscr.getkey()
                    if key == 'q' or key == 'x':
                        raise SystemExit(0)
                    elif key == 'a':
                        peer_report.displaymode = 'apeers'
                    elif key == 'd':
                        if not selectmode:
                            selected = 0
                        selectmode = not selectmode
                        detailmode = not detailmode
                        showpeers = True  # detail + hide peers == crash
                    elif key == 'm':
                        showpeers = not showpeers
                        detailmode = False  # detail + hide peers == crash
                    elif key == 'n':
                        peer_report.showhostnames = \
                            not peer_report.showhostnames
                        mru_report.showhostnames = not mru_report.showhostnames
                    elif key == 'o':
                        peer_report.displaymode = 'opeers'
                    elif key == 'p':
                        peer_report.displaymode = 'peers'
                    elif key == 's':
                        showall = not showall
                    elif key == 'u':
                        showunits = not showunits
                        peer_report.showunits = showunits
                    elif key == 'w':
                        wideremote = not wideremote
                        peer_report.wideremote = wideremote
                        mru_report.wideremote = wideremote
                    elif key == " ":
                        if peer_report.displaymode == 'peers':
                            peer_report.displaymode = 'apeers'
                        elif peer_report.displaymode == 'apeers':
                            peer_report.displaymode = 'opeers'
                        else:
                            peer_report.displaymode = 'peers'
                    elif key == 'j' or key == "KEY_DOWN":
                        if showpeers:
                            selected += 1
                            selected %= len(peers)
                    elif key == 'k' or key == "KEY_UP":
                        if showpeers:
                            selected += len(peers) - 1
                            selected %= len(peers)
                    elif key == '+':
                        if logfp is None:
                            logfp = open(defaultlog, "a", 1)
                            session.logfp = logfp
                            peer_report.logfp = logfp
                            mru_report.logfp = logfp
                        debug += 1
                        session.debug = debug
                        peer_report.debug = debug
                        mru_report.debug = debug
                    elif key == '-':
                        debug -= 1
                        session.debug = debug
                        peer_report.debug = debug
                        mru_report.debug = debug
                    elif key in ['?', 'h']:
                        helpmode = True
                except curses.error:
                    pass
    except KeyboardInterrupt:
        print("")
    except (Fatal, ntp.packet.ControlException) as e:
        print(e)
    except IOError:
        print("Bailing out...")

# end
