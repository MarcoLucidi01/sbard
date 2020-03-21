/*
 * MIT License
 *
 * Copyright (c) 2020 Marco Lucidi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <X11/Xlib.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "config.h"

#define USAGE   "usage: sbard [-o]\n\n"                                                \
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
                die("could not setup singal handlers");

        sa.sa_handler = noop;   /* break from sleep() to refresh the bar */
        if (sigaction(SIGHUP, &sa, NULL) < 0)
                die("could not setup singal handlers");

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
        static char bar[MAXBARLEN];

        int len = 0;
        int rem = sizeof(bar);

        for (unsigned int i = 0; i < ARRAYLEN(info); i++) {
                if (rem <= 0)
                        break;

                const char *s = info[i].func(info[i].arg);
                if (s == NULL)
                        s = NOTAVAILABLE;

                int w = snprintf(bar + len, rem, info[i].fmt, s);
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