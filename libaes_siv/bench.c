/* Copyright Akamai Technologies, Inc.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "config.h"
#include "aes_siv.h"

#define _POSIX_C_SOURCE 200112L

#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static const unsigned char key[64];
static const unsigned char ad[65536];
static const unsigned char nonce[16];
static unsigned char in[65536+16];
static unsigned char out[65536+16];

static const struct {
        size_t key_len;
        size_t nonce_len;
        size_t in_len;
        size_t ad_len;
} call_list[] = {
        { 32, 0, 0, 0 },
        { 32, 0, 65536, 0 },
        { 32, 0, 0, 65536 },
        { 32, 0, 65536, 65536 },
        { 32, 0, 1536, 0 },
        { 32, 0, 0, 1536 },
        { 32, 0, 1536, 1536 },
        { 32, 0, 64, 0},
        { 32, 0, 0, 64},
        { 32, 0, 64, 64},
        { 32, 16, 0, 0 },
        { 32, 16, 65536, 0 },
        { 32, 16, 0, 65536 },
        { 32, 16, 65536, 65536 },
        { 32, 16, 1536, 0 },
        { 32, 16, 0, 1536 },
        { 32, 16, 1536, 1536 },
        { 32, 16, 64, 0},
        { 32, 16, 0, 64},
        { 32, 16, 64, 64},

        { 48, 0, 0, 0 },
        { 48, 0, 65536, 0 },
        { 48, 0, 0, 65536 },
        { 48, 0, 65536, 65536 },
        { 48, 0, 1536, 0 },
        { 48, 0, 0, 1536 },
        { 48, 0, 1536, 1536 },
        { 48, 0, 64, 0},
        { 48, 0, 0, 64},
        { 48, 0, 64, 64},
        { 48, 16, 0, 0 },
        { 48, 16, 65536, 0 },
        { 48, 16, 0, 65536 },
        { 48, 16, 65536, 65536 },
        { 48, 16, 1536, 0 },
        { 48, 16, 0, 1536 },
        { 48, 16, 1536, 1536 },
        { 48, 16, 64, 0},
        { 48, 16, 0, 64},
        { 48, 16, 64, 64},

        { 64, 0, 0, 0 },
        { 64, 0, 65536, 0 },
        { 64, 0, 0, 65536 },
        { 64, 0, 65536, 65536 },
        { 64, 0, 1536, 0 },
        { 64, 0, 0, 1536 },
        { 64, 0, 1536, 1536 },
        { 64, 0, 64, 0},
        { 64, 0, 0, 64},
        { 64, 0, 64, 64},
        { 64, 16, 0, 0 },
        { 64, 16, 65536, 0 },
        { 64, 16, 0, 65536 },
        { 64, 16, 65536, 65536 },
        { 64, 16, 1536, 0 },
        { 64, 16, 0, 1536 },
        { 64, 16, 1536, 1536 },
        { 64, 16, 64, 0},
        { 64, 16, 0, 64},
        { 64, 16, 64, 64},

        { 0, 0, 0, 0 }
};

static volatile int alarm_rung;
static void alarm_handler(int num) {
  (void)num;
  alarm_rung = 1;
}

typedef int (*fn)(AES_SIV_CTX *, unsigned char *, size_t *,
                  unsigned char const*, size_t,
                  unsigned char const*, size_t,
                  unsigned char const*, size_t,
                  unsigned char const*, size_t);
static void
call(fn fn, AES_SIV_CTX *ctx,
     size_t key_len,
     size_t nonce_len, size_t in_len,
     size_t ad_len) {

        size_t out_len = sizeof out;
        size_t count;
        struct timespec start, end;
        double rate;

        printf("%3u bit key, %2u byte nonce, %5u byte input, %5u byte associated data: ",
               (unsigned)(key_len * 8), (unsigned)nonce_len,
	       (unsigned)in_len, (unsigned)ad_len);
        fflush(stdout);

        alarm_rung = 0;
        alarm(3);

        if(clock_gettime(CLOCK_MONOTONIC, &start)) {
                perror("clock_gettime");
                exit(1);
        }
        for(count = 0; !alarm_rung; count++) {
                fn(ctx, out, &out_len, key, key_len,
                   nonce_len > 0 ? nonce : NULL, nonce_len,
                   in, in_len, ad, ad_len);
        }
        if(clock_gettime(CLOCK_MONOTONIC, &end)) {
                perror("clock_gettime");
                exit(1);
        }

        rate = (double)count /
                ((double)(end.tv_sec) - (double)(start.tv_sec) +
                 ((double)end.tv_nsec - (double)start.tv_nsec)/1000000000.);
        printf("%12.2f calls/second (%11.2f ns/call, %8.2f MiB/s)\n",
               rate,
               1000000000./rate,
               scalbn(rate * (double)(ad_len + in_len),
                      -20));
}

int main(void) {
        size_t i;
        struct sigaction act;
        AES_SIV_CTX *ctx = AES_SIV_CTX_new();

        memset(&act, 0, sizeof act);
        act.sa_handler = alarm_handler;
        if(sigaction(SIGALRM, &act, NULL)) {
                perror("sigaction");
                exit(1);
        }

        for(i=0; call_list[i].key_len != 0; i++) {
                printf("Encrypt, ");
                call(AES_SIV_Encrypt, ctx,
                     call_list[i].key_len, call_list[i].nonce_len,
                     call_list[i].in_len, call_list[i].ad_len);


                printf("Decrypt, ");
                memcpy(in, out, call_list[i].in_len + 16);
                call(AES_SIV_Decrypt, ctx,
                     call_list[i].key_len, call_list[i].nonce_len,
                     call_list[i].in_len + 16, call_list[i].ad_len);

                printf("Forgery, ");
                memset(in, 0, sizeof in);
                call(AES_SIV_Decrypt, ctx,
                     call_list[i].key_len, call_list[i].nonce_len,
                     call_list[i].in_len + 16, call_list[i].ad_len);

        }

        AES_SIV_CTX_free(ctx);
        return 0;
}
