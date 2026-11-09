/*
 * FuzzExtens.c - OSS-Fuzz harness for NTS server extension-field parsing
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

    /* The parser may write into the packet, so give it a writable copy
     * instead of libFuzzer's read-only Data. */
    uint8_t *pkt = malloc(Size);
    if (NULL == pkt) {
        return 0;
    }
    memcpy(pkt, Data, Size);

    struct ntspacket_t ntspkt;
    memset(&ntspkt, 0, sizeof(ntspkt));

    (void)extens_server_recv(&ntspkt, pkt, Size);

    free(pkt);

    return 0;
}
