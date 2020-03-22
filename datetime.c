#include <time.h>

#include "sbard.h"

char *datetime(const void *arg, char *buf, size_t size)
{
        const char *fmt = arg;

        time_t now = time(NULL);
        if (strftime(buf, size, fmt, localtime(&now)) == 0)
                return NULL;

        return buf;
}
