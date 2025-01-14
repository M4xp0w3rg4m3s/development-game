#include "Audio.h"
#include "Log.h"

#include "SDL2/SDL.h"
#include "SDL2/SDL_mixer.h"
#include <algorithm>

Audio::Audio() : Module()
{
	music = NULL;
	name = "audio";
}

// Destructor
Audio::~Audio()
{}

// Called before render is available
bool Audio::Awake()
{
	LOG("Loading Audio Mixer");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
		active = false;
		ret = true;
	}

	// Load support for the JPG and PNG image formats
	int flags = MIX_INIT_OGG;
	int init = Mix_Init(flags);

	if((init & flags) != flags)
	{
		LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
		active = false;
		ret = true;
	}

	// Initialize SDL_mixer
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
		active = false;
		ret = true;
	}

	return ret;
}

bool Audio::Start()
{
	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	pugi::xml_node audioNode = loadFile.child("config").child("audio");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return false;
	}

	general_volume = audioNode.child("general_volume").attribute("value").as_float();
	music_volume = audioNode.child("music_volume").attribute("value").as_float();
	sfx_volume = audioNode.child("sfx_volume").attribute("value").as_float();

	ChangeGeneralVolume(general_volume);
	ChangeMusicVolume(music_volume);
	ChangeSfxVolume(sfx_volume);

	return true;
}

bool Audio::Update(float dt)
{
	return true;
}

// Called before quitting
bool Audio::CleanUp()
{
	if(!active)
		return true;

	LOG("Freeing sound FX, closing Mixer and Audio subsystem");

	if(music != NULL)
	{
		Mix_FreeMusic(music);
	}

	for (const auto& fxItem : fx) {
		Mix_FreeChunk(fxItem);
	}
	fx.clear();

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return true;
}

// Play a music file
bool Audio::PlayMusic(const char* path, float fadeTime)
{
	bool ret = true;

	if(!active)
		return false;

	if(music != NULL)
	{
		if(fadeTime > 0.0f)
		{
			Mix_FadeOutMusic(int(fadeTime * 1000.0f));
		}
		else
		{
			Mix_HaltMusic();
		}

		// this call blocks until fade out is done
		Mix_FreeMusic(music);
	}

	music = Mix_LoadMUS(path);

	if(music == NULL)
	{
		LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
		ret = false;
	}
	else
	{
		if(fadeTime > 0.0f)
		{
			if(Mix_FadeInMusic(music, -1, (int) (fadeTime * 1000.0f)) < 0)
			{
				LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
		else
		{
			if(Mix_PlayMusic(music, -1) < 0)
			{
				LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
				ret = false;
			}
		}
	}

	LOG("Successfully playing %s", path);
	return ret;
}

// Load WAV
int Audio::LoadFx(const char* path)
{
	int ret = 0;

	if(!active)
		return 0;

	Mix_Chunk* chunk = Mix_LoadWAV(path);

	if(chunk == NULL)
	{
		LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
	}
	else
	{
		fx.push_back(chunk);
		ret = (int)fx.size();
	}

	return ret;
}

// Play WAV
bool Audio::PlayFx(int id, int repeat)
{
	bool ret = false;

	if(!active)
		return false;

	if(id > 0 && id <= fx.size())
	{
		auto fxIt = fx.begin();
		std::advance(fxIt, id-1);
		Mix_PlayChannel(-1, *fxIt, repeat);
	}

	return ret;
}


void Audio::ChangeGeneralVolume(float volume)
{
	general_volume = std::max(0.0f, std::min(volume, 1.0f)); 
	SetMasterVolume(general_volume);

	ChangeSfxVolume(sfx_volume);
	ChangeMusicVolume(music_volume);
}

float Audio::GetGeneralVolume()
{
	return general_volume;
}

void Audio::ChangeSfxVolume(float volume)
{
	sfx_volume = std::max(0.0f, std::min(volume, 1.0f));

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	pugi::xml_node audioNode = loadFile.child("config").child("audio");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	audioNode.child("sfx_volume").attribute("value").set_value(sfx_volume);

	Mix_Volume(-1, static_cast<int>(general_volume * sfx_volume * MIX_MAX_VOLUME));
}

float Audio::GetSfxVolume()
{
	return sfx_volume;
}

void Audio::ChangeMusicVolume(float volume)
{
	music_volume = std::max(0.0f, std::min(volume, 1.0f));

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	pugi::xml_node audioNode = loadFile.child("config").child("audio");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	audioNode.child("music_volume").attribute("value").set_value(music_volume);

	Mix_VolumeMusic(static_cast<int>(general_volume * music_volume * MIX_MAX_VOLUME));
}

float Audio::GetMusicVolume()
{
	return music_volume;
}

void Audio::SetMasterVolume(float general_volume) {
	// Clamp the volume to [0.0f, 1.0f]
	general_volume = std::max(0.0f, std::min(general_volume, 1.0f));

	pugi::xml_document loadFile;
	pugi::xml_parse_result result = loadFile.load_file("config.xml");

	pugi::xml_node audioNode = loadFile.child("config").child("audio");

	if (result == NULL)
	{
		LOG("Could not load file. Pugi error: %s", result.description());
		return;
	}

	audioNode.child("general_volume").attribute("value").set_value(general_volume);

	// Set music and sound effects volume
	Mix_VolumeMusic(static_cast<int>(general_volume * MIX_MAX_VOLUME));
	Mix_Volume(-1, static_cast<int>(general_volume * MIX_MAX_VOLUME));
}
