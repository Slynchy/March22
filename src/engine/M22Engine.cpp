#include "M22Engine.h"

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::MAIN_MENU;
M22Engine::OPTIONS_STRUCTURE  M22Engine::OPTIONS;
SDL_Window* M22Engine::SDL_SCREEN = NULL;
SDL_Renderer *M22Engine::SDL_RENDERER = NULL;
SDL_Event M22Engine::SDL_EVENTS;
const Uint8 *M22Engine::SDL_KEYBOARDSTATE = NULL;
std::vector<std::string> M22Engine::CHARACTER_NAMES;
unsigned short int M22Engine::ACTIVE_BACKGROUND_INDEX = 0;
std::vector<std::string> M22Engine::CHARACTER_EMOTIONS;
std::vector<M22Engine::Character> M22Engine::CHARACTERS_ARRAY;
std::vector<M22Engine::Background> M22Engine::ACTIVE_BACKGROUNDS;
Vec2 M22Engine::MousePos;
bool M22Engine::LMB_Pressed;
bool M22Engine::QUIT = false;
bool M22Engine::skipping = false;
Uint32 M22Engine::last = 0, M22Engine::DELTA_TIME = 0;
Uint32 M22Engine::TIMER_CURR = 0, M22Engine::TIMER_TARGET = 0;
SDL_DisplayMode M22Engine::SDL_DISPLAYMODE = {NULL, NULL, NULL, NULL, NULL};;

Vec2 M22Engine::ScrSize(640,480);

void M22Engine::StartGame(void)
{
	M22Engine::GAMESTATE = M22Engine::GAMESTATES::INGAME;
	float fade_to_black_alpha = 0;
	bool fadeout = false;
	while(fade_to_black_alpha < 250.0f && Mix_PlayingMusic())
	{
		fade_to_black_alpha = M22Graphics::Lerp( fade_to_black_alpha, 255.0f, DEFAULT_LERP_SPEED / 128);
		SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, Uint8(fade_to_black_alpha) );
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BLACK_TEXTURE, NULL, NULL);
		SDL_RenderPresent(M22Engine::SDL_RENDERER);
		if(fadeout == false)
		{
			Mix_FadeOutMusic(2000);	
			fadeout = true;
		};
		//if(RENDERING_API == "direct3d") 
		SDL_Delay(1000/60);
	};
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
	SDL_SetTextureAlphaMod( M22Graphics::activeMenuBackground.sprite, 0 );
	SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, 0 );
	M22Interface::activeInterfaces.clear();
	for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
	{
		if(M22Graphics::backgroundIndex[i] == "graphics/backgrounds/BLACK.webp")
		{
			M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
			M22Engine::ACTIVE_BACKGROUNDS[1].sprite = M22Graphics::BACKGROUNDS[i];

			SDL_SetRenderTarget(M22Engine::SDL_RENDERER, M22Graphics::BACKGROUND_RENDER_TARGET);
			SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BACKGROUNDS[i], NULL, NULL);
			SDL_SetRenderTarget(M22Engine::SDL_RENDERER, NULL);

			SDL_SetRenderTarget(M22Engine::SDL_RENDERER, M22Graphics::NEXT_BACKGROUND_RENDER_TARGET);
			SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BACKGROUNDS[i], NULL, NULL);
			SDL_SetRenderTarget(M22Engine::SDL_RENDERER, NULL);

			SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BACKGROUND_RENDER_TARGET, NULL, NULL);
			break;
		};
	};
	M22Engine::LMB_Pressed = false;
	M22Script::ChangeLine(0);
	M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[0]);
	return;
};

void M22Engine::Shutdown()
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
	return;
};

void M22Engine::OptionsFileInitializer(void)
{
	std::fstream input("OPTIONS.SAV");
	if(!input)
	{
		printf("[M22Engine] OPTIONS.SAV doesn't exist; creating...\n");
		M22Engine::SaveOptions();
	}
	else
	{
		input.close();
		printf("[M22Engine] Loading OPTIONS.SAV\n");
		M22Engine::LoadOptions();
		M22Engine::UpdateOptions();
	};
	return;
};

short int M22Engine::InitializeSDL(const std::string _windowTitle, const std::string _version, Vec2 ScrPos)
{
	srand((unsigned int)time(NULL));
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_SetHint (SDL_HINT_RENDER_DRIVER, RENDERING_API);
	std::string tempTitle = _windowTitle;
	tempTitle += _version;

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

	// Display data
	for(int i = 0; i < SDL_GetNumVideoDisplays(); i++)
	{
		int x = SDL_GetDesktopDisplayMode(i, &M22Engine::SDL_DISPLAYMODE);
		if(x == 0)
		{
			break;
		};
	};

	return 0;
};

void M22Engine::UpdateEvents(void)
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
					M22Engine::skipping = true;
				}
				else if(M22Engine::SDL_EVENTS.key.repeat == 0 && M22Graphics::changeQueued == M22Graphics::BACKGROUND_UPDATE_TYPES::NONE)
				{
					M22Engine::UpdateKeyboard();
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

void M22Engine::UpdateKeyboard()
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

void M22Engine::UpdateDeltaTime(void)
{
	Uint32 now = SDL_GetTicks();  
	M22Engine::DELTA_TIME = now - M22Engine::last; 
	M22Engine::last = now; 
	return;
};

short int M22Engine::InitializeM22(int ScrW, int ScrH)
{
	printf("[M22Engine] Initializing M22...\n");

	printf("[M22Engine] Loading \"scripts/CHARACTER_NAMES.txt\"...\n");
	std::fstream input("scripts/CHARACTER_NAMES.txt");
	int length;
	std::string temp;
	if(input)
	{
		input >> length;
		getline(input,temp);
		M22Engine::CHARACTER_NAMES.clear();
		for(int i = 0; i < length; i++)
		{
			getline(input,temp);
			M22Engine::CHARACTER_NAMES.push_back(temp);
		};
	}
	else
	{
		printf("[M22Engine] Failed to load: \"scripts/CHARACTER_NAMES.txt\" \n ");
		return -1;
	};
	input.close();
	
	printf("[M22Engine] Loading \"graphics/characters/CHARACTERS.txt\"...\n");
	input.open("graphics/characters/CHARACTERS.txt");
	if(input)
	{
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
		M22Engine::CHARACTERS_ARRAY.clear();
		for(int i = 0; i < length; i++)
		{
			getline(input,temp);
			M22Engine::Character tempChar;
			tempChar.name = temp;
			M22Engine::CHARACTERS_ARRAY.push_back(tempChar);
		};
	}
	else
	{
		printf("[M22Engine] Failed to load: \"graphics/characters/CHARACTERS.txt\" \n ");
		return -1;
	};
	input.close();
	
	printf("[M22Engine] Loading character text frames...\n");
	for(int i = 0; i < length; i++)
	{
		temp.clear();
		temp = "graphics/text_frames/";
		temp += M22Engine::CHARACTERS_ARRAY[i].name;
		temp += ".png";
		M22Graphics::characterFrameHeaders.push_back(IMG_LoadTexture(M22Engine::SDL_RENDERER, temp.c_str()));
		temp.clear();
	};
	
	printf("[M22Engine] Loading EMOTIONS.txt...\n");
	for(size_t i = 0; i < CHARACTERS_ARRAY.size(); i++)
	{
		std::string filename = "graphics/characters/";
		filename += CHARACTERS_ARRAY[i].name;
		filename += "/EMOTIONS.txt";
		input.open(filename);
		if(input)
		{
			input >> length;
			getline(input,temp);
			M22Engine::CHARACTERS_ARRAY[i].emotions.resize(length);
			for(int k = 0; k < length; k++)
			{
				getline(input,temp);
				M22Engine::CHARACTERS_ARRAY[i].emotions[k] = temp;
			};
		}
		else
		{
			std::cout << "Failed to load script file: " << "graphics/characters/EMOTIONS.txt" << std::endl;
			return -1;
		};
		input.close();
	};
	
	printf("[M22Engine] Loading OUTFITS.txt...\n");
	for(size_t i = 0; i < CHARACTERS_ARRAY.size(); i++)
	{
		std::string filename = "graphics/characters/";
		filename += M22Engine::CHARACTERS_ARRAY[i].name;
		filename += "/OUTFITS.txt";
		input.open(filename);
		if(input)
		{
			input >> length;
			getline(input,temp);
			M22Engine::CHARACTERS_ARRAY[i].outfits.clear();
			for(int k = 0; k < length; k++)
			{
				getline(input,temp);
				M22Engine::CHARACTERS_ARRAY[i].outfits.push_back(temp);
			};
		}
		else
		{
			std::cout << "Failed to load script file: " << filename << std::endl;
			return -1;
		};
		input.close();
	};
	
	printf("[M22Engine] Loading character sprites...\n");
	for(size_t i = 0; i < CHARACTERS_ARRAY.size(); i++)
	{
		CHARACTERS_ARRAY[i].sprites.resize(CHARACTERS_ARRAY[i].outfits.size());
		for(size_t p = 0; p < CHARACTERS_ARRAY[i].outfits.size(); p++)
		{
			for(size_t k = 0; k < CHARACTERS_ARRAY[i].emotions.size(); k++)
			{
				std::string tempPath = "graphics/characters/";
				tempPath += CHARACTERS_ARRAY[i].name;
				tempPath += "/";
				tempPath += CHARACTERS_ARRAY[i].outfits[p];
				tempPath += "/";
				tempPath += CHARACTERS_ARRAY[i].emotions[k];
				tempPath += ".png";
				SDL_Texture* tempTex = IMG_LoadTexture(M22Engine::SDL_RENDERER, tempPath.c_str());
				SDL_SetTextureAlphaMod( tempTex, 0 );
				SDL_SetTextureBlendMode(tempTex, SDL_BLENDMODE_BLEND);
				M22Engine::CHARACTERS_ARRAY[i].sprites[p].push_back(tempTex);
			};
		};
	};
	
	printf("[M22Engine] Loading main menu backgrounds...\n");
	input.open("graphics/mainmenu/BACKGROUNDS.txt");
	if(input)
	{
		input >> length;
		getline(input,temp);
		M22Graphics::mainMenuBackgrounds.clear();
		for(int i = 0; i < length; i++)
		{
			std::string tempStr = "graphics/mainmenu/";
			tempStr += std::to_string(i);
			tempStr += ".webp";
			SDL_Texture* tempBackground = IMG_LoadTexture(M22Engine::SDL_RENDERER, tempStr.c_str());
			SDL_SetTextureAlphaMod( tempBackground, 0 );
			SDL_SetTextureBlendMode(tempBackground, SDL_BLENDMODE_BLEND);
			M22Graphics::mainMenuBackgrounds.push_back(tempBackground);
		};
	}
	else
	{
		std::cout << "Failed to load script file: " << "graphics/mainmenu/BACKGROUNDS.txt" << std::endl;
		return -1;
	};
	input.close();
	
	M22Graphics::activeMenuBackground.sprite = M22Graphics::mainMenuBackgrounds[rand()%M22Graphics::mainMenuBackgrounds.size()];
	M22Graphics::menuLogo.sprite = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/mainmenu/LOGO.webp");
	SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, 0 );
	SDL_SetTextureBlendMode(M22Graphics::menuLogo.sprite, SDL_BLENDMODE_BLEND);
	if(!M22Graphics::menuLogo.sprite) printf("Failed to load LOGO.png\n");

	M22Script::fontSize = (((float(ScrW) / 640.0f) + (float(ScrH) / 480.0f)) / 2.0f);

	M22Engine::ACTIVE_BACKGROUNDS.resize(2);
	M22Graphics::arrow.sprite = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/arrow.png");
	M22Graphics::OPTION_BAR = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/optionsmenu/bar.png");
	return 0;
};

int M22Engine::GetCharacterIndexFromName(std::string _input, bool _dialogue)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	int size = _input.size();
	_input.erase(std::remove_if(_input.begin(), _input.end(), M22Script::isColon));
	if(size == _input.size() && _dialogue == true)
	{
		// colon was not found, so must be narrative, not dialogue
		return 0;
	};
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.size(); i++)
	{
		if(M22Engine::CHARACTERS_ARRAY[i].name == _input)
		{
			return i;
		};
	};
	return 0;
};

int M22Engine::GetOutfitIndexFromName(std::string _input, int _charIndex)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY[_charIndex].outfits.size(); i++)
	{
		if(M22Engine::CHARACTERS_ARRAY[_charIndex].outfits[i] == _input)
		{
			return i;
		};
	};
	return -1;
};

int M22Engine::GetEmotionIndexFromName(std::string _input, int _charIndex)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY[_charIndex].emotions.size(); i++)
	{
		if(M22Engine::CHARACTERS_ARRAY[_charIndex].emotions[i] == _input)
		{
			return i;
		};
	};
	return -1;
};

void M22Engine::ResetGame(void)
{
	M22Interface::activeInterfaces.clear();
	std::string tempPath = "sfx/music/MENU.OGG";
	M22Sound::ChangeMusicTrack(tempPath);
	M22Interface::ResetStoredInterfaces();
	M22Interface::menuOpen = false;
	M22Engine::skipping = false;
	if(M22Interface::skipButtonState) *M22Interface::skipButtonState = M22Interface::BUTTON_STATES::RESTING;
	if(M22Interface::menuButtonState) *M22Interface::menuButtonState = M22Interface::BUTTON_STATES::RESTING;
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
	M22Graphics::menuLogo.alpha = 0;
	M22Graphics::activeMenuBackground.alpha = 0;
	SDL_SetTextureAlphaMod( M22Graphics::activeMenuBackground.sprite, Uint8(M22Graphics::menuLogo.alpha) );
	SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, Uint8(M22Graphics::activeMenuBackground.alpha) );
	M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC].alpha = 0;
	M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC]);
	M22Engine::GAMESTATE = M22Engine::GAMESTATES::MAIN_MENU;
	return;
};

void M22Engine::LoadOptions(void)
{
	std::ifstream input("OPTIONS.SAV", std::ios::binary | std::ios::in);
	if(input)
	{
		input.read( (char*) (&M22Engine::OPTIONS), sizeof( M22Engine::OPTIONS_STRUCTURE ) );
		input.close();
	};
	return;
};

void M22Engine::UpdateOptions(void)
{
	switch(M22Engine::OPTIONS.WINDOWED)
	{
		case M22Engine::WINDOW_STATES::WINDOWED:
			SDL_SetWindowFullscreen(M22Engine::SDL_SCREEN, 0);
			break;
		case M22Engine::WINDOW_STATES::FULLSCREEN:
			SDL_SetWindowFullscreen(M22Engine::SDL_SCREEN, SDL_WINDOW_FULLSCREEN);
			break;
		case M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS:
			SDL_SetWindowFullscreen(M22Engine::SDL_SCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP );
			break;
		default:
			break;
	};
	Mix_VolumeMusic(int(MIX_MAX_VOLUME* *M22Sound::MUSIC_VOLUME));
	Mix_Volume(M22Sound::MIXERS::SFX,int(MIX_MAX_VOLUME* *M22Sound::SFX_VOLUME));
	Mix_Volume(M22Sound::MIXERS::LOOPED_SFX,int(MIX_MAX_VOLUME* *M22Sound::SFX_VOLUME));
	return;
};

void M22Engine::SaveOptions(void)
{
	std::ofstream output("OPTIONS.SAV", std::ios::binary | std::ios::out);
	if(output)
	{
		output.write( reinterpret_cast <const char*> (&M22Engine::OPTIONS), sizeof(M22Engine::OPTIONS_STRUCTURE) );
		output.close();
	};
	return;
};