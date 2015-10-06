
#include "core.h"
#include "randombytes.h"
#include "utils.h"

static int initialized;

int
sodium_init(void)
{
    if (initialized != 0) {
        return 1;
    }
    randombytes_stir();

    initialized = 1;
    return 0;
}
