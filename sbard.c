/* see LICENSE file for copyright and license details */

#include <X11/Xlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "sbard.h"
#include "config.h"

#define USAGE   "usage: sbard [-o]\n"                                                  \
                "status bar daemon\n"                                                  \
                "runs in the background and updates WM_NAME X11 property of the\n"     \
                "root window periodically (tip: send SIGHUP to refresh immediately)\n" \
                "configuration happens in config.h\n\n"                                \
                "  -o  don't run as daemon, write bar to stdout and exit\n\n"          \
                "repo: https://github.com/MarcoLucidi01/sbard"                         \

#define ARRAYLEN(a)     (sizeof(a) / sizeof(a[0]))

static void             run(void);
static void             stop(int);
static void             noop(int);
static const char      *mkbar(void);
static void             die(const char *);

static volatile bool    running;        /* running flag for run() loop */

int main(int argc, char **argv)
{
        if (argc == 1)
                run();
        else if (strcmp(argv[1], "-o") == 0)
                puts(mkbar());
        else
                die(USAGE);
}

static void run(void)
{
        pid_t pid = fork();     /* demonize */
        if (pid < 0)
                die("could not fork()");
        if (pid > 0)
                exit(EXIT_SUCCESS);     /* parent */
        if (setsid() < 0)
                die("could not setsid()");
        if (chdir("/") < 0)
                die("could not chdir(\"/\")");

        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = stop;
        if (sigaction(SIGINT, &sa, NULL) < 0 || sigaction(SIGTERM, &sa, NULL) < 0)
                die("could not setup signal handlers");

        sa.sa_handler = noop;   /* break from sleep() to refresh the bar */
        if (sigaction(SIGHUP, &sa, NULL) < 0)
                die("could not setup signal handlers");

        Display *dpy = XOpenDisplay(NULL);
        if (dpy == NULL)
                die("could not open display");

        umask(0);
        fclose(stdin);
        fclose(stdout);
        fclose(stderr);

        running = true;
        while (running) {
                XStoreName(dpy, DefaultRootWindow(dpy), mkbar());
                XFlush(dpy);
                sleep(REFRESHINTERVAL);
        }

        XStoreName(dpy, DefaultRootWindow(dpy), NULL);  /* restore default */
        XCloseDisplay(dpy);
}

static void stop(int sig)
{
        (void)sig;

        running = false;
}

static void noop(int sig)
{
        (void)sig;
}

static const char *mkbar(void)
{
        static char bar[MAXBARLEN];     /* buffer for the bar */
        static char buf[MAXBARLEN / 2]; /* buffer for functions that extract infos */

        int len = 0;
        int rem = sizeof(bar);
        bar[0] = '\0';

        for (unsigned int i = 0; i < ARRAYLEN(config); i++) {
                if (rem <= 0)
                        break;

                const char *s = config[i].func(config[i].arg, buf, sizeof(buf));
                if (s == NULL)
                        s = NOTAVAILABLE;

                int w = snprintf(bar + len, rem, config[i].fmt, s);
                if (w < 0)
                        return NOTAVAILABLE;

                len += w;
                rem -= w;
        }
        return bar;
}

void die(const char *reason)
{
        fprintf(stderr, "sbard: %s\n", reason);
        exit(EXIT_FAILURE);
}
