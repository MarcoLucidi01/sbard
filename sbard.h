#ifndef SBARD_HEADER
#define SBARD_HEADER

#include <stddef.h>

typedef struct {
        const char      *fmt;
        char            *(*func)(const void *, char *, size_t);
        const void      *arg;
} Config;

typedef struct {
        char            **cap;
        const char      *ac;
        int             low;
        int             critical;
        const char      *criticalcmd;
} BatConfig;

char *battery(const void *, char *, size_t);
char *datetime(const void *, char *, size_t);
char *memory(const void *, char *, size_t);
char *sh(const void *, char *, size_t);
char *volume(const void *, char *, size_t);
char *wireless(const void *, char *, size_t);

#endif
