#include "../M22Engine.h"

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float M22Sound::MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
float M22Sound::SFX_VOLUME = (float)DEFAULT_SFX_VOLUME_MULT;
int M22Sound::currentTrack = 0;
std::vector<std::string> M22Sound::MUSIC_NAMES;
std::vector<std::string> M22Sound::SFX_NAMES;

short int M22Sound::PlaySting(short int _position)
{
	/*
		0 = success
		-1 = sound doesn't exist
		-2 = cannot play, already playing
	*/
	if(M22Sound::SOUND_FX[_position])
	{
		if(!Mix_Playing(M22Sound::MIXERS::SFX))
		{
			Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[_position], 0);
			return 0;
		}
		else
		{
			return -2;
		};
	}
	else
	{
		return -1;
	};
};

short int M22Sound::PlaySting(short int _position, bool _forceplayback)
{
	if(M22Sound::SOUND_FX[_position])
	{
		if(!Mix_Playing(M22Sound::MIXERS::SFX) || _forceplayback == true)
		{
			Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[_position], 0);
			return 0;
		}
		else
		{
			return -2;
		};
	}
	else
	{
		return -1;
	};
};

short int M22Sound::PlaySting(std::string _name, bool _forceplayback)
{
	for(size_t i = 0; i < M22Sound::SOUND_FX.size(); i++)
	{
		if(_name == M22Sound::SFX_NAMES[i])
		{
			if(!Mix_Playing(M22Sound::MIXERS::SFX) || _forceplayback == true)
			{
				Mix_PlayChannel( M22Sound::MIXERS::SFX, M22Sound::SOUND_FX[i], 0);
				return 0;
			};
			return -2;
		};
	};
	return -1;
};

short int M22Sound::ChangeMusicTrack(std::string _name)
{
	for(size_t i = 0; i < M22Sound::MUSIC.size(); i++)
	{
		if(_name == M22Sound::MUSIC_NAMES[i])
		{
			Mix_PlayMusic( M22Sound::MUSIC[i], M22Sound::MIXERS::BGM );
			return 0;
		};
	};
	printf("Failed to find music file: %s", _name.c_str());
	return -1;
};

short int M22Sound::ChangeMusicTrack(short int _position)
{
	if(M22Sound::MUSIC[_position])
	{
		Mix_PlayMusic( M22Sound::MUSIC[_position], M22Sound::MIXERS::BGM );
		return 0;
	}
	else
	{
		return -1;
	};
};

void M22Sound::StopMusic()
{
	Mix_HaltMusic();
	return;
};

void M22Sound::PauseMusic()
{
	Mix_PauseMusic();
	return;
};

void M22Sound::ResumeMusic()
{
	Mix_ResumeMusic();
	return;
};