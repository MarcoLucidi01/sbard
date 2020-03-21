#define NOTAVAILABLE "n/a"

enum {
        MAXBARLEN       = 1024,
        REFRESHINTERVAL = 60,   /* in seconds */
};

const char *memused(const void *);
const char *volperc(const void *);
const char *acstatus(const void *);
const char *sumbatperc(const void *);
const char *datetime(const void *);

struct {
        const char *fmt;
        const char *(*func)(const void *);
        const void *arg;
} info[] = {
        { "m:%s ",      memused,        NULL },
        { "v:%s ",      volperc,        NULL },
        { "b:%s",       acstatus,       "AC" },
        { "%s ",        sumbatperc,     (const char *[]){ "BAT0", "BAT1", NULL } },
        { "%s",         datetime,       "%d/%m %H:%M" },
};
