#pragma once

#include "Module.h"
#include "SDL2/SDL_mixer.h"
#include <list>
#include "pugixml.hpp"
using namespace pugi;

#define DEFAULT_MUSIC_FADE_TIME 2.0f

struct _Mix_Music;

class Audio : public Module
{
public:

	Audio();

	// Destructor
	virtual ~Audio();

	// Called before render is available
	bool Awake();

	bool Start();

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	// Play a music file
	bool PlayMusic(const char* path, float fadeTime = DEFAULT_MUSIC_FADE_TIME);

	// Load a WAV in memory
	int LoadFx(const char* path);

	// Play a previously loaded WAV
	bool PlayFx(int fx, int repeat = 0);

	void ChangeGeneralVolume(float volume);
	float GetGeneralVolume();
	void ChangeSfxVolume(float volume);
	float GetSfxVolume();
	void ChangeMusicVolume(float volume);
	float GetMusicVolume();

	void SetMasterVolume(float general_volume);
private:

	_Mix_Music* music;
	std::list<Mix_Chunk*> fx;

protected:

	float general_volume = 0;
	float sfx_volume = 0;
	float music_volume = 0;
};
