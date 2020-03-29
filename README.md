sbard
=====

status bar daemon for dwm-like window managers

I wrote this thing because I wanted an extremely lightweight status bar for [`dwm`](https://dwm.suckless.org/)
and [`tmux`](https://tmux.github.io/). I was using a shell script that spawned
6-10 process each 30 seconds and I was noticing glitches every now and then when
watching videos and movies.

yes, there is already [slstatus](https://git.suckless.org/slstatus/)
(and many others). I "stole" their whole design. You should really use [slstatus](https://git.suckless.org/slstatus/).
It's way more portable and way more well written. sbard only runs on Linux
(probably). I wrote it just for fun and for learning.

features
========

- battery: sum of the capacity of batteries and ac status
- datetime: formatted date and/or time
- memory: amount of memory used in MB or GB
- sh: first line of output of shell command
- temperature: temperature in celsius of thermal zone
- volume: percentage of playback volume fetched using alsa
- wireless: percentage of wireless link quality if connected

these are the features which I consider important and which I like to keep an
eye on while I'm doing my stuff. It also monitors for battery capacity and runs
a (configurable) command when it reaches a critical level.

install
=======

it requires `Xlib` and `libasound` ([ALSA](https://alsa-project.org/)). On
Debian:

    # apt-get install libx11-dev libasound2-dev

configuration happens inside [`config.h`](https://github.com/MarcoLucidi01/sbard/blob/master/config.h).
You must recompile the whole thing every time you want to change something.

to install inside `/usr/local/bin`:

    # make install

usage
=====

by default runs in the background as daemon and updates `WM_NAME` X11 property
of the root window periodically. You can send `SIGHUP` to refresh the bar
immediately.

with the `-o` flag, it doesn't run as daemon and writes to `stdout`, I use this
flag for [`tmux`](https://tmux.github.io/):

    $ sbard -o
    t:52°C w:100% m:1.37G v:69% b:+151% 29/03 14:29
