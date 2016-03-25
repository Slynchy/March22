#include "M22Engine.h"

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::MAIN_MENU;
SDL_Window* M22Engine::SDL_SCREEN = NULL;
SDL_Renderer *M22Engine::SDL_RENDERER = NULL;
SDL_Event M22Engine::SDL_EVENTS;
const Uint8 *M22Engine::SDL_KEYBOARDSTATE = NULL;

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float M22Sound::MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
float M22Sound::SFX_VOLUME = (float)DEFAULT_SFX_VOLUME_MULT;

std::vector<SDL_Texture*> M22Graphics::BACKGROUNDS;
SDL_Texture* M22Graphics::textFrame;
std::vector<SDL_Texture*> M22Graphics::characterFrameHeaders;

std::string M22Script::currentLine;
int M22Script::currentLineIndex;
std::vector<std::string> M22Script::currentScript;
short int M22Script::activeSpeakerIndex;

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

short int M22Sound::ChangeMusicTrack(short int _position)
{
	if(M22Sound::MUSIC[_position])
	{
		Mix_PlayMusic( M22Sound::MUSIC[_position], -1 );
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

short int M22Graphics::LoadBackgroundsFromIndex(const char* _filename)
{
	std::fstream input(_filename);
	int length;
	if(input)
	{
		input >> length;
		for(int i = 0; i < length; i++)
		{
			std::string currentfile;
			input >> currentfile;
			SDL_Texture *temp = IMG_LoadTexture(M22Engine::SDL_RENDERER, currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Graphics::BACKGROUNDS.push_back(temp);
		};
	}
	else
	{
		std::cout << "Failed to load index file: " << _filename << std::endl;
		return -1;
	};
	input.close();
	return 0;
};

short int M22Script::LoadScriptToCurrent(const char* _filename)
{
	std::fstream input(_filename);
	int length;
	std::string temp;
	if(input)
	{
		input >> length;
		M22Script::currentScript.clear();
		for(int i = 0; i < length; i++)
		{
			getline(input,temp);
			M22Script::currentScript.push_back(temp);
		};
	}
	else
	{
		std::cout << "Failed to load script file: " << _filename << std::endl;
		return -1;
	};
	input.close();
	return 0;
};