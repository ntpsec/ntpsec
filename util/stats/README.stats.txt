= Statistics file formats (README.stats.txt) =

The ntpd daemon can produce a variety of statistics files which are
useful for maintenance, evaluation and retrospective calibration
purposes. See the ntpd man page for instructions on how to configure
this feature. Since these files can become rather large and cumbersome,
they are ordinarily reduced to summary form by running the summary.sh
shell script once per day, week or month, as appropriate. There are
three file collections presently defined: peerstats, loopstats and
clockstats, each of which is described in this note.

Note: The NTPsec versions of these formats differ in that clock IDs
consist of a string drivername followed by unit number in parentheses
rather than the magic IP addresses used in NTP Classic.  The code can
be built in a Classic compatibility node that restores the old behavior.

== peerstats ==

The following data are collected in the peerstats files. The files are
reduced to summary data using the peer.sh shell script. See the peer.awk
script for further information. A line in the file is produced upon
reception of each valid update from a configured peer.

----------------------------------------------------------------------------
  49236 30.756 140.173.96.1 9474 0.000603 0.37532

  49236             modified Julian day number
  30.756            time of day (s) past midnight UTC
  140.173.96.1      peer identifier (IP address or receiver identifier)
  9474              peer status word (hex) (see NTP specification)
  0.000603          offset (s)
  0.08929           delay (s)
  0.37532           dispersion (s)
----------------------------------------------------------------------------

== loopstats ==

The following data are collected in the loopstats files. The files are
reduced to summary data using the loop.sh shell script. See the loop.awk
script for further information. A line in the file is produced at each
valid update of the local clock.

----------------------------------------------------------------------------
  49236 11.897 -0.000004 -35.9384 0.000003815 0.000805 0

  49236             modified Julian day number
  11.897            time of day (s) past midnight UTC
  -0.000004         time offset (s)
  -35.9384          frequency offset (ppm)
  0.000003815       jitter
  0.000805          wander (ppm)
  0                 phase-lock loop time constant
----------------------------------------------------------------------------

== clockstats ==

The following data are collected in the clockstats files. The files are
reduced to summary data using the clock.sh shell script, which also
updates the etf, itf and tdata data files as well. See the
clock.awk, etf.awk, itf.awk and tdta.awk scripts for
further information. A line in the file is produced at each valid update
received from a configured radio clock. Data are at present recorded for
several radios. The first part of each data line is similar for all
radios, e.g.:

----------------------------------------------------------------------------
  49234 60517.826 SPECTRACOM(1) 93 247 16:48:21.814

  49234                modified Julian day number
  60517.826            time of day (s) past midnight UTC
  SPECTRACOM(1)        receiver identifier (Spectracom unit 1)
  93 247 16:48:21.814  timecode (format varies)
----------------------------------------------------------------------------

The formats shown consist of one line with all the fields shown in
order. The timecode formats specific to each radio follow. See the
file README.timecodes for detailed information on the timecode formats
used by these radios.

=== Spectracom receiver ===

----------------------------------------------------------------------------
  49234 60517.826 SPECTRACOM(1) ?A93 247 16:48:21.814
----------------------------------------------------------------------------

  The '?' and 'A' characters are present only when the receiver is
  unsynchronized; otherwise, they are replaced by space ' ' characters.

=== IRIG audio decoder ===

----------------------------------------------------------------------------
  49234 60517.826 IRIG(0) 247 16:48:21?
----------------------------------------------------------------------------

  The '?' character is present only when the receiver is unsynchronized.


//end
