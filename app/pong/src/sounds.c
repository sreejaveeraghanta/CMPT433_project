#include "audioMixer.h"
#include <alsa/asoundlib.h>
#include <stdbool.h>
#include <limits.h>
#include <alloca.h> // needed for mixer
#include <time.h>

#define BACKGROUND_MUSIC_FILE "wave-files/787932__edwardszakal__retro-game-music.wav"
#define COLLISION_SOUND_FILE "wave-files/582697__ironcross32__short-beep-02.wav"
#define SCORE_SOUND_FILE "wave-files/shorter_beep.wav"

#define MAX_VOLUME 100
#define MIN_VOLUME 0
#define INCREMENT 5
#define DECREMENT 5

static wavedata_t background_music_file;
static wavedata_t collision_sound_file;
static wavedata_t score_sound_file;

void sound_init(void);
void sound_play_collision(void);
void sound_play_score(void);
wavedata_t* background_music_file_address(void);
void sound_cleanup(void);

void sound_init(void)
{
    // Configure Output Device
	AudioMixer_init();

	// Load wave file we want to play:
	AudioMixer_readWaveFileIntoMemory(BACKGROUND_MUSIC_FILE, &background_music_file);
	AudioMixer_readWaveFileIntoMemory(COLLISION_SOUND_FILE, &collision_sound_file);
	AudioMixer_readWaveFileIntoMemory(SCORE_SOUND_FILE, &score_sound_file);

    AudioMixer_queueSound(&background_music_file);
}


void sound_play_collision(void)
{
    AudioMixer_queueSound(&collision_sound_file);
}

void sound_play_score(void)
{
	AudioMixer_queueSound(&score_sound_file);
}

wavedata_t* background_music_file_address(void)
{
    return &background_music_file;
}


void sound_cleanup(void)
{
    AudioMixer_cleanup();
	AudioMixer_freeWaveFileData(&score_sound_file);
	AudioMixer_freeWaveFileData(&collision_sound_file);
	AudioMixer_freeWaveFileData(&background_music_file);
}