/*
 * Copyright the NTPsec project contributors
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

/* Report information about packet timestamps. */

#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <netinet/in.h>

#include <sys/param.h>
#include <sys/socket.h>
#ifndef __linux__
#include <sys/sysctl.h>
#endif
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>

#if defined(__ppc__)
#define ARCH "ppc"
#elif defined(__ppc64__)
#define ARCH "ppc64"
#elif defined(__ppc64le__) || defined(__ppc64el__)
#define ARCH "ppc64le"
#elif defined(__i386__)
#define ARCH "i386"
#elif defined(__x86_64__)
#define ARCH "x86_64"
#elif defined(__arm__)
#define ARCH "arm"
#elif defined(__arm64__) || defined(__aarch64__)
#define ARCH "arm64"
#else
#define ARCH "unknown"
#endif

#if defined(__linux__)
#define OSTYPE "Linux"
#elif defined(__APPLE__)
#define OSTYPE "Darwin"
#elif defined(__FreeBSD__)
#define OSTYPE "FreeBSD"
#elif defined(__OpenBSD__)
#define OSTYPE "OpenBSD"
#elif defined(__NetBSD__)
#define OSTYPE "NetBSD"
#elif defined(__sun)
#define OSTYPE "Solaris"
#else
#define OSTYPE "unknown"
#endif

#define SYSCTL_OSVER_CLASS CTL_KERN
#define SYSCTL_OSVER_ITEM  KERN_OSRELEASE
#define LINUX_OSVER_PROC   "/proc/sys/kernel/osrelease"

typedef struct timeval timeval_t;
const timeval_t timeval_sample;

#ifdef CLOCK_REALTIME
#define HAVE_TIMESPEC
typedef struct timespec timespec_t;
const timespec_t timespec_sample;
#endif

#define TS_TYPES \
  TS_ONE(none,) \
  TS_ONE(tv,struct timeval) \
  TS_ONE(ts,struct timespec) \
  TS_ONE(bin,struct bintime) \
  TS_ONE(u64,uint64_t) \
  TS_ONE(u64mach,uint64_t (mach)) \

#define TS_ONE(name,str) ts_##name,
typedef enum ts_type {
  TS_TYPES
} ts_type_t;
#undef TS_ONE

#define TS_ONE(name,str) #str,
const char * const ts_type_names[] = {
  TS_TYPES
};
#undef TS_ONE

const union {
	uint8_t c[8];
	uint64_t i;
} endian_test = {{1, 2, 3, 4, 5, 6, 7, 8}};

#define ENDIAN_BIG 0x0102030405060708ULL
#define ENDIAN_LITTLE 0x0807060504030201ULL

#define ENDIANNESS (endian_test.i == ENDIAN_BIG ? "big" \
                    : endian_test.i == ENDIAN_LITTLE ? "little" \
                    : "unknown")

#define BITNESS ((sizeof(long) != sizeof(void *)) ? "unknown" : \
                 (sizeof(long) == 4) ? "32" : (sizeof(long) == 8) ? "64" \
                 : "unknown")

const char sample[] = "The Quick Brown Fox";

char osver[256];
uint8_t dbuf[256];
uint8_t cbuf[256];

static void
report_sizes(const char *name, const char *xname,
             int size, int size1, int size2)
{
  int pad = size - size1 - size2;
  if (pad) {
    printf("    struct %s is %d bytes, with %d-bit tv_sec, %d-bit %s,"
           " and %d pad bytes\n",
           name, size, size1 * 8, size2 * 8, xname, pad);
  } else {
    printf("    struct %s is %d bytes, with %d-bit tv_sec, and %d-bit %s\n",
           name, size, size1 * 8, size2 * 8, xname);
  }
}

#define REPORT_STRUCT(name, name_s, xname) \
  report_sizes(#name, #xname, sizeof(name_s), \
               sizeof(name_s.tv_sec), sizeof(name_s.xname))

static int
get_osver(void)
{
#ifdef __linux__
  int fd, saverr;
  ssize_t len;

  if ((fd = open(LINUX_OSVER_PROC, O_RDONLY)) < 0) return -1;
  len = read(fd, osver, sizeof(osver));
  saverr = errno;
  (void) close(fd);
  errno = saverr;
#else /* !__linux__ */
  int mib[] = {SYSCTL_OSVER_CLASS, SYSCTL_OSVER_ITEM};
  int miblen = sizeof(mib) / sizeof(mib[0]);
  size_t len = sizeof(osver);

  if (sysctl(mib, miblen, osver, &len, NULL, 0)) return -1;
#endif /* !__linux__ */
  if (len <= 0 || len >= sizeof(osver)) return -1;
  osver[len] = '\0';
  if (osver[len - 1] == '\n') osver[len - 1] = '\0';
  return 0;
}

static void
report_sys(void)
{
  int err;

  err = get_osver();
  printf("OS is %s %s, CPU is %s, %s-bit %s-endian\n",
         OSTYPE, err ? "???" : osver, ARCH, BITNESS, ENDIANNESS);
  REPORT_STRUCT(timeval, timeval_sample, tv_usec);
  #ifdef HAVE_TIMESPEC
  REPORT_STRUCT(timespec, timespec_sample, tv_nsec);
  #endif
}

static const char *
test_packet(int sockopt, int sockopt2, int sockval2,
            socklen_t *cbuflen, timeval_t *tv1, timeval_t *tv2)
{
  const char *err = NULL;
  int saverr, sockin = -1, sockout = -1;
  struct sockaddr_in addrin = {.sin_family = AF_INET};
  struct sockaddr_in addrout = {.sin_family = AF_INET};
  struct sockaddr_in addrinused, addroutused;
  socklen_t addrinused_len, addroutused_len;
  struct iovec msgiov = {.iov_base = dbuf, .iov_len = sizeof(dbuf)};
  struct msghdr hdr = {
      .msg_name = NULL,
      .msg_namelen = 0,
      .msg_iov = &msgiov,
      .msg_iovlen = 1,
      };
  static const int trueval = 1;

  hdr.msg_control = cbuf;
  hdr.msg_controllen = *cbuflen;

  do {
    if ((sockin = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
      err = "socket() in";
      break;
    }
    if ((sockout = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
      err = "socket() out";
      break;
    }

    if (sockopt && setsockopt(sockin, SOL_SOCKET, sockopt,
                              (const void *) &trueval, sizeof(trueval)) < 0) {
      err = "setsockopt()";
      break;
    }
    if (sockopt2 && setsockopt(sockin, SOL_SOCKET, sockopt2,
                              (const void *) &sockval2, sizeof(sockval2)) < 0) {
      err = "setsockopt() (2)";
      break;
    }

    addrin.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(sockin, (struct sockaddr *) &addrin, sizeof(addrin))) {
      err = "bind() in";
      break;
    }
    addrinused_len = sizeof(addrinused);
    if (getsockname(sockin,
                    (struct sockaddr *) &addrinused, &addrinused_len)) {
      err = "getsockname() in";
      break;
    }

    addrout.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(sockout, (struct sockaddr *) &addrout, sizeof(addrout))) {
      err = "bind() out";
      break;
    }
    addroutused_len = sizeof(addroutused);
    if (getsockname(sockout,
                    (struct sockaddr *) &addroutused, &addroutused_len)) {
      err = "getsockname() out";
      break;
    }

    if (connect(sockin, (struct sockaddr *) &addroutused, addroutused_len)) {
      err = "connect() in->out";
      break;
    }
    if (connect(sockout, (struct sockaddr *) &addrinused, addrinused_len)) {
      err = "connect() out->in";
      break;
    }

    if (gettimeofday(tv1, NULL)) {
      err = "pre-send gettimeofday()";
      break;
    }
    if (send(sockout, sample, sizeof(sample), 0) != sizeof(sample)) {
      err = "send()";
      break;
    }
    if (recvmsg(sockin, &hdr, 0) != sizeof(sample)) {
      err = "recvmsg()";
      break;
    }
    if (gettimeofday(tv2, NULL)) {
      err = "post-recv gettimeofday()";
      break;
    }
    *cbuflen = hdr.msg_controllen;

    if (strncmp((const char *) dbuf, sample, sizeof(sample))) {
      err = "received data compare";
    }
  } while(0);

  saverr = errno;
  if (sockout >= 0) (void) close(sockout);
  if (sockin >= 0) (void) close(sockin);
  errno = saverr;
  return err;
}

static void
test_timestamp(const char *name, ts_type_t tstype,
               int sockopt, int sockopt2, int sockval2, int scmtype)
{
  const char *tsname = ts_type_names[tstype];
  const char *err = NULL;
  timeval_t time1, time2;
  double time1d, time2d;
  socklen_t cbuflen, cmsglen;
  struct cmsghdr *cmsg;
  int cmsglvl, cmsgtype, hdrlen, datalen;
  uint32_t *datap, *xdatap;

  printf("  Testing %s:\n", name);
  cbuflen = sizeof(cbuf);
  err = test_packet(sockopt, sockopt2, sockval2,
                    &cbuflen, &time1, &time2);
  if (err) {
    printf("    Error on %s: %s\n", err, strerror(errno));
    return;
  }

  time1d = (double) time1.tv_sec + time1.tv_usec * 1.0E-6;
  time2d = (double) time2.tv_sec + time2.tv_usec * 1.0E-6;
  if (cbuflen < sizeof(struct cmsghdr)) {
    cmsglen = 0;
    cmsglvl = 0;
    cmsgtype = -1;
    cmsg = NULL;
  } else {
    cmsg = (struct cmsghdr *) cbuf;
    cmsglen = cmsg->cmsg_len;
    cmsglvl = cmsg->cmsg_level;
    cmsgtype = cmsg->cmsg_type;
  }
  printf("    Sent at %f, received at %f, took %f\n",
         time1d, time2d, time2d - time1d);
  if (cmsgtype != scmtype) {
    printf("    Unexpected cmsg_type %d != %d\n", cmsgtype, scmtype);
    return;
  }
  if (!cmsglen) return;
  if (cmsg->cmsg_level != SOL_SOCKET) {
    printf("    Unexpected cmsg_level %d != SOL_SOCKET\n", cmsg->cmsg_level);
    return;
  }

  hdrlen = ((uint8_t *) CMSG_DATA(cmsg)) - ((uint8_t *) cmsg);
  datalen = cmsglen - hdrlen;
  datap = (uint32_t *) (cbuf + hdrlen);
  printf("    cmsg length = %d (%d+%d), level = %d, type = %d\n",
         cmsglen, hdrlen, datalen, cmsglvl, cmsgtype);
  if (hdrlen > (int) sizeof(*cmsg)) {
    xdatap = (uint32_t *) (cbuf + sizeof(*cmsg));
    printf("    header padding:\n");
    while (xdatap < datap) {
      printf("     (%010u)\n", *xdatap++);
    }
  }
  printf("    %s%spayload longwords:\n", tsname, tsname[0] ? " " : "");
  while (datap < (uint32_t *) (cbuf + cmsglen)) {
    printf("      %010u\n", *datap++);
  }
  if (cbuflen > cmsglen) {
    printf("    +%d bytes of additional cmsg data\n", cbuflen - cmsglen);
  }
}

int
main(int argc, char *argv[])
{
  (void) argc; (void) argv;

  report_sys();

  test_timestamp("(no timestamp)", ts_none, 0, 0, 0, -1);
  /* Standard version, available on most systems */
  #ifdef SO_TIMESTAMP
  test_timestamp("SO_TIMESTAMP", ts_tv, SO_TIMESTAMP, 0, 0, SCM_TIMESTAMP);
    /* FreeBSD 11.3+ enhancements to SO_TIMESTAMP */
    #ifdef SO_TS_CLOCK
      /* This one is the default (same as standard SO_TIMESTAMP) */
      #ifdef SO_TS_REALTIME_MICRO
      test_timestamp("SO_TIMESTAMP+SO_TS_CLOCK+SO_TS_REALTIME_MICRO", ts_tv,
                     SO_TIMESTAMP, SO_TS_CLOCK,
                     SO_TS_REALTIME_MICRO, SCM_TIMESTAMP);
      #endif
      #ifdef SO_TS_BINTIME
      test_timestamp("SO_TIMESTAMP+SO_TS_CLOCK+SO_TS_BINTIME", ts_bin,
                     SO_TIMESTAMP, SO_TS_CLOCK,
                     SO_TS_BINTIME, SCM_BINTIME);
      #endif
      #ifdef SO_TS_REALTIME
      test_timestamp("SO_TIMESTAMP+SO_TS_CLOCK+SO_TS_REALTIME", ts_ts,
                     SO_TIMESTAMP, SO_TS_CLOCK,
                     SO_TS_REALTIME, SCM_REALTIME);
      #endif
      #ifdef SO_TS_MONOTONIC
      test_timestamp("SO_TIMESTAMP+SO_TS_CLOCK+SO_TS_MONOTONIC", ts_ts,
                     SO_TIMESTAMP, SO_TS_CLOCK,
                     SO_TS_MONOTONIC, SCM_MONOTONIC);
      #endif
    #endif
  #endif
  /* Linux enhancement */
  #ifdef SO_TIMESTAMPNS
  test_timestamp("SO_TIMESTAMPNS", ts_ts,
                 SO_TIMESTAMPNS, 0, 0, SCM_TIMESTAMPNS);
  #endif
  /* macOS enhancement in 10.7+ */
  #ifdef SO_TIMESTAMP_MONOTONIC
  test_timestamp("SO_TIMESTAMP_MONOTONIC", ts_u64mach,
                 SO_TIMESTAMP_MONOTONIC,  0, 0, SCM_TIMESTAMP_MONOTONIC);
  #endif
  /* The following is in macOS 10.14+ kernel sources, but not user headers. */
  #ifdef SO_TIMESTAMP_CONTINUOUS
  test_timestamp("SO_TIMESTAMP_CONTINUOUS", ts_u64mach,
                 SO_TIMESTAMP_CONTINUOUS, 0, 0, SCM_TIMESTAMP_CONTINUOUS);
  #endif

  return 0;
}
