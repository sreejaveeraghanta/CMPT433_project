#ifndef SOUNDS_H_
#define SOUNDS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "audioMixer.h"

void sound_init(void);
void sound_play_collision(void);
void sound_play_score(void);
wavedata_t* background_music_file_address(void);
void sound_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
