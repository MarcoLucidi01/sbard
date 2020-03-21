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

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static char buf[512];   /* shared working and output buffer */

const char *memused(const void *arg)
{
        (void)arg;

        snprintf(buf, sizeof(buf), "/proc/meminfo");
        FILE *f = fopen(buf, "r");
        if (f == NULL)
                return NULL;

        unsigned long total, free, available, buffers, cached;
        int ret = fscanf(f, "MemTotal: %lu kB\n"
                            "MemFree: %lu kB\n"
                            "MemAvailable: %lu kB\n"
                            "Buffers: %lu kB\n"
                            "Cached: %lu kB\n",
                            &total, &free, &available, &buffers, &cached);
        fclose(f);
        if (ret == EOF || ret != 5)
                return NULL;

        unsigned long usedmb = (total - free - buffers - cached) / 1024;
        if (usedmb < 1024)
                snprintf(buf, sizeof(buf), "%luM", usedmb);
        else
                snprintf(buf, sizeof(buf), "%.2fG", (double)usedmb / 1024.0);

        return buf;
}

/* code adapted from https://stackoverflow.com/a/7661176 */
const char *volperc(const void *arg)
{
        (void)arg;

        snd_mixer_t *mixer = NULL;
        if (snd_mixer_open(&mixer, 0) < 0)
                goto error;
        if (snd_mixer_attach(mixer, "default") < 0)
                goto error;
        if (snd_mixer_selem_register(mixer, NULL, NULL) < 0)
                goto error;
        if (snd_mixer_load(mixer) < 0)
                goto error;

        snd_mixer_selem_id_t *sid = NULL;
        if (snd_mixer_selem_id_malloc(&sid) < 0)
                goto error;
        snd_mixer_selem_id_set_index(sid, 0);
        snd_mixer_selem_id_set_name(sid, "Master");

        snd_mixer_elem_t *elem = snd_mixer_find_selem(mixer, sid);
        if (elem == NULL)
                goto error;

        int notmute;
        if (snd_mixer_selem_get_playback_switch(elem, 0, &notmute) < 0)
                goto error;

        if (notmute) {
                long min, max, vol;
                snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
                if (snd_mixer_selem_get_playback_volume(elem, 0, &vol) < 0)
                        goto error;

                /* bound vol to 0-100 */
                vol -= min;
                max -= min;
                vol = 100 * vol / max;
                snprintf(buf, sizeof(buf), "%ld%%", vol);
        } else
                snprintf(buf, sizeof(buf), "--");

        snd_mixer_close(mixer);
        snd_mixer_selem_id_free(sid);
        snd_config_update_free_global();

        return buf;
error:
        if (mixer != NULL)
                snd_mixer_close(mixer);
        if (sid != NULL)
                snd_mixer_selem_id_free(sid);
        snd_config_update_free_global();
        return NULL;
}

const char *acstatus(const void *arg)
{
        const char *name = arg;

        snprintf(buf, sizeof(buf), "/sys/class/power_supply/%s/online", name);
        FILE *f = fopen(buf, "r");
        if (f == NULL)
                return NULL;

        buf[0] = fgetc(f) == '1' ? '+' : '-';
        buf[1] = '\0';

        fclose(f);
        return buf;
}

const char *sumbatperc(const void *arg)
{
        char *const *names = arg;

        int sum = 0;
        for (int i = 0; names[i] != NULL; i++) {
                snprintf(buf, sizeof(buf), "/sys/class/power_supply/%s/capacity", names[i]);
                FILE *f = fopen(buf, "r");
                if (f == NULL)
                        return NULL;
                if (fgets(buf, sizeof(buf), f) != NULL)
                        sum += atoi(buf);
                fclose(f);
        }

        snprintf(buf, sizeof(buf), "%u%%", sum);
        return buf;
}

const char *datetime(const void *arg)
{
        const char *fmt = arg;

        time_t now = time(NULL);
        if (strftime(buf, sizeof(buf), fmt, localtime(&now)) == 0)
                return NULL;

        return buf;
}
