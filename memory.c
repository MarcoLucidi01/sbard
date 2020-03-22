#include <stdio.h>

#include "sbard.h"

char *memory(const void *arg, char *buf, size_t size)
{
        (void)arg;

        FILE *f = fopen("/proc/meminfo", "r");
        if (f == NULL)
                return NULL;

        unsigned long total, free, available, buffers, cached;
        int ret = fscanf(f,
                "MemTotal: %lu kB\n"
                "MemFree: %lu kB\n"
                "MemAvailable: %lu kB\n"
                "Buffers: %lu kB\n"
                "Cached: %lu kB\n",
                &total, &free, &available, &buffers, &cached
        );
        fclose(f);
        if (ret == EOF || ret != 5)
                return NULL;

        unsigned long usedmb = (total - free - buffers - cached) / 1024;
        if (usedmb < 1024)
                snprintf(buf, size, "%luM", usedmb);
        else
                snprintf(buf, size, "%.2fG", (double)usedmb / 1024.0);

        return buf;
}
