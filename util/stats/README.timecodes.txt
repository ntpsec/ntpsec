= Radio Timecode Formats (README.timecodes.txt) =

Following are examples of the serial timecode formats used by various
timecode receivers as given in the instruction manuals. These examples
are intended only for illustration and not as the basis of system
design. The following symbols are used to identify the timecode
character that begins a subfield. The values given after this symbol
represent the character offset from the beginning of the timecode string
as edited to remove control characters.

----------------------------------------------------------------------------
C         on-time character (start bit)
Y         year of century
T         time of day
D         day of year or month/day
A         alarm indicator (format specific)
Q         quality indicator (format specific)
<LF>      ASCII line feed (hex 0a)
<CR>      ASCII carriage return (hex 0d)
<SP>      ASCII space (hex 20)
----------------------------------------------------------------------------

In order to promote uniform behavior in the various implementations, it
is useful to have a common interpretation of alarm conditions and signal
quality. When the alarm indicator it on, the receiver is not operating
correctly or has never synchronized to the broadcast signal. When the
alarm indicator is off and the quality indicator is on, the receiver has
synchronized to the broadcast signal, then lost the signal and is
coasting on its internal oscillator.

In the following uppercase letters, punctuation marks and spaces <SP>
stand for themselves; lowercase letters stand for fields as described.
Special characters other than <LF>, <CR> and <SP> are preceded by ^.

== Spectracom 8170 and Netclock/2 (format 0) ==

----------------------------------------------------------------------------
"<CR><LF>i  ddd hh:mm:ss  TZ=zz<CR><LF>"
 C       A  D   T

     poll: ?; offsets: Y = none, D = 3, T = 7, A = 0, Q = none
     i = synchronization flag (<SP> = in synch, ? = out synch)
     ddd = day of year
     hh:mm:ss = hours, minutes, seconds
     zz = timezone offset (hours from UTC)

     Note: alarm condition is indicated by other than <SP> at A, which
     occurs during initial synchronization and when received signal has
     been lost for about ten hours

     example: "   216 15:36:43  TZ=0"
               A  D   T
----------------------------------------------------------------------------

== Netclock/2 (format 2) ==

This format is used by several clocks from different vendors.

----------------------------------------------------------------------------
"<CR><LF>iqyy ddd hh:mm:ss.fff ld"
 C       AQY  D   T

     poll: ?; offsets: Y = 2, D = 5, T = 9, A = 0, Q = 1
     i = synchronization flag (<SP> = in synch, ? = out synch)
     q = quality indicator (<SP> < 1ms, A < 10 ms, B < 100 ms, C < 500
     ms, D > 500 ms)
     yy = year (as broadcast)
     ddd = day of year
     hh:mm:ss.fff = hours, minutes, seconds, milliseconds of day
     l = leap-second warning (L indicates leap at end of month)
     d = standard/daylight time indicator (<SP> standard, D daylight)

     Note: alarm condition is indicated by other than <SP> at A, which
     occurs during initial synchronization and when received signal has
     been lost for about ten hours; unlock condition is indicated by
     other than <SP> at Q, with time since last lock indicated by the
     letter code A < 13 min, B < 1.5 hr, C < 7 hr, D > 7 hr.

     example: "  92 216 15:36:43.640  D"
               AQ   D   T
----------------------------------------------------------------------------

== TrueTime 468-DC Satellite Synchronized Clock ==

Also aopplies to other TrueTime receivers 

----------------------------------------------------------------------------
"<CR><LF><^A>ddd:hh:mm:ssq<CR>"
              D   T       QC

     poll: none; offsets: Y = none, D = 0, T = 4, A = 12, Q = 12
     hh:mm:ss = hours, minutes, seconds
     q = quality/alarm indicator (<SP> = locked, ? = alarm)

     Note: alarm condition is indicated by ? at A, which occurs during
     initial synchronization and when received signal is lost for an
     extended period; unlock condition is indicated by other than <SP>
     at Q

     example: "216:15:36:43 "
               D   T       Q
----------------------------------------------------------------------------

 David L. Mills
 University of Delaware
 mills@udel.edu
 23 October 1993
