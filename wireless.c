/* see LICENSE file for copyright and license details */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *wireless(const void *arg, char *buf, size_t size)
{
        const char *iface = arg;

        FILE *f = fopen("/proc/net/wireless", "r");
        if (f == NULL)
                return NULL;

        char *p;
        while ((p = fgets(buf, size, f)) != NULL)
                if (strncmp(buf, iface, strlen(iface)) == 0)
                        break;
        fclose(f);
        if (p == NULL)
                return NULL;

        /* link quality is the third field */
        p = strtok(buf, " \t");
        for (int i = 0; i < 2 && p != NULL; i++)
                p = strtok(NULL, " \t");
        if (p == NULL)
                return NULL;

        int link = atoi(p);
        if (link < 0)
                return NULL;

        /* link quality goes from 0 to 70 */
        snprintf(buf, size, "%.0f", (double)link / 70.0 * 100.0);
        return buf;
}
