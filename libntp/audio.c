/*
 * audio.c - audio interface for reference clock audio drivers
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#if defined(HAVE_SYS_AUDIOIO_H) || defined(HAVE_SUN_AUDIOIO_H) || \
    defined(HAVE_SYS_SOUNDCARD_H) || defined(HAVE_MACHINE_SOUNDCARD_H)

#include "audio.h"
#include "ntp_stdlib.h"
#include "ntp_syslog.h"
#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <stdio.h>
#include "ntp_string.h"

#ifdef HAVE_SYS_AUDIOIO_H
#include <sys/audioio.h>
#endif /* HAVE_SYS_AUDIOIO_H */

#ifdef HAVE_SUN_AUDIOIO_H
#include <sys/ioccom.h>
#include <sun/audioio.h>
#endif /* HAVE_SUN_AUDIOIO_H */

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */

#include <fcntl.h>

#ifdef HAVE_MACHINE_SOUNDCARD_H
# include <machine/soundcard.h>
# define PCM_STYLE_SOUND
#else
# ifdef HAVE_SYS_SOUNDCARD_H
#  include <sys/soundcard.h>
#  define PCM_STYLE_SOUND
# endif
#endif

/*
 * Global variables
 */
#ifdef HAVE_SYS_AUDIOIO_H
static struct audio_device device; /* audio device ident */
#endif /* HAVE_SYS_AUDIOIO_H */
#ifdef PCM_STYLE_SOUND
#else /* not PCM_STYLE_SOUND */
static struct audio_info info;	/* audio device info */
#endif /* not PCM_STYLE_SOUND */
static int ctl_fd;		/* audio control file descriptor */


/*
 * audio_init - open and initialize audio device
 *
 * This code works with SunOS 4.x and Solaris 2.x; however, it is
 * believed generic and applicable to other systems with a minor twid
 * or two. All it does is open the device, set the buffer size (Solaris
 * only), preset the gain and set the input port. It assumes that the
 * codec sample rate (8000 Hz), precision (8 bits), number of channels
 * (1) and encoding (ITU-T G.711 mu-law companded) have been set by
 * default.
 */
int
audio_init(
	char	*dname,		/* device name */
	int	bufsiz		/* buffer size */
	)
{
	int fd;
	int rval;
	char *actl =
#ifdef PCM_STYLE_SOUND
		"/dev/mixer"
#else
		"/dev/audioctl"
#endif
		;
#ifdef PCM_STYLE_SOUND
	struct snd_size s_size;
	snd_chan_param s_c_p;
#endif

	/*
	 * Open audio device. Do not complain if not there.
	 */
	fd = open(dname, O_RDWR | O_NONBLOCK, 0777);
	if (fd < 0)
		return (fd);

	/*
	 * Open audio control device.
	 */
	ctl_fd = open(actl, O_RDWR);
	if (ctl_fd < 0) {
		msyslog(LOG_ERR, "audio: invalid control device <%s>\n", actl);
		close(fd);
		return(ctl_fd);
	}

	/*
	 * Set audio device parameters.
	 */
#ifdef PCM_STYLE_SOUND
	printf("audio_init: <%s> bufsiz %d\n", dname, bufsiz);
	rval = fd;

	if (ioctl(fd, AIOGSIZE, &s_size) == -1)
	    printf("AIOGSIZE: %s\n", strerror(errno));
	else
	    printf("play_size %d, rec_size %d\n",
		s_size.play_size, s_size.rec_size);

	if (ioctl(fd, AIOGFMT, &s_c_p) == -1)
	    printf("AIOGFMT: %s\n", strerror(errno));
	else
	    printf("play_rate %lu, rec_rate %lu, play_format %#lx, rec_format %#lx\n",
		s_c_p.play_rate, s_c_p.rec_rate, s_c_p.play_format, s_c_p.rec_format);
#else /* not PCM_STYLE_SOUND */
	AUDIO_INITINFO(&info);
# ifdef HAVE_SYS_AUDIOIO_H
	info.record.buffer_size = bufsiz;
# endif /* HAVE_SYS_AUDIOIO_H */
	rval = ioctl(ctl_fd, (int)AUDIO_SETINFO, &info);
	if (rval < 0) {
		msyslog(LOG_ERR, "audio: invalid control device parameters\n");
		close(ctl_fd);
		close(fd);
		return(rval);
	}
	rval = fd;
#endif /* not PCM_STYLE_SOUND */
	return (rval);
}


/*
 * audio_gain - adjust codec gains and port
 */
int
audio_gain(
	int gain,		/* volume level (gain) 0-255 */
	int mongain,		/* input to output mix (monitor gain) 0-255 */
	int port		/* selected I/O port: 1 mic/2 line in */
	)
{
	int rval;
	static int done;

#ifdef PCM_STYLE_SOUND
	int l, r;

	rval = 0;

	r = l = 100 * gain / 255;	/* Normalize to 0-100 */
# ifdef DEBUG
	if (debug > 1)
	    printf("audio_gain: gain %d/%d\n", gain, l);
# endif
	l |= r << 8;
	rval = ioctl(ctl_fd, SOUND_MIXER_WRITE_IGAIN, &l);
	if (rval == -1) {
	    printf("SOUND_MIXER_WRITE_IGAIN: %s\n", strerror(errno));
	    return (rval);
	}

	if (!done) {
		r = l = 100 * mongain / 255;	/* Normalize to 0-100 */
# ifdef DEBUG
		if (debug > 1)
			printf("audio_gain: mongain %d/%d\n", mongain, l);
# endif
		l |= r << 8;
		rval = ioctl(ctl_fd, SOUND_MIXER_WRITE_OGAIN, &l);
		if (rval == -1) {
			printf("SOUND_MIXER_WRITE_OGAIN: %s\n",
			       strerror(errno));
			return (rval);
		}

# ifdef DEBUG
		if (debug > 1)
			printf("audio_gain: port %d\n", port);
# endif
		l = (1 << ((port == 2) ? SOUND_MIXER_LINE : SOUND_MIXER_MIC));
		rval = ioctl(ctl_fd, SOUND_MIXER_WRITE_RECSRC, &l);
		if (rval == -1) {
			printf("SOUND_MIXER_WRITE_RECSRC: %s\n",
			       strerror(errno));
			return (rval);
		}
# ifdef DEBUG
		if (debug > 1) {
			if (ioctl(ctl_fd, SOUND_MIXER_READ_RECSRC, &l) == -1)
				printf("SOUND_MIXER_WRITE_RECSRC: %s\n",
				       strerror(errno));
			else
				printf("audio_gain: recsrc is %d\n", l);
		}
# endif
		++done;
	}
#else /* not PCM_STYLE_SOUND */
	ioctl(ctl_fd, (int)AUDIO_GETINFO, &info);
	info.record.error = 0;
	info.record.gain = gain;
	if (!done) {
		info.record.port = port;
		info.monitor_gain = mongain;
		info.play.gain = AUDIO_MAX_GAIN;
		info.play.port = AUDIO_SPEAKER;
		++done;
	}
	rval = ioctl(ctl_fd, (int)AUDIO_SETINFO, &info);
	if (rval < 0) {
		msyslog(LOG_ERR, "audio_gain: %m");
		return (rval);
	}
	rval = info.record.error;
#endif /* not PCM_STYLE_SOUND */
	return (rval);
}


/*
 * audio_show - display audio parameters
 *
 * This code doesn't really do anything, except satisfy curiousity and
 * verify the ioctl's work.
 */
void
audio_show(void)
{
#ifdef PCM_STYLE_SOUND
	int devmask = 0, recmask = 0, recsrc = 0, orecsrc;

	printf("audio_show: ctl_fd %d\n", ctl_fd);
	if (ioctl(ctl_fd, SOUND_MIXER_READ_DEVMASK, &devmask) == -1)
	    printf("SOUND_MIXER_READ_DEVMASK: %s\n", strerror(errno));
	if (ioctl(ctl_fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1)
	    printf("SOUND_MIXER_READ_RECMASK: %s\n", strerror(errno));
	if (ioctl(ctl_fd, SOUND_MIXER_READ_RECSRC, &recsrc) == -1)
	    printf("SOUND_MIXER_READ_RECSRC: %s\n", strerror(errno));
	orecsrc = recsrc;

#else /* not PCM_STYLE_SOUND */
# ifdef HAVE_SYS_AUDIOIO_H
	ioctl(ctl_fd, (int)AUDIO_GETDEV, &device);
	printf("audio: name %s, version %s, config %s\n",
	    device.name, device.version, device.config);
# endif /* HAVE_SYS_AUDIOIO_H */
	ioctl(ctl_fd, (int)AUDIO_GETINFO, &info);
	printf(
	    "audio: rate %d, chan %d, prec %d, code %d, gain %d, mon %d, port %d\n",
	    info.record.sample_rate, info.record.channels,
	    info.record.precision, info.record.encoding,
	    info.record.gain, info.monitor_gain, info.record.port);
	printf(
	    "audio: samples %d, eof %d, pause %d, error %d, waiting %d, balance %d\n",
	    info.record.samples, info.record.eof,
	    info.record.pause, info.record.error,
	    info.record.waiting, info.record.balance);
#endif /* not PCM_STYLE_SOUND */
}
#else
int audio_bs;
#endif /* HAVE_{SYS_AUDIOIO,SUN_AUDIOIO,MACHINE_SOUNDCARD,SYS_SOUNDCARD}_H */
