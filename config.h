#define NOTAVAILABLE    "n/a"

enum {
        MAXBARLEN       = 1024,
        REFRESHINTERVAL = 60,   /* in seconds */
};

BatConfig batconfig = {
        .cap            = (char *[]){ "/sys/class/power_supply/BAT0/capacity", "/sys/class/power_supply/BAT1/capacity", NULL },
        .ac             = "/sys/class/power_supply/AC/online",
        .low            = 40,
        .critical       = 20,
        .criticalcmd    = "systemctl poweroff"
};

Config config[] = {
        { .fmt = "m:%s ",       .func = memory,         .arg = NULL },
        { .fmt = "v:%s ",       .func = volume,         .arg = NULL },
        { .fmt = "b:%s ",       .func = battery,        .arg = &batconfig },
        { .fmt = "%s",          .func = datetime,       .arg = "%d/%m %H:%M" },
};
