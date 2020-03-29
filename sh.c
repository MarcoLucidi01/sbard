/* see LICENSE file for copyright and license details */

#include <stdio.h>
#include <string.h>

char *sh(const void *arg, char *buf, size_t size)
{
        FILE *f = popen(arg, "r");
        if (f == NULL)
                return NULL;

        char *ret = fgets(buf, size, f);
        pclose(f);
        if (ret != NULL) {
                char *p = strchr(buf, '\n');
                if (p != NULL)
                        *p = '\0';
        }

        return buf;
}
