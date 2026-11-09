/*
 * FuzzServer.c - OSS-Fuzz harness for NTS-KE server record processing
 * Copyright the NTPsec project contributors
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Section references are to https://tools.ietf.org/html/rfc8915
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "nts.h"

#define kMinInputLength 20
#define kMaxInputLength 1024

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size);

int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size) {
    if (Size < kMinInputLength || Size > kMaxInputLength) {
        return 0;
    }

    /* The parser walks the buffer with a mutable cursor, so give it a
     * writable copy instead of libFuzzer's read-only Data. */
    uint8_t *buff = malloc(Size);
    if (NULL == buff) {
        return 0;
    }
    memcpy(buff, Data, Size);

    NTS_Server_Info nsi;
    struct BufCtl_t buf;

    memset(&nsi, 0, sizeof(nsi));
    nsi.aead = NO_AEAD;    /* Dummy init value */
    buf.next = buff;
    buf.left = Size;

    (void)nts_ke_process_receive(&nsi, &buf);

    free(buff);

    return 0;
}
