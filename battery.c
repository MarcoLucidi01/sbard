#include <stdio.h>
#include <stdlib.h>

#include "sbard.h"

enum {
        ACON      = '+',
        ACOFF     = '-',
        ACUNKNOWN = '?',
};

static char acstatus(const char *acfname);

char *battery(const void *arg, char *buf, size_t size)
{
        const BatConfig *bat = arg;

        int sum = 0;
        for (int i = 0; bat->cap[i] != NULL; i++) {
                FILE *f = fopen(bat->cap[i], "r");
                if (f == NULL)
                        return NULL;
                if (fgets(buf, size, f) != NULL)
                        sum += atoi(buf);
                fclose(f);
        }

        snprintf(buf, size, "%c%u%%", acstatus(bat->ac), sum);
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
