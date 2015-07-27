/*****************************************************************************

intercept.c - capture and replay logic for NTP environment calls

Think of ntpd as a complex finite-state machine for transforming a
stream of input events to output events.  Events are of the
following kinds:

1. Startup or termination.

2. Time reports from reference clocks.

3. Time calls to the host system clock.

4. Calls to the host's random-number generator.

5. Packets incoming from peer NTP daemons

6. Calls to adjtime/adjtimex to set the system clock.

7. Packets outgoing to peer NTP daemons.

We must support two modes of operation.  In "capture" mode, ntpd
operates normally, logging all events.  In "replay" mode, ntpd accepts
an event-capture log and replays it, processing all input events in a
previous capture.

We say that test performance is *stable* when replay mode is
idempotent - that is, replaying an event-capture log produces an exact
copy of itself by duplicating the same output events.

When test performance is stable, we know two things: (1) we have  
successfully captured all inputs of the system, and (2) the code
has experienced no functional regressions since the event capture.

We can regression-test the code by capturing logs of production
behavior and replaying them.  We can also hand-craft tests to probe
edge cases.  To support the latter case, it is highly desirable that the
event-capture format be a text stream in an eyeball-friendly,
readily-editable format.

Note that for test purposes we will not consider I/O traffic to and
from refclocks to be events.  In part this is a way to avoid the high
complexity of emulating the refclocks.  In part it reflects the
strategic decision to carve those drivers off of ntpd into a separate
refclockd communicating through an IPC driver (task REFCLOCKD). At a
later stage refclockd may have its own simulation mode.

== Implementation ==

ntpd needs two new switches: capture and replay.  The capture switch
says: to log all event calls to an event capture file in addition to
their normal behaviors.  This includes both read events (such as
refclock inputs) and write events (such as adjtimex calls).

The replay switch has more complex behavior. Interpret a capture
file. Mock all event calls with code that looks at each event
sequentially from the capture.  If a read call is performed, and the
next data in the log file is for that read call, the logged data is
returned for the call.  If a write call is performed, the call type and
call data is compared to the next log data.  If the next event doesn't
match the expected type or has different data, show the difference and
terminate - the replay failed.  Otherwise continue.

Replay succeeds if the event stream reaches the shutdown event with
no mismatches.

== Event file format ==

Simple is best.  Textual, one event per line.  Each line consists of a
verb optionally followed by argument fields (or is a comment, or
whitespace).  The verbs look like:

startup::
	Initialization event.  No argument fields.

refclock::
	Report from a reference clock.  Fields should be all possible data
        that a refclock may return.  Including, not limited to:  driver ID,
	refid, unit number, system time of measurement, refclock at time
        of measurement, precision, leap notify, etc.

systime::
	Report from the system clock: seconds part of time, fractional part of
	time.

random::
	Call to a random-number generator. One field, the number
	returned.

driftread:
	Read the drift file

driftwrite:
	Write the drift file.

receive::
	Receive packet. Field is some sort of textual packet dump.

adjtime::
	Adjust system clock.  Fields are the arguments.  May need an
	adjtimex variant for Linux.

send::
	Send packet. Field is some sort of textual packet dump.

shutdown::
	Termination. The single argument field is a return status.

*****************************************************************************/

#include <time.h>
#include <sys/time.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <signal.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "ntpd.h"
#include "ntp_io.h"
#include "ntp_control.h"
#include "ntp_unixtime.h"
#include "ntp_stdlib.h"
#include "ntp_config.h"
#include "ntp_crypto.h"
#include "ntp_assert.h"
#include "ntp_intercept.h"
#include "ntp_fp.h"

void intercept_log(const char *legend, ...)
{
}

void intercept_get_systime(const char *legend, l_fp *now)
{
	struct timespec ts;	/* seconds and nanoseconds */
	get_ostime(&ts);
	/* logging and replay goes here */
	normalize_time(ts, now);
}
