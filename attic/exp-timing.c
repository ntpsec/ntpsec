/* Hack to time exponential decay calculations.
 *
 * exp() and expf() are used to calculate the score for rate limiting.
 * expf() is used in the mainline path.
 * exp/expf are used to limit logging.
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define UNUSED_ARG(arg)         ((void)(arg))

int NUM = 1000000;
int STEPS = 1000;

/*******************************************************************/

static void DoExp(void) {
    struct timespec start, stop;
    double average;
    float x = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i=0; i<NUM; i++) {
       x = 1000.0;
       for (int j=0; j<STEPS; j++)
           x *= exp(-j/64);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    average = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
    average = average/NUM/STEPS;
    printf("exp:  %8d    %.6f %.6f %.6f\n", (int)average, x, exp(0.0), exp(-1.0));
}

static void DoExpf(void) {
    struct timespec start, stop;
    double average;
    float x = 0;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i=0; i<NUM; i++) {
       x = 1000.0;
       for (int j=0; j<STEPS; j++)
           x *= expf(-j/64);
    }
    clock_gettime(CLOCK_MONOTONIC, &stop);
    average = (stop.tv_sec-start.tv_sec)*1E9 + (stop.tv_nsec-start.tv_nsec);
    average = average/NUM/STEPS;
    printf("expf: %8d    %.6f %.6f %.6f\n", (int)average, x, exp(0.0), exp(-1.0));
}

int main (int argc, char *argv[]) {

	UNUSED_ARG(argc);
	UNUSED_ARG(argv);

	printf("         avg ns\n");
        DoExp();
        DoExpf();

	return 0;
}
