/* see LICENSE file for copyright and license details */

#include <stdio.h>
#include <stdlib.h>

#include "sbard.h"

enum {
        ACON      = '+',
        ACOFF     = '-',
        ACUNKNOWN = '?',
};

static char acstatus(const char *, char *, size_t);
static int sumcapacity(char **, char *, size_t);

char *battery(const void *arg, char *buf, size_t size)
{
        const BatConfig *bat = arg;

        char ac = acstatus(bat->acname, buf, size);
        int cap = sumcapacity(bat->names, buf, size);
        if (cap < 0)
                return NULL;

        if (ac != ACON && cap <= bat->critical)
                sh(bat->criticalcmd, buf, size);

        const char *fmt = (cap > bat->low || ac == ACON) ? "%c%d%%" : "(!!! %c%d%% !!!)";
        snprintf(buf, size, fmt, ac, cap);
        return buf;
}

static char acstatus(const char *acname, char *buf, size_t size)
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

static int sumcapacity(char **names, char *buf, size_t size)
{
        int sum = 0;
        for (int i = 0; names[i] != NULL; i++) {
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
        return sum;
}
