/* see LICENSE file for copyright and license details */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "sbard.h"

typedef enum {
        ACON,
        ACOFF,
        ACUNKNOWN,
} ACState;

static ACState  acstate(const char *, char *, size_t);
static int      avgcapacity(char **, char *, size_t);

char *battery(const void *arg, char *buf, size_t size)
{
        const BatConfig *bat = arg;

        int cap = avgcapacity(bat->names, buf, size);
        if (cap < 0)
                return NULL;

        ACState ac = acstate(bat->acname, buf, size);
        if (bat->criticalcmd != NULL && ac != ACON && cap <= bat->critical)
                sh(bat->criticalcmd, buf, size);

        int i = 0;
        if (ac == ACON)
                buf[i++] = '+';
        else if (cap <= bat->low)
                while (i < (int)size - 1 && i <= bat->low - cap)
                        buf[i++] = '!';
        else if (ac == ACUNKNOWN)
                buf[i++] = '?';

        snprintf(buf + i, size - i, "%d", cap);
        return buf;
}

static ACState acstate(const char *acname, char *buf, size_t size)
{
        snprintf(buf, size, "/sys/class/power_supply/%s/online", acname);
        FILE *f = fopen(buf, "r");
        if (f == NULL)
                return ACUNKNOWN;

        int c = fgetc(f);
        fclose(f);

        if (c == '1')
                return ACON;
        if (c == '0')
                return ACOFF;
        return ACUNKNOWN;
}

static int avgcapacity(char **names, char *buf, size_t size)
{
        int sum = 0;
        int i;
        for (i = 0; names[i] != NULL; i++) {
                snprintf(buf, size, "/sys/class/power_supply/%s/capacity", names[i]);
                FILE *f = fopen(buf, "r");
                if (f == NULL)
                        return -1;
                int cap;
                int ret = fscanf(f, "%d", &cap);
                fclose(f);
                if (ret == EOF || ret != 1 || cap < 0)
                        return -1;

                sum += cap;
        }
        if (i > 1)
                return lround((double)sum / i);
        return sum;
}
