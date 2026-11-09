/*
 * FuzzExtensClient.c - OSS-Fuzz harness for NTS client extension-field parsing
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntpd.h"
#include "nts.h"

#define kMinInputLength 20
#define kMaxInputLength 1024

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < kMinInputLength || Size > kMaxInputLength) {
        return 0;
    }

    /* extens_client_recv decrypts in place, so give it a writable copy
     * instead of libFuzzer's read-only Data. */
    uint8_t *pkt = malloc(Size);
    if (NULL == pkt) {
        return 0;
    }
    memcpy(pkt, Data, Size);

    struct peer peer;
    memset(&peer, 0, sizeof(peer));

    /* Stay reachable once upstream !1591 starts bailing out when
     * peer->outcount is 0. */
    peer.outcount = 1;

    (void)extens_client_recv(&peer, pkt, Size);

    free(pkt);

    return 0;
}
