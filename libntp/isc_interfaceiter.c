/*
 * Copyright (C) 2004, 2005, 2007, 2008  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1999-2003  Internet Software Consortium.
 * Copyright 2015 by the NTPsec project contributors
 * SPDX-License-Identifier: ISC
 */

#include "config.h"

#include <sys/types.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_SOCKIO_H
#include <sys/sockio.h>         /* Required for ifiter_ioctl.c. */
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>                /* Contractual promise. */

#include "ntp_assert.h"
#include "ntp_stdlib.h"
#include "isc_interfaceiter.h"
#include "isc_netaddr.h"
#include "isc_result.h"
#include "isc_error.h"


/* Must follow <isc/net.h>. */
#ifdef HAVE_NET_IF6_H
#include <net/if6.h>
#endif
#include <net/if.h>

#ifdef HAVE_LINUX_IF_ADDR_H
# include <linux/if_addr.h>
#endif

#define ISC_TF(x) ((x) ? true : false)

static void
isc_netaddr_setzone(isc_netaddr_t *netaddr, uint32_t zone);

static void
isc_netaddr_setzone(isc_netaddr_t *netaddr, uint32_t zone) {
        /* we currently only support AF_INET6. */
        REQUIRE(netaddr->family == AF_INET6);

        netaddr->zone = zone;
}

typedef struct {
        unsigned int magic;
} isc__magic_t;

/*
 * To use this macro the magic number MUST be the first thing in the
 * structure, and MUST be of type "unsigned int".
 * The intent of this is to allow magic numbers to be checked even though
 * the object is otherwise opaque.
 */
#define ISC_MAGIC_VALID(a,b)    (((a) != NULL) && \
                                 (((const isc__magic_t *)(a))->magic == (b)))

#define ISC_MAGIC(a, b, c, d)   ((a) << 24 | (b) << 16 | (c) << 8 | (d))

/* Common utility functions */

/*%
 * Extract the network address part from a "struct sockaddr".
 * \brief
 * The address family is given explicitly
 * instead of using src->sa_family, because the latter does not work
 * for copying a network mask obtained by SIOCGIFNETMASK (it does
 * not have a valid address family).
 */

static void
get_addr(unsigned int family, isc_netaddr_t *dst, struct sockaddr *src,
         char *ifname)
{
        struct sockaddr_in6 *sa6;

        /* clear any remaining value for safety */
        memset(dst, 0, sizeof(*dst));

        dst->family = family;
        switch (family) {
        case AF_INET:
                memcpy(&dst->type.in,
                       &((struct sockaddr_in *)(void *)src)->sin_addr,
                       sizeof(struct in_addr));
                break;
        case AF_INET6:
                sa6 = (struct sockaddr_in6 *)(void *)src;
                memcpy(&dst->type.in6, &sa6->sin6_addr,
                       sizeof(struct in6_addr));
                if (sa6->sin6_scope_id != 0)
                        isc_netaddr_setzone(dst, sa6->sin6_scope_id);
                else {
                        /*
                         * BSD variants embed scope zone IDs in the 128bit
                         * address as a kernel internal form.  Unfortunately,
                         * the embedded IDs are not hidden from applications
                         * when getting access to them by sysctl or ioctl.
                         * We convert the internal format to the pure address
                         * part and the zone ID part.
                         * Since multicast addresses should not appear here
                         * and they cannot be distinguished from netmasks,
                         * we only consider unicast link-local addresses.
                         */
                        if (IN6_IS_ADDR_LINKLOCAL(&sa6->sin6_addr)) {
                                uint16_t zone16;

                                memcpy(&zone16, &sa6->sin6_addr.s6_addr[2],
                                       sizeof(zone16));
                                zone16 = ntohs(zone16);
                                if (zone16 != 0) {
                                        /* the zone ID is embedded */
                                        isc_netaddr_setzone(dst,
                                                            (uint32_t)zone16);
                                        dst->type.in6.s6_addr[2] = 0;
                                        dst->type.in6.s6_addr[3] = 0;
                                } else if (ifname != NULL) {
                                        unsigned int zone;

                                        /*
                                         * sin6_scope_id is still not provided,
                                         * but the corresponding interface name
                                         * is know.  Use the interface ID as
                                         * the link ID.
                                         */
                                        zone = if_nametoindex(ifname);
                                        if (zone != 0) {
                                                isc_netaddr_setzone(dst,
                                                                    (uint32_t)zone);
                                        }
                                }
                        }
                }
                break;
        default:
                INSIST(0);
                break;
        }
}

/*
 * Include system-dependent code.
 */

#ifdef __linux
#define ISC_IF_INET6_SZ \
    sizeof("00000000000000000000000000000001 01 80 10 80 XXXXXXloXXXXXXXX\n")
static isc_result_t linux_if_inet6_next(isc_interfaceiter_t *);
static isc_result_t linux_if_inet6_current(isc_interfaceiter_t *);
static void linux_if_inet6_first(isc_interfaceiter_t *iter);
static bool seenv6 = false;

/* IF_NAMESIZE usually defined in net/if.h */
# ifndef IF_NAMESIZE
#  ifdef IFNAMSIZ
#   define IF_NAMESIZE  IFNAMSIZ
#  else
#   define IF_NAMESIZE 16
#  endif
# endif

#endif


bool
isc_interfaceiter_next_bool(isc_interfaceiter_t *iter) {
  return (ISC_R_SUCCESS == isc_interfaceiter_next(iter));
}

bool
isc_interfaceiter_create_bool(isc_mem_t *mctx, isc_interfaceiter_t **iterp) {
  return (ISC_R_SUCCESS == isc_interfaceiter_create(mctx, iterp));
}

bool
isc_interfaceiter_first_bool(isc_interfaceiter_t *iter) {
  return (ISC_R_SUCCESS == isc_interfaceiter_first(iter));
}

bool
isc_interfaceiter_current_bool(isc_interfaceiter_t *iter,
                          isc_interface_t *ifdata) {
  return (ISC_R_SUCCESS == isc_interfaceiter_current(iter, ifdata));
}


/*
 *  start of the big 3 way switch, in order, try:
 *
 * 1. have ifaddrs.h
 * 2. have sys/sysctl.h
 * 3. using the SIOCGLIFCONF ioctl.
 *
 * question, do we need all three??
 */
#if HAVE_IFADDRS_H

/*
 * Obtain the list of network interfaces using the getifaddrs(3) library.
 */

#include <ifaddrs.h>

/*% Iterator Magic */
#define IFITER_MAGIC            ISC_MAGIC('I', 'F', 'I', 'G')
/*% Valid Iterator */
#define VALID_IFITER(t)         ISC_MAGIC_VALID(t, IFITER_MAGIC)

/*% Iterator structure */
struct isc_interfaceiter {
        unsigned int            magic;          /*%< Magic number. */
        isc_mem_t               *mctx;
        void                    *buf;           /*%< (unused) */
        unsigned int            bufsize;        /*%< (always 0) */
        struct ifaddrs          *ifaddrs;       /*%< List of ifaddrs */
        struct ifaddrs          *pos;           /*%< Ptr to current ifaddr */
        isc_interface_t         current;        /*%< Current interface data. */
        isc_result_t            result;         /*%< Last result code. */
#ifdef __linux
        FILE *                  proc;
        char                    entry[ISC_IF_INET6_SZ];
        isc_result_t            valid;
#endif
};

isc_result_t
isc_interfaceiter_create(isc_mem_t *mctx, isc_interfaceiter_t **iterp) {
        isc_interfaceiter_t *iter;
        isc_result_t result;
        char strbuf[BUFSIZ];
        int trys, ret = 0;

        REQUIRE(mctx != NULL);
        REQUIRE(iterp != NULL);
        REQUIRE(*iterp == NULL);

        iter = emalloc(sizeof(*iter));
        if (iter == NULL)
                return (ISC_R_NOMEMORY);

        iter->mctx = mctx;
        iter->buf = NULL;
        iter->bufsize = 0;
        iter->ifaddrs = NULL;
#ifdef __linux
        /*
         * Only open "/proc/net/if_inet6" if we have never seen a IPv6
         * address returned by getifaddrs().
         */
        if (!seenv6) {
                iter->proc = fopen("/proc/net/if_inet6", "r");
                if (iter->proc == NULL) {
			IGNORE(strerror_r(errno, strbuf, sizeof(strbuf)));
/*                      isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
                                      ISC_LOGMODULE_SOCKET, ISC_LOG_WARNING,
                                      "failed to open /proc/net/if_inet6");
*/
                }
        } else
                iter->proc = NULL;
        iter->valid = ISC_R_FAILURE;
#endif

        /* If interrupted, try again */
        for (trys = 0; trys < 3; trys++) {
                if ((ret = getifaddrs(&iter->ifaddrs)) >= 0)
                        break;
                if (errno != EINTR)
                        break;
        }
        if (ret < 0) {
		IGNORE(strerror_r(errno, strbuf, sizeof(strbuf)));
                UNEXPECTED_ERROR("getting interface addresses: getifaddrs: %s",
                                 strbuf);
                result = ISC_R_UNEXPECTED;
                goto failure;
        }

        /*
         * A newly created iterator has an undefined position
         * until isc_interfaceiter_first() is called.
         */
        iter->pos = NULL;
        iter->result = ISC_R_FAILURE;

        iter->magic = IFITER_MAGIC;
        *iterp = iter;
        return (ISC_R_SUCCESS);

 failure:
#ifdef __linux
        if (iter->proc != NULL)
                fclose(iter->proc);
#endif
        if (iter->ifaddrs != NULL) /* just in case */
                freeifaddrs(iter->ifaddrs);
        free(iter);
        return (result);
}

/*
 * Get information about the current interface to iter->current.
 * If successful, return ISC_R_SUCCESS.
 * If the interface has an unsupported address family,
 * return ISC_R_IGNORE.
 */

static isc_result_t
internal_current(isc_interfaceiter_t *iter) {
        struct ifaddrs *ifa;
        int family;
        unsigned int namelen;

        REQUIRE(VALID_IFITER(iter));

        ifa = iter->pos;

#ifdef __linux
        /*
         * [Bug 2792]
         * burnicki: iter->pos is usually never NULL here (anymore?),
         * so linux_if_inet6_current(iter) is never called here.
         * However, that routine would check (under Linux), if the
         * interface is in a tentative state, e.g. if there's no link
         * yet but an IPv6 address has already be assigned.
         */
        if (iter->pos == NULL)
                return (linux_if_inet6_current(iter));
#endif

        INSIST(ifa != NULL);
        INSIST(ifa->ifa_name != NULL);


#ifdef IFF_RUNNING
        /*
         * [Bug 2792]
         * burnicki: if the interface is not running then
         * it may be in a tentative state. See above.
         */
        if ((ifa->ifa_flags & IFF_RUNNING) == 0)
                return (ISC_R_IGNORE);
#endif

        if (ifa->ifa_addr == NULL)
                return (ISC_R_IGNORE);

        family = ifa->ifa_addr->sa_family;
        if (family != AF_INET && family != AF_INET6)
                return (ISC_R_IGNORE);

#ifdef __linux
        if (family == AF_INET6)
                seenv6 = true;
#endif

        memset(&iter->current, 0, sizeof(iter->current));

        namelen = (unsigned int)strlen(ifa->ifa_name);
        if (namelen > sizeof(iter->current.name) - 1)
                namelen = sizeof(iter->current.name) - 1;

        memset(iter->current.name, 0, sizeof(iter->current.name));
        memcpy(iter->current.name, ifa->ifa_name, namelen);

        iter->current.flags = 0;

        if ((ifa->ifa_flags & IFF_UP) != 0)
                iter->current.flags |= INTERFACE_F_UP;

        if ((ifa->ifa_flags & IFF_POINTOPOINT) != 0)
                iter->current.flags |= INTERFACE_F_POINTTOPOINT;

        if ((ifa->ifa_flags & IFF_LOOPBACK) != 0)
                iter->current.flags |= INTERFACE_F_LOOPBACK;

        if ((ifa->ifa_flags & IFF_BROADCAST) != 0)
                iter->current.flags |= INTERFACE_F_BROADCAST;

#ifdef IFF_MULTICAST
        if ((ifa->ifa_flags & IFF_MULTICAST) != 0)
                iter->current.flags |= INTERFACE_F_MULTICAST;
#endif

        iter->current.af = (unsigned int)family;

        get_addr((unsigned int)family, &iter->current.address,
                  ifa->ifa_addr, ifa->ifa_name);

        if (ifa->ifa_netmask != NULL)
                get_addr((unsigned int)family, &iter->current.netmask, ifa->ifa_netmask,
                         ifa->ifa_name);

        if (ifa->ifa_dstaddr != NULL &&
            (iter->current.flags & INTERFACE_F_POINTTOPOINT) != 0)
                get_addr((unsigned int)family, &iter->current.dstaddress,
                         ifa->ifa_dstaddr,
                         ifa->ifa_name);

        if (ifa->ifa_broadaddr != NULL &&
            (iter->current.flags & INTERFACE_F_BROADCAST) != 0)
                get_addr((unsigned int)family, &iter->current.broadcast,
                         ifa->ifa_broadaddr,
                         ifa->ifa_name);

        iter->current.ifindex = if_nametoindex(iter->current.name);
        return (ISC_R_SUCCESS);
}

/*
 * Step the iterator to the next interface.  Unlike
 * isc_interfaceiter_next(), this may leave the iterator
 * positioned on an interface that will ultimately
 * be ignored.  Return ISC_R_NOMORE if there are no more
 * interfaces, otherwise ISC_R_SUCCESS.
 */
static isc_result_t
internal_next(isc_interfaceiter_t *iter) {

        if (iter->pos != NULL)
                iter->pos = iter->pos->ifa_next;
        if (iter->pos == NULL) {
#ifdef __linux
                if (!seenv6)
                        return (linux_if_inet6_next(iter));
#endif
                return (ISC_R_NOMORE);
        }

        return (ISC_R_SUCCESS);
}

static void
internal_destroy(isc_interfaceiter_t *iter) {

#ifdef __linux
        if (iter->proc != NULL)
                fclose(iter->proc);
        iter->proc = NULL;
#endif
        if (iter->ifaddrs)
                freeifaddrs(iter->ifaddrs);
        iter->ifaddrs = NULL;
}

static
void internal_first(isc_interfaceiter_t *iter) {

#ifdef __linux
        linux_if_inet6_first(iter);
#endif
        iter->pos = iter->ifaddrs;
}
/* end HAVE_IFADDRS_H */
#elif defined(HAVE_IFLIST_SYSCTL) && HAVE_IFLIST_SYSCTL

/*
 * Obtain the list of network interfaces using sysctl.
 * See TCP/IP Illustrated Volume 2, sections 19.8, 19.14,
 * and 19.16.
 */

#include <sys/param.h>
#include <sys/sysctl.h>

#include <net/route.h>
#include <net/if_dl.h>

#define ROUNDUP(a) ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) \
                    : sizeof(long))

#define IFITER_MAGIC            ISC_MAGIC('I', 'F', 'I', 'S')
#define VALID_IFITER(t)         ISC_MAGIC_VALID(t, IFITER_MAGIC)

struct isc_interfaceiter {
        unsigned int            magic;          /* Magic number. */
        isc_mem_t               *mctx;
        void                    *buf;           /* Buffer for sysctl data. */
        unsigned int            bufsize;        /* Bytes allocated. */
        unsigned int            bufused;        /* Bytes used. */
        unsigned int            pos;            /* Current offset in
                                                   sysctl data. */
        isc_interface_t         current;        /* Current interface data. */
        isc_result_t            result;         /* Last result code. */
};

static int mib[6] = {
        CTL_NET,
        PF_ROUTE,
        0,
        0,                      /* Any address family. */
        NET_RT_IFLIST,
        0                       /* Flags. */
};

isc_result_t
isc_interfaceiter_create(isc_mem_t *mctx, isc_interfaceiter_t **iterp) {
        isc_interfaceiter_t *iter;
        isc_result_t result;
        size_t bufsize;
        size_t bufused;
        char strbuf[BUFSIZ];

        REQUIRE(mctx != NULL);
        REQUIRE(iterp != NULL);
        REQUIRE(*iterp == NULL);

        iter = emalloc(sizeof(*iter));
        if (iter == NULL)
                return (ISC_R_NOMEMORY);

        iter->mctx = mctx;
        iter->buf = 0;

        /*
         * Determine the amount of memory needed.
         */
        bufsize = 0;
        if (sysctl(mib, 6, NULL, &bufsize, NULL, (size_t) 0) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("getting interface list size: sysctl: %s",
                                 strbuf);
                result = ISC_R_UNEXPECTED;
                goto failure;
        }
        iter->bufsize = bufsize;

        iter->buf = emalloc(iter->bufsize);
        if (iter->buf == NULL) {
                result = ISC_R_NOMEMORY;
                goto failure;
        }

        bufused = bufsize;
        if (sysctl(mib, 6, iter->buf, &bufused, NULL, (size_t) 0) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("getting interface list: sysctl: %s",
                                 strbuf);
                result = ISC_R_UNEXPECTED;
                goto failure;
        }
        iter->bufused = bufused;
        INSIST(iter->bufused <= iter->bufsize);

        /*
         * A newly created iterator has an undefined position
         * until isc_interfaceiter_first() is called.
         */
        iter->pos = (unsigned int) -1;
        iter->result = ISC_R_FAILURE;

        iter->magic = IFITER_MAGIC;
        *iterp = iter;
        return (ISC_R_SUCCESS);

 failure:
        if (iter->buf != NULL)
                free(iter->buf);
        free(iter);
        return (result);
}

/*
 * Get information about the current interface to iter->current.
 * If successful, return ISC_R_SUCCESS.
 * If the interface has an unsupported address family,
 * return ISC_R_IGNORE.  In case of other failure,
 * return ISC_R_UNEXPECTED.
 */

static isc_result_t
internal_current(isc_interfaceiter_t *iter) {
        struct ifa_msghdr *ifam, *ifam_end;

        REQUIRE(VALID_IFITER(iter));
        REQUIRE (iter->pos < (unsigned int) iter->bufused);

        ifam = (struct ifa_msghdr *) ((char *) iter->buf + iter->pos);
        ifam_end = (struct ifa_msghdr *) ((char *) iter->buf + iter->bufused);

        // Skip wrong RTM version headers
        if (ifam->ifam_version != RTM_VERSION)
                return (ISC_R_IGNORE);

        if (ifam->ifam_type == RTM_IFINFO) {
                struct if_msghdr *ifm = (struct if_msghdr *) ifam;
                struct sockaddr_dl *sdl = (struct sockaddr_dl *) (ifm + 1);
                unsigned int namelen;

                memset(&iter->current, 0, sizeof(iter->current));

                iter->current.ifindex = sdl->sdl_index;
                namelen = sdl->sdl_nlen;
                if (namelen > sizeof(iter->current.name) - 1)
                        namelen = sizeof(iter->current.name) - 1;

                memset(iter->current.name, 0, sizeof(iter->current.name));
                memcpy(iter->current.name, sdl->sdl_data, namelen);

                iter->current.flags = 0;

                if ((ifam->ifam_flags & IFF_UP) != 0)
                        iter->current.flags |= INTERFACE_F_UP;

                if ((ifam->ifam_flags & IFF_POINTOPOINT) != 0)
                        iter->current.flags |= INTERFACE_F_POINTTOPOINT;

                if ((ifam->ifam_flags & IFF_LOOPBACK) != 0)
                        iter->current.flags |= INTERFACE_F_LOOPBACK;

                if ((ifam->ifam_flags & IFF_BROADCAST) != 0)
                        iter->current.flags |= INTERFACE_F_BROADCAST;

#ifdef IFF_MULTICAST
                if ((ifam->ifam_flags & IFF_MULTICAST) != 0)
                        iter->current.flags |= INTERFACE_F_MULTICAST;
#endif

                /*
                 * This is not an interface address.
                 * Force another iteration.
                 */
                return (ISC_R_IGNORE);
        } else if (ifam->ifam_type == RTM_NEWADDR) {
                int i;
                int family;
                struct sockaddr *mask_sa = NULL;
                struct sockaddr *addr_sa = NULL;
                struct sockaddr *dst_sa = NULL;

                struct sockaddr *sa = (struct sockaddr *)(ifam + 1);
                family = sa->sa_family;

                for (i = 0; i < RTAX_MAX; i++)
                {
                        if ((ifam->ifam_addrs & (1 << i)) == 0)
                                continue;

                        INSIST(sa < (struct sockaddr *) ifam_end);

                        switch (i) {
                        case RTAX_NETMASK: /* Netmask */
                                mask_sa = sa;
                                break;
                        case RTAX_IFA: /* Interface address */
                                addr_sa = sa;
                                break;
                        case RTAX_BRD: /* Broadcast or destination address */
                                dst_sa = sa;
                                break;
                        }
#ifdef ISC_PLATFORM_HAVESALEN
                        sa = (struct sockaddr *)((char*)(sa)
                                         + ROUNDUP(sa->sa_len));
#else
                        /* XXX untested. */
                        sa = (struct sockaddr *)((char*)(sa)
                                         + ROUNDUP(sizeof(struct sockaddr)));
#endif
                }

                if (addr_sa == NULL)
                        return (ISC_R_IGNORE);

                family = addr_sa->sa_family;
                if (family != AF_INET && family != AF_INET6)
                        return (ISC_R_IGNORE);

                iter->current.af = family;

                get_addr(family, &iter->current.address, addr_sa,
                         iter->current.name);

                if (mask_sa != NULL)
                        get_addr(family, &iter->current.netmask, mask_sa,
                                 iter->current.name);

                if (dst_sa != NULL &&
                    (iter->current.flags & INTERFACE_F_POINTTOPOINT) != 0)
                        get_addr(family, &iter->current.dstaddress, dst_sa,
                                 iter->current.name);

                if (dst_sa != NULL &&
                    (iter->current.flags & INTERFACE_F_BROADCAST) != 0)
                        get_addr(family, &iter->current.broadcast, dst_sa,
                                 iter->current.name);

                return (ISC_R_SUCCESS);
        } else {
                printf("warning: unexpected interface list message type\n");
                return (ISC_R_IGNORE);
        }
}

/*
 * Step the iterator to the next interface.  Unlike
 * isc_interfaceiter_next(), this may leave the iterator
 * positioned on an interface that will ultimately
 * be ignored.  Return ISC_R_NOMORE if there are no more
 * interfaces, otherwise ISC_R_SUCCESS.
 */
static isc_result_t
internal_next(isc_interfaceiter_t *iter) {
        struct ifa_msghdr *ifam;
        REQUIRE (iter->pos < (unsigned int) iter->bufused);

        ifam = (struct ifa_msghdr *) ((char *) iter->buf + iter->pos);

        iter->pos += ifam->ifam_msglen;

        if (iter->pos >= iter->bufused)
                return (ISC_R_NOMORE);

        return (ISC_R_SUCCESS);
}

static void
internal_destroy(isc_interfaceiter_t *iter) {
        UNUSED_ARG(iter); /* Unused. */
        /*
         * Do nothing.
         */
}

static
void internal_first(isc_interfaceiter_t *iter) {
        iter->pos = 0;
}
/* end defined(HAVE_IFLIST_SYSCTL) && HAVE_IFLIST_SYSCTL */
#else

/*
 * Obtain the list of network interfaces using the SIOCGLIFCONF ioctl.
 * See netintro(4).
 */

#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
#ifdef HAVE_STRUCT_IF_LADDRCONF
#define lifc_len iflc_len
#define lifc_buf iflc_buf
#define lifc_req iflc_req
#define LIFCONF if_laddrconf
#else
#define USE_LIFC_FAMILY 1
#define USE_LIFC_FLAGS 1
#define LIFCONF lifconf
#endif

#ifdef HAVE_STRUCT_IF_LADDRREQ
#define lifr_addr iflr_addr
#define lifr_name iflr_name
#define lifr_dstaddr iflr_dstaddr
#define lifr_broadaddr iflr_broadaddr
#define lifr_flags iflr_flags
#define lifr_index iflr_index
#define ss_family sa_family
#define LIFREQ if_laddrreq
#else
#define LIFREQ lifreq
#endif
#endif

#define IFITER_MAGIC            ISC_MAGIC('I', 'F', 'I', 'T')
#define VALID_IFITER(t)         ISC_MAGIC_VALID(t, IFITER_MAGIC)

struct isc_interfaceiter {
        unsigned int            magic;          /* Magic number. */
        isc_mem_t               *mctx;
        int                     mode;
        int                     socket;
        struct ifconf           ifc;
        void                    *buf;           /* Buffer for sysctl data. */
        unsigned int            bufsize;        /* Bytes allocated. */
        unsigned int            pos;            /* Current offset in
                                                   SIOCGIFCONF data */
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        int                     socket6;
        struct LIFCONF          lifc;
        void                    *buf6;          /* Buffer for sysctl data. */
        unsigned int            bufsize6;       /* Bytes allocated. */
        unsigned int            pos6;           /* Current offset in
                                                   SIOCGLIFCONF data */
        isc_result_t            result6;        /* Last result code. */
        bool            first6;
#endif
#ifdef __linux
        FILE *                  proc;
        char                    entry[ISC_IF_INET6_SZ];
        isc_result_t            valid;
#endif
        isc_interface_t         current;        /* Current interface data. */
        isc_result_t            result;         /* Last result code. */
};

/*%
 * Size of buffer for SIOCGLIFCONF, in bytes.  We assume no sane system
 * will have more than a megabyte of interface configuration data.
 */
#define IFCONF_BUFSIZE_INITIAL  4096
#define IFCONF_BUFSIZE_MAX      1048576

static int
isc_ioctl(int fildes, int req, char *arg);

static int
isc_ioctl(int fildes, int req, char *arg) {
        int trys;
        int ret;

        for (trys = 0; trys < 3; trys++) {
                if ((ret = ioctl(fildes, req, arg)) < 0) {
                        if (errno == EINTR)
                                continue;
                }
                break;
        }
        return (ret);
}

static isc_result_t
getbuf4(isc_interfaceiter_t *iter) {
        char strbuf[BUFSIZ];

        iter->bufsize = IFCONF_BUFSIZE_INITIAL;

        for (;;) {
                iter->buf = emalloc(iter->bufsize);
                if (iter->buf == NULL)
                        return (ISC_R_NOMEMORY);

                memset(&iter->ifc.ifc_len, 0, sizeof(iter->ifc.ifc_len));
                iter->ifc.ifc_len = iter->bufsize;
                iter->ifc.ifc_buf = iter->buf;
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion".  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(iter->socket, SIOCGIFCONF, (char *)&iter->ifc)
                    == -1) {
                        if (errno != EINVAL) {
                                strerror_r(errno, strbuf, sizeof(strbuf));
                                UNEXPECTED_ERROR("get interface "
                                                 "configuration: %s",
                                                 strbuf);
                                goto unexpected;
                        }
                        /*
                         * EINVAL.  Retry with a bigger buffer.
                         */
                } else {
                        /*
                         * The ioctl succeeded.
                         * Some OS's just return what will fit rather
                         * than set EINVAL if the buffer is too small
                         * to fit all the interfaces in.  If
                         * ifc.lifc_len is too near to the end of the
                         * buffer we will grow it just in case and
                         * retry.
                         */
                        if (iter->ifc.ifc_len + 2 * sizeof(struct ifreq)
                            < iter->bufsize)
                                break;
                }
                if (iter->bufsize >= IFCONF_BUFSIZE_MAX) {
                        UNEXPECTED_ERROR("get interface configuration: "
                                         "maximum buffer size exceeded");
                        goto unexpected;
                }
                free(iter->buf);

                iter->bufsize *= 2;
        }
        return (ISC_R_SUCCESS);

 unexpected:
        free(iter->buf);
        iter->buf = NULL;
        return (ISC_R_UNEXPECTED);
}

#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
static isc_result_t
getbuf6(isc_interfaceiter_t *iter) {
        char strbuf[BUFSIZ];
        isc_result_t result;

        iter->bufsize6 = IFCONF_BUFSIZE_INITIAL;

        for (;;) {
                iter->buf6 = emalloc(iter->bufsize6);
                if (iter->buf6 == NULL)
                        return (ISC_R_NOMEMORY);

                memset(&iter->lifc, 0, sizeof(iter->lifc));
#ifdef USE_LIFC_FAMILY
                iter->lifc.lifc_family = AF_INET6;
#endif
#ifdef USE_LIFC_FLAGS
                iter->lifc.lifc_flags = 0;
#endif
                iter->lifc.lifc_len = iter->bufsize6;
                iter->lifc.lifc_buf = iter->buf6;
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion".  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(iter->socket6, SIOCGLIFCONF, (char *)&iter->lifc)
                    == -1) {
                        if (errno != EINVAL) {
                                strerror_r(errno, strbuf, sizeof(strbuf));
                                UNEXPECTED_ERROR("get interface "
                                                 "configuration: %s",
                                                 strbuf);
                                result = ISC_R_UNEXPECTED;
                                goto cleanup;
                        }
                        /*
                         * EINVAL.  Retry with a bigger buffer.
                         */
                } else {
                        /*
                         * The ioctl succeeded.
                         * Some OS's just return what will fit rather
                         * than set EINVAL if the buffer is too small
                         * to fit all the interfaces in.  If
                         * ifc.ifc_len is too near to the end of the
                         * buffer we will grow it just in case and
                         * retry.
                         */
                        if (iter->lifc.lifc_len + 2 * sizeof(struct LIFREQ)
                            < iter->bufsize6)
                                break;
                }
                if (iter->bufsize6 >= IFCONF_BUFSIZE_MAX) {
                        UNEXPECTED_ERROR("get interface configuration: "
                                         "maximum buffer size exceeded");
                        result = ISC_R_UNEXPECTED;
                        goto cleanup;
                }
                free(iter->buf6);

                iter->bufsize6 *= 2;
        }

        if (iter->lifc.lifc_len != 0)
                iter->mode = 6;
        return (ISC_R_SUCCESS);

 cleanup:
        free(iter->buf6);
        iter->buf6 = NULL;
        return (result);
}
#endif

isc_result_t
isc_interfaceiter_create(isc_mem_t *mctx, isc_interfaceiter_t **iterp) {
        isc_interfaceiter_t *iter;
        isc_result_t result;
        char strbuf[BUFSIZ];

        REQUIRE(mctx != NULL);
        REQUIRE(iterp != NULL);
        REQUIRE(*iterp == NULL);

        iter = emalloc(sizeof(*iter));
        if (iter == NULL)
                return (ISC_R_NOMEMORY);

        iter->mctx = mctx;
        iter->mode = 4;
        iter->buf = NULL;
        iter->pos = (unsigned int) -1;
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        iter->buf6 = NULL;
        iter->pos6 = (unsigned int) -1;
        iter->result6 = ISC_R_NOMORE;
        iter->socket6 = -1;
        iter->first6 = false;
#endif

        /*
         * Get the interface configuration, allocating more memory if
         * necessary.
         */

#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        result = isc_net_probeipv6();
        if (result == ISC_R_SUCCESS) {
                /*
                 * Create an unbound datagram socket to do the SIOCGLIFCONF
                 * ioctl on.  HP/UX requires an AF_INET6 socket for
                 * SIOCGLIFCONF to get IPv6 addresses.
                 */
                if ((iter->socket6 = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
                        strerror_r(errno, strbuf, sizeof(strbuf));
                        UNEXPECTED_ERROR("making interface scan socket: %s",
                                         strbuf);
                        result = ISC_R_UNEXPECTED;
                        goto socket6_failure;
                }
                result = iter->result6 = getbuf6(iter);
                if (result != ISC_R_NOTIMPLEMENTED && result != ISC_R_SUCCESS)
                        goto ioctl6_failure;
        }
#endif
        if ((iter->socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("making interface scan socket: %s",
                                 strbuf);
                result = ISC_R_UNEXPECTED;
                goto socket_failure;
        }
        result = getbuf4(iter);
        if (result != ISC_R_SUCCESS)
                goto ioctl_failure;

        /*
         * A newly created iterator has an undefined position
         * until isc_interfaceiter_first() is called.
         */
#ifdef __linux
        iter->proc = fopen("/proc/net/if_inet6", "r");
        iter->valid = ISC_R_FAILURE;
#endif
        iter->result = ISC_R_FAILURE;

        iter->magic = IFITER_MAGIC;
        *iterp = iter;
        return (ISC_R_SUCCESS);

 ioctl_failure:
        if (iter->buf != NULL)
                free(iter->buf);
        (void) close(iter->socket);

 socket_failure:
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        if (iter->buf6 != NULL)
                free(iter->buf6);
  ioctl6_failure:
        if (iter->socket6 != -1)
                (void) close(iter->socket6);
  socket6_failure:
#endif

        free(iter);
        return (result);
}

/*
 * Get information about the current interface to iter->current.
 * If successful, return ISC_R_SUCCESS.
 * If the interface has an unsupported address family, or if
 * some operation on it fails, return ISC_R_IGNORE to make
 * the higher-level iterator code ignore it.
 */

static isc_result_t
internal_current4(isc_interfaceiter_t *iter) {
        struct ifreq *ifrp;
        struct ifreq ifreq;
        int family;
        char strbuf[BUFSIZ];
#if !defined(HAVE_STRUCT_IF_LADDRREQ) && defined(SIOCGLIFADDR)
        struct lifreq lifreq;
#else
        char sabuf[256];
#endif
        int i, bits, prefixlen;

        REQUIRE(VALID_IFITER(iter));

        if (iter->ifc.ifc_len == 0 ||
            iter->pos == (unsigned int)iter->ifc.ifc_len) {
#ifdef __linux
                return (linux_if_inet6_current(iter));
#else
                return (ISC_R_NOMORE);
#endif
        }

        INSIST( iter->pos < (unsigned int) iter->ifc.ifc_len);

        ifrp = (void *)((char *) iter->ifc.ifc_req + iter->pos);

        memset(&ifreq, 0, sizeof(ifreq));
        memcpy(&ifreq, ifrp, sizeof(ifreq));

        family = ifreq.ifr_addr.sa_family;
        if (family != AF_INET && family != AF_INET6)
                return (ISC_R_IGNORE);

        memset(&iter->current, 0, sizeof(iter->current));
        iter->current.af = family;

        INSIST(sizeof(ifreq.ifr_name) <= sizeof(iter->current.name));
        memset(iter->current.name, 0, sizeof(iter->current.name));
        memcpy(iter->current.name, ifreq.ifr_name, sizeof(ifreq.ifr_name));

        get_addr(family, &iter->current.address,
                 (struct sockaddr *)&ifrp->ifr_addr, ifreq.ifr_name);

        /*
         * If the interface does not have a address ignore it.
         */
        switch (family) {
        case AF_INET:
                if (iter->current.address.type.in.s_addr == htonl(INADDR_ANY))
                        return (ISC_R_IGNORE);
                break;
        case AF_INET6:
                if (memcmp(&iter->current.address.type.in6, &in6addr_any,
                           sizeof(in6addr_any)) == 0)
                        return (ISC_R_IGNORE);
                break;
        }

        /*
         * Get interface flags.
         */

        iter->current.flags = 0;

        /*
         * Ignore the HP/UX warning about "integer overflow during
         * conversion.  It comes from its own macro definition,
         * and is really hard to shut up.
         */
        if (isc_ioctl(iter->socket, SIOCGIFFLAGS, (char *) &ifreq) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("%s: getting interface flags: %s",
                                 ifreq.ifr_name, strbuf);
                return (ISC_R_IGNORE);
        }

        if ((ifreq.ifr_flags & IFF_UP) != 0)
                iter->current.flags |= INTERFACE_F_UP;

#ifdef IFF_POINTOPOINT
        if ((ifreq.ifr_flags & IFF_POINTOPOINT) != 0)
                iter->current.flags |= INTERFACE_F_POINTTOPOINT;
#endif

        if ((ifreq.ifr_flags & IFF_LOOPBACK) != 0)
                iter->current.flags |= INTERFACE_F_LOOPBACK;

        if ((ifreq.ifr_flags & IFF_BROADCAST) != 0)
                iter->current.flags |= INTERFACE_F_BROADCAST;

#ifdef IFF_MULTICAST
        if ((ifreq.ifr_flags & IFF_MULTICAST) != 0)
                iter->current.flags |= INTERFACE_F_MULTICAST;
#endif

        if (family == AF_INET)
                goto inet;

        memset(&lifreq, 0, sizeof(lifreq));
        memcpy(lifreq.lifr_name, iter->current.name, sizeof(lifreq.lifr_name));
        memcpy(&lifreq.lifr_addr, &iter->current.address.type.in6,
               sizeof(iter->current.address.type.in6));

        if (isc_ioctl(iter->socket, SIOCGLIFADDR, &lifreq) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("%s: getting interface address: %s",
                                 ifreq.ifr_name, strbuf);
                return (ISC_R_IGNORE);
        }
        prefixlen = lifreq.lifr_addrlen;

        /*
         * Netmask already zeroed.
         */
        iter->current.netmask.family = family;
        for (i = 0; i < 16; i++) {
                if (prefixlen > 8) {
                        bits = 0;
                        prefixlen -= 8;
                } else {
                        bits = 8 - prefixlen;
                        prefixlen = 0;
                }
                iter->current.netmask.type.in6.s6_addr[i] = (~0 << bits) & 0xff;
        }
        iter->current.ifindex = if_nametoindex(iter->current.name);
        return (ISC_R_SUCCESS);

 inet:
        if (family != AF_INET)
                return (ISC_R_IGNORE);
#ifdef IFF_POINTOPOINT
        /*
         * If the interface is point-to-point, get the destination address.
         */
        if ((iter->current.flags & INTERFACE_F_POINTTOPOINT) != 0) {
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion.  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(iter->socket, SIOCGIFDSTADDR, (char *)&ifreq)
                    < 0) {
                        strerror_r(errno, strbuf, sizeof(strbuf));
                        UNEXPECTED_ERROR("%s: getting destination address: %s",
                                         ifreq.ifr_name, strbuf);
                        return (ISC_R_IGNORE);
                }
                get_addr(family, &iter->current.dstaddress,
                         (struct sockaddr *)&ifreq.ifr_dstaddr, ifreq.ifr_name);
        }
#endif

        if ((iter->current.flags & INTERFACE_F_BROADCAST) != 0) {
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion.  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(iter->socket, SIOCGIFBRDADDR, (char *)&ifreq)
                    < 0) {
                        strerror_r(errno, strbuf, sizeof(strbuf));
                        UNEXPECTED_ERROR("%s: getting broadcast address: %s",
                                         ifreq.ifr_name, strbuf);
                        return (ISC_R_IGNORE);
                }
                get_addr(family, &iter->current.broadcast,
                         (struct sockaddr *)&ifreq.ifr_broadaddr, ifreq.ifr_name);
        }

        /*
         * Get the network mask.
         */
        memset(&ifreq, 0, sizeof(ifreq));
        memcpy(&ifreq, ifrp, sizeof(ifreq));
        /*
         * Ignore the HP/UX warning about "integer overflow during
         * conversion.  It comes from its own macro definition,
         * and is really hard to shut up.
         */
        if (isc_ioctl(iter->socket, SIOCGIFNETMASK, (char *)&ifreq) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("%s: getting netmask: %s",
                                 ifreq.ifr_name, strbuf);
                return (ISC_R_IGNORE);
        }
        get_addr(family, &iter->current.netmask,
                 (struct sockaddr *)&ifreq.ifr_addr, ifreq.ifr_name);
        iter->current.ifindex = if_nametoindex(iter->current.name);
        return (ISC_R_SUCCESS);
}

#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
static isc_result_t
internal_current6(isc_interfaceiter_t *iter) {
        struct LIFREQ *ifrp;
        struct LIFREQ lifreq;
        int family;
        char strbuf[BUFSIZ];
        int fd;

        REQUIRE(VALID_IFITER(iter));
        if (iter->result6 != ISC_R_SUCCESS)
                return (iter->result6);
        REQUIRE(iter->pos6 < (unsigned int) iter->lifc.lifc_len);

        ifrp = (void *)((char *)iter->lifc.lifc_req + iter->pos6);

        memset(&lifreq, 0, sizeof(lifreq));
        memcpy(&lifreq, ifrp, sizeof(lifreq));

        family = lifreq.lifr_addr.ss_family;
        if (family != AF_INET && family != AF_INET6)
                return (ISC_R_IGNORE);

        memset(&iter->current, 0, sizeof(iter->current));
        iter->current.af = family;

        INSIST(sizeof(lifreq.lifr_name) <= sizeof(iter->current.name));
        memset(iter->current.name, 0, sizeof(iter->current.name));
        memcpy(iter->current.name, lifreq.lifr_name, sizeof(lifreq.lifr_name));

        get_addr(family, &iter->current.address,
                 (struct sockaddr *)&lifreq.lifr_addr, lifreq.lifr_name);

        if (isc_netaddr_islinklocal(&iter->current.address))
                isc_netaddr_setzone(&iter->current.address,
                                    (uint32_t)lifreq.lifr_index);

        /*
         * If the interface does not have a address ignore it.
         */
        switch (family) {
        case AF_INET:
                if (iter->current.address.type.in.s_addr == htonl(INADDR_ANY))
                        return (ISC_R_IGNORE);
                break;
        case AF_INET6:
                if (memcmp(&iter->current.address.type.in6, &in6addr_any,
                           sizeof(in6addr_any)) == 0)
                        return (ISC_R_IGNORE);
                break;
        }

        /*
         * Get interface flags.
         */

        iter->current.flags = 0;

        if (family == AF_INET6)
                fd = iter->socket6;
        else
                fd = iter->socket;

        /*
         * Ignore the HP/UX warning about "integer overflow during
         * conversion.  It comes from its own macro definition,
         * and is really hard to shut up.
         */
        if (isc_ioctl(fd, SIOCGLIFFLAGS, (char *) &lifreq) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("%s: getting interface flags: %s",
                                 lifreq.lifr_name, strbuf);
                return (ISC_R_IGNORE);
        }

        if ((lifreq.lifr_flags & IFF_UP) != 0)
                iter->current.flags |= INTERFACE_F_UP;

#ifdef IFF_POINTOPOINT
        if ((lifreq.lifr_flags & IFF_POINTOPOINT) != 0)
                iter->current.flags |= INTERFACE_F_POINTTOPOINT;
#endif

        if ((lifreq.lifr_flags & IFF_LOOPBACK) != 0)
                iter->current.flags |= INTERFACE_F_LOOPBACK;

        if ((lifreq.lifr_flags & IFF_BROADCAST) != 0) {
                iter->current.flags |= INTERFACE_F_BROADCAST;
        }

#ifdef IFF_MULTICAST
        if ((lifreq.lifr_flags & IFF_MULTICAST) != 0) {
                iter->current.flags |= INTERFACE_F_MULTICAST;
        }
#endif

#ifdef IFF_POINTOPOINT
        /*
         * If the interface is point-to-point, get the destination address.
         */
        if ((iter->current.flags & INTERFACE_F_POINTTOPOINT) != 0) {
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion.  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(fd, SIOCGLIFDSTADDR, (char *)&lifreq)
                    < 0) {
                        strerror_r(errno, strbuf, sizeof(strbuf));
                        UNEXPECTED_ERROR("%s: getting destination address: %s",
                                         lifreq.lifr_name, strbuf);
                        return (ISC_R_IGNORE);
                }
                get_addr(family, &iter->current.dstaddress,
                         (struct sockaddr *)&lifreq.lifr_dstaddr,
                         lifreq.lifr_name);
        }
#endif

#ifdef SIOCGLIFBRDADDR
        if ((iter->current.flags & INTERFACE_F_BROADCAST) != 0) {
                /*
                 * Ignore the HP/UX warning about "integer overflow during
                 * conversion.  It comes from its own macro definition,
                 * and is really hard to shut up.
                 */
                if (isc_ioctl(iter->socket, SIOCGLIFBRDADDR, (char *)&lifreq)
                    < 0) {
                        strerror_r(errno, strbuf, sizeof(strbuf));
                        UNEXPECTED_ERROR("%s: getting broadcast address: %s",
                                         lifreq.lifr_name, strbuf);
                        return (ISC_R_IGNORE);
                }
                get_addr(family, &iter->current.broadcast,
                         (struct sockaddr *)&lifreq.lifr_broadaddr,
                         lifreq.lifr_name);
        }
#endif  /* SIOCGLIFBRDADDR */

        /*
         * Get the network mask.  Netmask already zeroed.
         */
        memset(&lifreq, 0, sizeof(lifreq));
        memcpy(&lifreq, ifrp, sizeof(lifreq));

#ifdef lifr_addrlen
        /*
         * Special case: if the system provides lifr_addrlen member, the
         * netmask of an IPv6 address can be derived from the length, since
         * an IPv6 address always has a contiguous mask.
         */
        if (family == AF_INET6) {
                int i, bits;

                iter->current.netmask.family = family;
                for (i = 0; i < lifreq.lifr_addrlen; i += 8) {
                        bits = lifreq.lifr_addrlen - i;
                        bits = (bits < 8) ? (8 - bits) : 0;
                        iter->current.netmask.type.in6.s6_addr[i / 8] =
                                (~0 << bits) & 0xff;
                }
                iter->current.ifindex = if_nametoindex(iter->current.name);
                return (ISC_R_SUCCESS);
        }
#endif

        /*
         * Ignore the HP/UX warning about "integer overflow during
         * conversion.  It comes from its own macro definition,
         * and is really hard to shut up.
         */
        if (isc_ioctl(fd, SIOCGLIFNETMASK, (char *)&lifreq) < 0) {
                strerror_r(errno, strbuf, sizeof(strbuf));
                UNEXPECTED_ERROR("%s: getting netmask: %s",
                                 lifreq.lifr_name, strbuf);
                return (ISC_R_IGNORE);
        }
        get_addr(family, &iter->current.netmask,
                 (struct sockaddr *)&lifreq.lifr_addr, lifreq.lifr_name);

        iter->current.ifindex = if_nametoindex(iter->current.name);
        return (ISC_R_SUCCESS);
}
#endif

static isc_result_t
internal_current(isc_interfaceiter_t *iter) {
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        if (iter->mode == 6) {
                iter->result6 = internal_current6(iter);
                if (iter->result6 != ISC_R_NOMORE)
                        return (iter->result6);
        }
#endif
        return (internal_current4(iter));
}

/*
 * Step the iterator to the next interface.  Unlike
 * isc_interfaceiter_next(), this may leave the iterator
 * positioned on an interface that will ultimately
 * be ignored.  Return ISC_R_NOMORE if there are no more
 * interfaces, otherwise ISC_R_SUCCESS.
 */
static isc_result_t
internal_next4(isc_interfaceiter_t *iter) {
#ifdef ISC_PLATFORM_HAVESALEN
        struct ifreq *ifrp;
#endif

        if (iter->pos < (unsigned int) iter->ifc.ifc_len) {
#ifdef ISC_PLATFORM_HAVESALEN
                ifrp = (struct ifreq *)((char *) iter->ifc.ifc_req + iter->pos);

                if (ifrp->ifr_addr.sa_len > sizeof(struct sockaddr))
                        iter->pos += sizeof(ifrp->ifr_name) +
                                     ifrp->ifr_addr.sa_len;
                else
#endif
                        iter->pos += sizeof(struct ifreq);

        } else {
                INSIST(iter->pos == (unsigned int) iter->ifc.ifc_len);
#ifdef __linux
                return (linux_if_inet6_next(iter));
#else
                return (ISC_R_NOMORE);
#endif
        }
        return (ISC_R_SUCCESS);
}

#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
static isc_result_t
internal_next6(isc_interfaceiter_t *iter) {
#ifdef ISC_PLATFORM_HAVESALEN
        struct LIFREQ *ifrp;
#endif

        if (iter->result6 != ISC_R_SUCCESS && iter->result6 != ISC_R_IGNORE)
                return (iter->result6);

        REQUIRE(iter->pos6 < (unsigned int) iter->lifc.lifc_len);

#ifdef ISC_PLATFORM_HAVESALEN
        ifrp = (struct LIFREQ *)((char *) iter->lifc.lifc_req + iter->pos6);

        if (ifrp->lifr_addr.sa_len > sizeof(struct sockaddr))
                iter->pos6 += sizeof(ifrp->lifr_name) + ifrp->lifr_addr.sa_len;
        else
#endif
                iter->pos6 += sizeof(struct LIFREQ);

        if (iter->pos6 >= (unsigned int) iter->lifc.lifc_len)
                return (ISC_R_NOMORE);

        return (ISC_R_SUCCESS);
}
#endif

static isc_result_t
internal_next(isc_interfaceiter_t *iter) {
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        if (iter->mode == 6) {
                iter->result6 = internal_next6(iter);
                if (iter->result6 != ISC_R_NOMORE)
                        return (iter->result6);
                if (iter->first6) {
                        iter->first6 = false;
                        return (ISC_R_SUCCESS);
                }
        }
#endif
        return (internal_next4(iter));
}

static void
internal_destroy(isc_interfaceiter_t *iter) {
        (void) close(iter->socket);
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        if (iter->socket6 != -1)
                (void) close(iter->socket6);
        if (iter->buf6 != NULL) {
                free(iter->buf6);
        }
#endif
#ifdef __linux
        if (iter->proc != NULL)
                fclose(iter->proc);
#endif
}

static
void internal_first(isc_interfaceiter_t *iter) {
        iter->pos = 0;
#if defined(SIOCGLIFCONF) && defined(SIOCGLIFADDR)
        iter->pos6 = 0;
        if (iter->result6 == ISC_R_NOMORE)
                iter->result6 = ISC_R_SUCCESS;
        iter->first6 = true;
#endif
#ifdef __linux
        linux_if_inet6_first(iter);
#endif
}
/* end of "ifiter_ioctl" */
#endif
/* end of the big 3 way switch */

#ifdef __linux
static void
isc_netaddr_fromin6(isc_netaddr_t *netaddr, const struct in6_addr *ina6);

static void
isc_netaddr_fromin6(isc_netaddr_t *netaddr, const struct in6_addr *ina6) {
        memset(netaddr, 0, sizeof(*netaddr));
        netaddr->family = AF_INET6;
        netaddr->type.in6 = *ina6;
}

/*
 * Returns #true if the address is a link local address.
 */
static bool
isc_netaddr_islinklocal(isc_netaddr_t *na) __attribute__((pure));

static bool
isc_netaddr_islinklocal(isc_netaddr_t *na) {
        switch (na->family) {
        case AF_INET:
                return (false);
        case AF_INET6:
                return (ISC_TF(IN6_IS_ADDR_LINKLOCAL(&na->type.in6)));
        default:
                return (false);
        }
}

static void
linux_if_inet6_first(isc_interfaceiter_t *iter) {
        if (iter->proc != NULL) {
                rewind(iter->proc);
                (void)linux_if_inet6_next(iter);
        } else
                iter->valid = ISC_R_NOMORE;
}

static isc_result_t
linux_if_inet6_next(isc_interfaceiter_t *iter) {
        if (iter->proc != NULL &&
            fgets(iter->entry, sizeof(iter->entry), iter->proc) != NULL)
                iter->valid = ISC_R_SUCCESS;
        else
                iter->valid = ISC_R_NOMORE;
        return (iter->valid);
}

static isc_result_t
linux_if_inet6_current(isc_interfaceiter_t *iter) {
        char address[33];
        char name[IF_NAMESIZE+1];
        struct in6_addr addr6;
        unsigned int ifindex;
        int prefix, scope, flags;
        int res;
        unsigned int i;

        if (iter->valid != ISC_R_SUCCESS)
                return (iter->valid);
        if (iter->proc == NULL) {
/*              isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
                              ISC_LOGMODULE_INTERFACE, ISC_LOG_ERROR,
                              "/proc/net/if_inet6:iter->proc == NULL");
*/
                return (ISC_R_FAILURE);
        }

        res = sscanf(iter->entry, "%32[a-f0-9] %x %x %x %x %16s\n",
                     address, &ifindex, (unsigned int *)&prefix,
                     (unsigned int *) &scope,
                     (unsigned int *)&flags, name);
        if (res != 6) {
/*              isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
                              ISC_LOGMODULE_INTERFACE, ISC_LOG_ERROR,
                              "/proc/net/if_inet6:sscanf() -> %d (expected 6)",
                              res);
*/
                return (ISC_R_FAILURE);
        }
        if (strlen(address) != 32) {
/*              isc_log_write(isc_lctx, ISC_LOGCATEGORY_GENERAL,
                              ISC_LOGMODULE_INTERFACE, ISC_LOG_ERROR,
                              "/proc/net/if_inet6:strlen(%s) != 32", address);
*/
                return (ISC_R_FAILURE);
        }
        /*
        ** Ignore DAD addresses --
        ** we can't bind to them until they are resolved
        */
#ifdef IFA_F_TENTATIVE
        if (flags & IFA_F_TENTATIVE)
                return (ISC_R_IGNORE);
#endif

        for (i = 0; i < 16; i++) {
                unsigned char byte;
                static const char hex[] = "0123456789abcdef";
                byte = ((strchr(hex, address[i * 2]) - hex) << 4) |
                       (strchr(hex, address[i * 2 + 1]) - hex);
                addr6.s6_addr[i] = byte;
        }
        iter->current.af = AF_INET6;
        iter->current.flags = INTERFACE_F_UP;
        isc_netaddr_fromin6(&iter->current.address, &addr6);
        iter->current.ifindex = ifindex;
        if (isc_netaddr_islinklocal(&iter->current.address)) {
                isc_netaddr_setzone(&iter->current.address,
                                    (uint32_t)ifindex);
        }
        for (i = 0; i < 16; i++) {
                if (prefix > 8) {
                        addr6.s6_addr[i] = 0xff;
                        prefix -= 8;
                } else {
                        addr6.s6_addr[i] = (0xff << (8 - prefix)) & 0xff;
                        prefix = 0;
                }
        }
        isc_netaddr_fromin6(&iter->current.netmask, &addr6);
        strlcpy(iter->current.name, name, sizeof(iter->current.name));
        return (ISC_R_SUCCESS);
}
#endif

/*
 * The remaining code is common to the sysctl and ioctl case.
 */

isc_result_t
isc_interfaceiter_current(isc_interfaceiter_t *iter,
                          isc_interface_t *ifdata)
{
        REQUIRE(iter->result == ISC_R_SUCCESS);
        memcpy(ifdata, &iter->current, sizeof(*ifdata));
        return (ISC_R_SUCCESS);
}

isc_result_t
isc_interfaceiter_first(isc_interfaceiter_t *iter) {
        isc_result_t result;

        REQUIRE(VALID_IFITER(iter));

        internal_first(iter);
        for (;;) {
                result = internal_current(iter);
                if (result != ISC_R_IGNORE)
                        break;
                result = internal_next(iter);
                if (result != ISC_R_SUCCESS)
                        break;
        }
        iter->result = result;
        return (result);
}

isc_result_t
isc_interfaceiter_next(isc_interfaceiter_t *iter) {
        isc_result_t result;

        REQUIRE(VALID_IFITER(iter));
        REQUIRE(iter->result == ISC_R_SUCCESS);

        for (;;) {
                result = internal_next(iter);
                if (result != ISC_R_SUCCESS)
                        break;
                result = internal_current(iter);
                if (result != ISC_R_IGNORE)
                        break;
        }
        iter->result = result;
        return (result);
}

void
isc_interfaceiter_destroy(isc_interfaceiter_t **iterp)
{
        isc_interfaceiter_t *iter;
        REQUIRE(iterp != NULL);
        iter = *iterp;
        REQUIRE(VALID_IFITER(iter));

        internal_destroy(iter);
        if (iter->buf != NULL)
                free(iter->buf);

        iter->magic = 0;
        free(iter);
        *iterp = NULL;
}
