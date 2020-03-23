#define NOTAVAILABLE    "n/a"   /* placeholder */

enum {
        MAXBARLEN       = 1024,
        REFRESHINTERVAL = 60,   /* in seconds */
};

/*
 * Configuration for the battery function. I have 2 batteries inside my laptop,
 * that's way cap is an array and the function return a sum. low and critical
 * levels refers to the sum.
 *
 *   cap                NULL terminated array of capacity filepaths of the batteries
 *   ac                 filepath of the AC/online file
 *   low                low capacity level
 *   critical           critical capacity level
 *   criticalcmd        command executed when critical level is reached
 */
BatConfig batconfig = {
        .cap            = (char *[]){ "/sys/class/power_supply/BAT0/capacity", "/sys/class/power_supply/BAT1/capacity", NULL },
        .ac             = "/sys/class/power_supply/AC/online",
        .low            = 40,
        .critical       = 20,
        .criticalcmd    = "systemctl poweroff"
};

/*
 * Bar configuration. All functions take one argument and return a string
 * containing the information to display in the bar.
 *
 *   function   return                                                  argument
 *
 *   datetime   formatted date and/or time                              format string for strftime()
 *   memory     amount of memory used in MB or GB                       NULL
 *   volume     percentage of playback volume fetched using alsa        NULL
 *   battery    sum of the capacity of batteries and ac status          BatConfig
 *   sh         first line of output of shell command                   shell command to run
 */
Config config[] = {
        { .fmt = "m:%s ",       .func = memory,         .arg = NULL },
        { .fmt = "v:%s ",       .func = volume,         .arg = NULL },
        { .fmt = "b:%s ",       .func = battery,        .arg = &batconfig },
        { .fmt = "%s",          .func = datetime,       .arg = "%d/%m %H:%M" },
};
