/*
	March22 Engine Prototype:
		A C++ port of Snow Sakura

	Code by Sam Lynch, 
		Amduat Games

	Everything else is property of 
		D.O. and G-Collections
*/

#include "SDL.h" // Main SDL library
#include <SDL_image.h> // SDL Image library
#include <SDL_mixer.h> // SDL Image library
#include "M22Engine.h"
#include <iostream>
#include <chrono>

#define DEBUG_ENABLED false

#define WINDOW_TITLE "March22 Engine Prototype"

#define FPS 60
#define screenW 640
#define screenH 480
#define screenX 600
#define screenY 200

bool QUIT = false;

void Shutdown();
short int InitializeSDL();
short int InitializeSound();
short int InitializeSFX();
short int InitializeMusic();
void UpdateKeyboard();
void UpdateEvents();
void UpdateSound();
void DrawBackground(short int);

int main(int argc, char* argv[]) 
{
	InitializeSDL();
	InitializeSound();
	M22Graphics::LoadBackgroundsFromIndex("graphics/backgrounds/index.txt");
	M22Graphics::textFrame = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/frame.png");
	M22Graphics::characterFrameHeaders.push_back(IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/text_frames/yuuji.png"));
	M22Script::LoadScriptToCurrent("scripts/EVC_001_strings.txt");

	while( !QUIT )
	{
		UpdateEvents();
		UpdateKeyboard();
		UpdateSound();

		switch(M22Engine::GAMESTATE)
		{
			case M22Engine::GAMESTATES::MAIN_MENU:
				DrawBackground(0);
				SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::textFrame, NULL, NULL);
				SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::characterFrameHeaders[M22Script::activeSpeakerIndex], NULL, NULL);
				break;
			case M22Engine::GAMESTATES::INGAME:
				break;
			default:
				break;
		};

		SDL_RenderPresent(M22Engine::SDL_RENDERER);
		SDL_Delay(1000/FPS);
	};

	Shutdown();
    return 0;
}

void Shutdown()
{
	SDL_Quit();
	M22Engine::SDL_RENDERER = NULL;
	M22Engine::SDL_SCREEN = NULL;
	M22Engine::SDL_KEYBOARDSTATE = NULL;
	M22Sound::SOUND_FX.clear();
	M22Sound::MUSIC.clear();
	M22Graphics::BACKGROUNDS.clear();
	M22Graphics::characterFrameHeaders.clear();
	M22Graphics::textFrame = NULL;
	if(DEBUG_ENABLED)
	{
		printf("\nPress enter to exit...");
		getchar();
	};
};

short int InitializeSDL()
{
	SDL_Init( SDL_INIT_EVERYTHING );
	M22Engine::SDL_SCREEN = SDL_CreateWindow(WINDOW_TITLE, screenX, screenY, screenW, screenH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);
    M22Engine::SDL_RENDERER = SDL_CreateRenderer(M22Engine::SDL_SCREEN, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderSetLogicalSize(M22Engine::SDL_RENDERER, screenW, screenH);
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer failed to init! Error: %s\n", Mix_GetError() );
	};
	Mix_VolumeMusic(int(MIX_MAX_VOLUME*M22Sound::MUSIC_VOLUME));
	return 0;
};

short int InitializeMusic()
{
	std::fstream input("sfx/music/index.txt");
	int length;
	if(input)
	{
		input >> length;
		for(int i = 0; i < length; i++)
		{
			std::string currentfile;
			input >> currentfile;
			Mix_Music *temp = Mix_LoadMUS(currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Sound::MUSIC.push_back(temp);
		};
	}
	else
	{
		std::cout << "Failed to load index file for music!" << std::endl;
		return -1;
	};
	input.close();
	return 0;
};

short int InitializeSFX()
{
	std::fstream input("sfx/stings/index.txt");
	int length;
	if(input)
	{
		input >> length;
		for(int i = 0; i < length; i++)
		{
			std::string currentfile;
			input >> currentfile;
			Mix_Chunk *temp = Mix_LoadWAV(currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Sound::SOUND_FX.push_back(temp);
			Mix_VolumeChunk(M22Sound::SOUND_FX[i], int(MIX_MAX_VOLUME*M22Sound::SFX_VOLUME));
		};
	}
	else
	{
		std::cout << "Failed to load index file for music!" << std::endl;
		return -1;
	};
	input.close();
	return 0;
};

short int InitializeSound()
{
	if(InitializeMusic() != 0)
	{
		printf("Failed to initialize music!");
		return -1;		
	};
	if(InitializeSFX() != 0)
	{
		printf("Failed to initialize SFX!");
		return -1;		
	};
	return 0;
};

void UpdateSound()
{
	if(!Mix_PlayingMusic() && DEBUG_ENABLED == false)
	{
		M22Sound::ChangeMusicTrack(0);
	};
	return;
};


void UpdateKeyboard()
{
	M22Engine::SDL_KEYBOARDSTATE = SDL_GetKeyboardState(NULL);
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_ESCAPE])
	{
		QUIT=true;
	};
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_W] && DEBUG_ENABLED == true)
	{
		M22Sound::PlaySting(0);
	};
	return;
};

void UpdateEvents()
{
	while( SDL_PollEvent( &M22Engine::SDL_EVENTS ) )
	{
		switch( M22Engine::SDL_EVENTS.type )
		{
			case SDL_QUIT:
				QUIT = true;
				break;
			default:
				break;
		}
	};
	return;
};

void DrawBackground(short int _index)
{
	if(M22Graphics::BACKGROUNDS[_index])
	{
		// Backgrounds match the 640x480 resolution, so no need for scaling... for now.
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BACKGROUNDS[_index], NULL, NULL);
	};
	return;
};