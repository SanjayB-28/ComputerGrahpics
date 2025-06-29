#include "CSCIx229.h"
#include "sound.h"

static Mix_Music* ambienceMusic = NULL;
static int soundInitialized = 0;

// Initialize SDL2 audio system and load ambient music
int soundSystemInit() {
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 0;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        return 0;
    }
    ambienceMusic = Mix_LoadMUS("sounds/forest-ambience.mp3");
    if (!ambienceMusic) {
        fprintf(stderr, "Failed to load ambience music: %s\n", Mix_GetError());
        return 0;
    }
    soundInitialized = 1;
    return 1;
}

// Start playing ambient background music in loop
void soundSystemPlayAmbience() {
    if (soundInitialized && ambienceMusic) {
        Mix_PlayMusic(ambienceMusic, -1);
    }
}

void soundSystemStopAmbience() {
    Mix_HaltMusic();
}

void soundSystemCleanup() {
    if (ambienceMusic) {
        Mix_FreeMusic(ambienceMusic);
        ambienceMusic = NULL;
    }
    Mix_CloseAudio();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    soundInitialized = 0;
} 