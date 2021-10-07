/* see LICENSE file for copyright and license details */

#define NOTAVAILABLE    "N/A"   /* placeholder */

enum {
        MAXBARLEN       = 1024,
        REFRESHINTERVAL = 60,   /* in seconds */
};

/*
 * Configuration for the battery function.
 *
 * Look at /sys/class/power_supply for battery and AC names.
 * TODO battery and AC names could be easily detected at runtime.
 *
 * The function returns average capacity percentage.
 *
 *   names              NULL terminated array of batteries names
 *   acname             usually AC
 *   low                low capacity level
 *   lowcmd             command executed when low level is reached
 *   critical           critical capacity level
 *   criticalcmd        command executed when critical level is reached
 */
BatConfig batconfig = {
        .names       = (char *[]){ "BAT0", "BAT1", NULL },
        .acname      = "AC",
        .low         = 20,
        .lowcmd      = "echo",  /* placeholder */
        .critical    = 10,
        .criticalcmd = "systemctl poweroff",
};

/*
 * Bar configuration. All functions take one argument and return a string
 * containing the information to display in the bar.
 *
 *   function    return                                                 argument
 *
 *   battery     sum of the capacity of batteries and ac status         BatConfig
 *   datetime    formatted date and/or time                             format string for strftime()
 *   memory      amount of memory used in MB or GB                      NULL
 *   sh          first line of output of shell command                  shell command to run
 *   temperature temperature in celsius of thermal zone                 thermal zone name
 *   volume      percentage of playback volume fetched using alsa       NULL
 *   wireless    percentage of wireless link quality if connected       interface name
 */
Config config[] = {
        { .fmt = "t:%s ",       .func = temperature,    .arg = "thermal_zone1" },
        { .fmt = "w:%s ",       .func = wireless,       .arg = "wlp3s0" },
        { .fmt = "m:%s ",       .func = memory,         .arg = NULL },
        { .fmt = "v:%s ",       .func = volume,         .arg = NULL },
        { .fmt = "b:%s ",       .func = battery,        .arg = &batconfig },
        { .fmt = "%s",          .func = datetime,       .arg = "%d/%m %H:%M" },
};
