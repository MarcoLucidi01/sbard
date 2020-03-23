#include <stdio.h>
#include <stdlib.h>

#include "sbard.h"

enum {
        ACON      = '+',
        ACOFF     = '-',
        ACUNKNOWN = '?',
};

static char acstatus(const char *acfname);
static int sumcapacity(char **capfnames);

char *battery(const void *arg, char *buf, size_t size)
{
        const BatConfig *bat = arg;

        char ac = acstatus(bat->ac);
        int perc = sumcapacity(bat->cap);
        if (perc < 0)
                return NULL;

        if (ac != ACON && perc <= bat->critical)
                sh(bat->criticalcmd, buf, size);

        const char *fmt = (perc > bat->low || ac == ACON) ? "%c%u%%" : "(!!! %c%u%% !!!)";
        snprintf(buf, size, fmt, ac, perc);
        return buf;
}

static char acstatus(const char *acfname)
{
        FILE *f = fopen(acfname, "r");
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

static int sumcapacity(char **capfnames)
{
        int sum = 0;
        for (int i = 0; capfnames[i] != NULL; i++) {
                FILE *f = fopen(capfnames[i], "r");
                if (f == NULL)
                        return -1;

                int perc;
                int ret = fscanf(f, "%d", &perc);
                fclose(f);
                if (ret == EOF || ret != 1 || perc < 0)
                        return -1;

                sum += perc;
        }
        return sum;
}
