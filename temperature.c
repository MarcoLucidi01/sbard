/* see LICENSE file for copyright and license details */

#include <stdio.h>

char *temperature(const void *arg, char *buf, size_t size)
{
        const char *zone = arg;

        snprintf(buf, size, "/sys/class/thermal/%s/temp", zone);
        FILE *f = fopen(buf, "r");
        if (f == NULL)
                return NULL;
        int temp;
        int ret = fscanf(f, "%d\n", &temp);
        fclose(f);
        if (ret == EOF || ret != 1)
                return NULL;

        /* temperature unit is millidegree celsius */
        snprintf(buf, size, "%.0f°C", (double)temp / 1000.0);
        return buf;
}
