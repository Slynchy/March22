#ifndef M22ENGINE_H
#define M22ENGINE_H

/*
	RENDERER == SDL_RENDERER
	Shorthand
*/
#define RENDERER SDL_RENDERER

#define DEFAULT_MUSIC_VOLUME_MULT 0.25
#define DEFAULT_SFX_VOLUME_MULT 0.05

#include "SDL.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>

class M22Engine
{
	private:
	public:
		enum GAMESTATES
		{
			MAIN_MENU,
			INGAME
		};

		enum CHARACTERS
		{
			YUUJI
		};

		static M22Engine::GAMESTATES GAMESTATE;
		static SDL_Window* SDL_SCREEN;
		static SDL_Renderer *SDL_RENDERER;
		static SDL_Event SDL_EVENTS;
		static const Uint8 *SDL_KEYBOARDSTATE;
};

class M22Graphics
{
	private:
	public:
		static SDL_Texture* textFrame;
		static std::vector<SDL_Texture*> characterFrameHeaders;

		static std::vector<SDL_Texture*> BACKGROUNDS;
		static short int LoadBackgroundsFromIndex(const char* _filename);
};

class M22Sound
{
	private:
	public:
		enum MIXERS
		{
			BGM,
			SFX,
			VOICE
		};
		static std::vector<Mix_Chunk*> SOUND_FX;
		static std::vector<Mix_Music*> MUSIC;
		static float MUSIC_VOLUME;
		static float SFX_VOLUME;

		static short int PlaySting(short int);
		static short int PlaySting(short int, bool);

		static short int ChangeMusicTrack(short int _position);
		static void StopMusic();
		static void PauseMusic();
		static void ResumeMusic();
		static short int StartMusic(short int _position);
};

class M22Script
{
	private:
	public:
		static std::string currentLine;
		static int currentLineIndex;
		static std::vector<std::string> currentScript;
		static short int activeSpeakerIndex;

		static short int LoadScriptToCurrent(const char* _filename);
};

#endif