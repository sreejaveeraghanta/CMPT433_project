// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

#define AUDIOMIXER_MAX_VOLUME 100
#define AUDIOMIXER_MIN_VOLUME 0

// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AudioMixer_init(void);
void AudioMixer_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioMixer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioMixer_queueSound(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AudioMixer_getVolume();
void increase_volume(void);
void decrease_volume(void);
void AudioMixer_setVolume(int newVolume);

#ifdef __cplusplus
}
#endif

#endif
