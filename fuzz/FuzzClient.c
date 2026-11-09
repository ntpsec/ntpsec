/*
 * FuzzClient.c - OSS-Fuzz harness for NTS-KE client response processing
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to https://tools.ietf.org/html/rfc8915
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ntpd.h"

#define kMinInputLength 20
#define kMaxInputLength 1024

bool nts_client_process_response_core(uint8_t *buff, int transferred, struct peer* peer);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < kMinInputLength || Size > kMaxInputLength) {
        return 0;
    }

    /* The parser may write into the buffer, so give it a writable copy
     * instead of libFuzzer's read-only Data. */
    uint8_t *buff = malloc(Size);
    if (NULL == buff) {
        return 0;
    }
    memcpy(buff, Data, Size);

    struct peer peer;

    ZERO(peer);    /* Don't feed uninitialized memory to the parser */

    peer.nts_state.aead = 42;    /* Dummy init values */
    peer.nts_state.cookielen = 0;
    peer.nts_state.writeIdx = 0;
    peer.nts_state.count = 0;

    peer.srcadr.sa4.sin_family = AF_INET;
    peer.srcadr.sa4.sin_port = htons(9999);
    peer.srcadr.sa4.sin_addr.s_addr = htonl(0x04030201);

    (void)nts_client_process_response_core(buff, Size, &peer);

    free(buff);

    return 0;
}
