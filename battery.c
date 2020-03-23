#include <stdio.h>
#include <stdlib.h>

#include "sbard.h"

enum {
        ACON      = '+',
        ACOFF     = '-',
        ACUNKNOWN = '?',
};

static char acstatus(const char *);
static int sumcapacity(char **);

char *battery(const void *arg, char *buf, size_t size)
{
        const BatConfig *bat = arg;

        char ac = acstatus(bat->ac);
        int cap = sumcapacity(bat->cap);
        if (cap < 0)
                return NULL;

        if (ac != ACON && cap <= bat->critical)
                sh(bat->criticalcmd, buf, size);

        const char *fmt = (cap > bat->low || ac == ACON) ? "%c%d%%" : "(!!! %c%d%% !!!)";
        snprintf(buf, size, fmt, ac, cap);
        return buf;
}

static char acstatus(const char *acfpath)
{
        FILE *f = fopen(acfpath, "r");
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

static int sumcapacity(char **capfpaths)
{
        int sum = 0;
        for (int i = 0; capfpaths[i] != NULL; i++) {
                FILE *f = fopen(capfpaths[i], "r");
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
