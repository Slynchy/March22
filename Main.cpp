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
#include <SDL_mixer.h> // SDL Sound library
#include <SDL_ttf.h> // SDL TTF library
#include "M22Engine.h"
#include <iostream>
#include <chrono>

#define DEBUG_ENABLED false

#define WINDOW_TITLE		"March22 Engine Prototype "
#define VERSION				"v0.4.4"

#define FPS 60

Vec2 ScrPos(600,200);

void Shutdown();
short int InitializeSDL();
short int InitializeSound();
short int InitializeSFX();
short int InitializeMusic();
void UpdateKeyboard();
void UpdateEvents();
void UpdateSound();
void DrawBackground(short int);
void InitTextBox();

int main(int argc, char* argv[]) 
{
	srand((unsigned int)time(NULL));

	std::fstream input("OPTIONS.SAV");
	if(!input)
	{
		printf("OPTIONS.SAV doesn't exist; creating...\n");
		M22Engine::SaveOptions();
	}
	else
	{
		input.close();
		printf("Loading OPTIONS.SAV\n");
		M22Engine::LoadOptions();
		M22Engine::UpdateOptions();
	};

	InitializeSDL();
	InitializeSound();
	M22Engine::InitializeM22(int(M22Engine::ScrSize.x()),int(M22Engine::ScrSize.y()));
	M22Graphics::LoadBackgroundsFromIndex("graphics/backgrounds/index.txt");
	InitTextBox();
	M22Script::LoadScriptToCurrent("EVC_001_strings.txt");
	M22Graphics::textFont = TTF_OpenFont( "graphics/FONT.ttf", 19);//int(19.0f * M22Script::fontSize) );

	M22Graphics::wipeBlack = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/wipeblack.png");
	SDL_QueryTexture(M22Graphics::wipeBlack, NULL, NULL, &M22Graphics::wipeBlackRect.w, &M22Graphics::wipeBlackRect.h);
	M22Graphics::wipeBlackRect.w /= 3;

	M22Interface::storedInterfaces.resize(M22Interface::INTERFACES::NUM_OF_INTERFACES);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::INGAME_INTRFC], 3, 0, "graphics/interface/GAME_BUTTONS.txt", true, M22Interface::INTERFACES::INGAME_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MENU_BUTTON_INTRFC], 4, 0, "graphics/interface/MENU_BUTTONS.txt", true, M22Interface::INTERFACES::MENU_BUTTON_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC], 3, 0, "graphics/mainmenu/BUTTONS.txt", false, M22Interface::INTERFACES::MAIN_MENU_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::OPTIONS_MENU_INTRFC], 7, 0, "graphics/optionsmenu/BUTTONS.txt", true, M22Interface::INTERFACES::OPTIONS_MENU_INTRFC);

	SDL_SetRenderDrawColor(M22Engine::SDL_RENDERER, 255, 255, 255, 255);

	M22Graphics::wipePosition = new SDL_Rect();
	M22Graphics::wipePosition->x = (0 - 640);
	M22Graphics::wipePosition->y = 0;
	M22Graphics::wipePosition->w = 640;
	M22Graphics::wipePosition->h = 480;

	if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
	{
		M22Engine::StartGame();
	}
	else if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU)
	{
		std::string tempPath = "sfx/music/MENU.OGG";
		M22Sound::ChangeMusicTrack(tempPath);
		M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC]);
	};

	while( !M22Engine::QUIT )
	{
		Uint32 now = SDL_GetTicks();  
		M22Engine::DELTA_TIME = now - M22Engine::last; 
		M22Engine::last = now; 

		UpdateEvents();
		UpdateSound();
		if(M22Engine::skipping) M22Script::ChangeLine(++M22Script::currentLineIndex);
		if(M22Engine::TIMER_TARGET != 0)
		{
			if(M22Engine::TIMER_CURR < M22Engine::TIMER_TARGET)
			{
				M22Engine::TIMER_CURR += M22Engine::DELTA_TIME;
			}
			else
			{
				M22Engine::TIMER_CURR = 0;
				M22Engine::TIMER_TARGET = 0;
				M22Script::ChangeLine(++M22Script::currentLineIndex);
			};
		};
		if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU) M22Graphics::UpdateBackgrounds();
		M22Graphics::UpdateCharacters();
		M22Interface::UpdateActiveInterfaces( int(M22Engine::ScrSize.x()), int(M22Engine::ScrSize.y()) );
		
		SDL_RenderClear(M22Engine::SDL_RENDERER);

		switch(M22Engine::GAMESTATE)
		{
			case M22Engine::GAMESTATES::MAIN_MENU:
				SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::activeMenuBackground.sprite, NULL, NULL);
				SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::menuLogo.sprite			, NULL, NULL);
				M22Interface::DrawActiveInterfaces();
				break;
			case M22Engine::GAMESTATES::INGAME:
				M22Graphics::DrawInGame();
				break;
			default:
				break;
		};
		//SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::testmask, NULL, NULL);

		M22Engine::LMB_Pressed = false;
		if(!M22Engine::QUIT) SDL_RenderPresent(M22Engine::SDL_RENDERER);
		//if(RENDERING_API == "direct3d" || RENDERING_API == "opengl") 
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
	M22Graphics::textFont = NULL;
	M22Engine::CHARACTERS_ARRAY.clear();
	TTF_Quit();
	if(DEBUG_ENABLED)
	{
		printf("\nPress enter to exit...");
		getchar();
	};
};

short int InitializeSDL()
{
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_SetHint (SDL_HINT_RENDER_DRIVER, RENDERING_API);
	std::string tempTitle = WINDOW_TITLE;
	tempTitle += VERSION;

	if(M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::FULLSCREEN)
	{
		M22Engine::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	}
	else if(M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS)
	{
		M22Engine::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	}
	else
	{
		M22Engine::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	};

    M22Engine::SDL_RENDERER = SDL_CreateRenderer(M22Engine::SDL_SCREEN, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	SDL_RenderSetLogicalSize(M22Engine::SDL_RENDERER, (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y());
	if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 4096 ) < 0 )
	{
		printf( "SDL_mixer failed to init! Error: %s\n", Mix_GetError() );
	};

	Mix_VolumeMusic(int(MIX_MAX_VOLUME* *M22Sound::MUSIC_VOLUME));
	Mix_Volume(M22Sound::MIXERS::SFX,int(MIX_MAX_VOLUME* *M22Sound::SFX_VOLUME));
	Mix_Volume(M22Sound::MIXERS::LOOPED_SFX,int(MIX_MAX_VOLUME* *M22Sound::SFX_VOLUME));

	if( TTF_Init() < 0 )
    {
		printf( "SDL_TTF failed to init! Error: %s\n", TTF_GetError() );
    }

	// linear filter; works on all platforms
	SDL_SetHint (SDL_HINT_RENDER_SCALE_QUALITY, BILINEAR_FILTERING);

	return 0;
};

short int InitializeMusic()
{
	std::fstream input("sfx/music/index.txt");
	int length;
	if(input)
	{
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
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
			M22Sound::MUSIC_NAMES.push_back(currentfile);
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
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
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
			M22Sound::SFX_NAMES.push_back(currentfile);
			//Mix_VolumeChunk(M22Sound::SOUND_FX[i], int(MIX_MAX_VOLUME*M22Sound::SFX_VOLUME));
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
	if( !Mix_PlayingMusic() )
	{
		M22Sound::ChangeMusicTrack(M22Sound::currentTrack);
	};
	return;
};


void UpdateKeyboard()
{
	M22Engine::SDL_KEYBOARDSTATE = SDL_GetKeyboardState(NULL);
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_ESCAPE])
	{
		M22Engine::QUIT=true;
	};
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_SPACE] && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
	{
		M22Interface::DRAW_TEXT_AREA = !M22Interface::DRAW_TEXT_AREA;
	};
	if(M22Engine::TIMER_TARGET != 0) return;
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_RETURN] && M22Interface::DRAW_TEXT_AREA == true && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
	{
		M22Script::ChangeLine(++M22Script::currentLineIndex);
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
				M22Engine::QUIT = true;
				break;
			case SDL_MOUSEMOTION:
				M22Engine::MousePos.x(M22Engine::SDL_EVENTS.motion.x);
				M22Engine::MousePos.y(M22Engine::SDL_EVENTS.motion.y);
				break;
			case SDL_MOUSEBUTTONDOWN:
				if(M22Engine::SDL_EVENTS.button.button == SDL_BUTTON_LEFT)
				{
					M22Engine::LMB_Pressed = true;
				};
				break;
			case SDL_MOUSEBUTTONUP:
				if(M22Engine::SDL_EVENTS.button.button == SDL_BUTTON_LEFT)
				{
					M22Engine::LMB_Pressed = false;
				};
				break;
			case SDL_KEYDOWN:
				if(M22Engine::SDL_EVENTS.key.keysym.scancode == SDL_SCANCODE_RSHIFT && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME && M22Graphics::changeQueued == M22Graphics::BACKGROUND_UPDATE_TYPES::NONE)
				{
					//M22Script::ChangeLine(++M22Script::currentLineIndex);
					M22Engine::skipping = true;
				}
				else if(M22Engine::SDL_EVENTS.key.repeat == 0 && M22Graphics::changeQueued == M22Graphics::BACKGROUND_UPDATE_TYPES::NONE)
				{
					UpdateKeyboard();
				};
				break;
			case SDL_KEYUP:
				if(M22Engine::SDL_EVENTS.key.keysym.scancode == SDL_SCANCODE_RSHIFT)
				{
					M22Engine::skipping = false;
				};
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

void InitTextBox()
{
	// load texture
	M22Graphics::textFrame = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/frame.png");

	// init the render target texture
	M22Interface::ChatBoxRenderer = SDL_CreateTexture( M22Engine::SDL_RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET , 640, 480 );

	// allow alpha channels
	SDL_SetTextureBlendMode(M22Graphics::textFrame, SDL_BLENDMODE_BLEND);
	SDL_SetTextureBlendMode(M22Interface::ChatBoxRenderer, SDL_BLENDMODE_BLEND);

	return;
};