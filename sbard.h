#ifndef SBARD_HEADER
#define SBARD_HEADER

typedef struct {
        const char      *fmt;
        char            *(*func)(const void *, char *, size_t);
        const void      *arg;
} Config;

typedef struct {
        char            **cap;
        const char      *ac;
        unsigned int    low;
        unsigned int    critical;
        const char      *criticalcmd;
} BatConfig;

char *datetime(const void *, char *, size_t);
char *memory(const void *, char *, size_t);
char *volume(const void *, char *, size_t);
char *battery(const void *, char *, size_t);

#endif
