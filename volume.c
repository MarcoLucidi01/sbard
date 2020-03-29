/* see LICENSE file for copyright and license details */

#include <alsa/asoundlib.h>
#include <stddef.h>

/* code adapted from https://stackoverflow.com/a/7661176 */
char *volume(const void *arg, char *buf, size_t size)
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
                snprintf(buf, size, "%ld%%", vol);
        } else
                snprintf(buf, size, "--");

        snd_mixer_free(mixer);
        snd_mixer_close(mixer);
        snd_mixer_selem_id_free(sid);
        snd_config_update_free_global();

        return buf;
error:
        if (mixer != NULL) {
                snd_mixer_free(mixer);
                snd_mixer_close(mixer);
        }
        if (sid != NULL)
                snd_mixer_selem_id_free(sid);
        snd_config_update_free_global();
        return NULL;
}
