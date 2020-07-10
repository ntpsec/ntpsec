/*
 * pymodule-mac.h -- hack for moving routines that call OpenSSL
 * out of pymodule.c because of shadow warnings for freefunc
 *
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 */

#ifndef GUARD_PYMODULE_MAC_H
#define GUARD_PYMODULE_MAC_H

#include <stddef.h>
#include <stdbool.h>

/* can't include Python.h or anything from openssl */

#define MAX_MAC_LENGTH 64

int do_checkname(const char *name);

void do_mac(char *name,
        uint8_t *data, size_t datalen,
        uint8_t *key, size_t keylen,
        uint8_t mac[MAX_MAC_LENGTH], size_t *maclen);

#endif /* GUARD_PYMODULE_MAC_H */
