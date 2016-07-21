#include <engine/M22Engine.h>

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::INGAME;
M22Engine::OPTIONS_STRUCTURE  M22Engine::OPTIONS;
SDL_Event M22Engine::SDL_EVENTS;
const Uint8 *M22Engine::SDL_KEYBOARDSTATE = NULL;
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
		SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Graphics::BLACK_TEXTURE, NULL, NULL);
		SDL_RenderPresent(M22Renderer::SDL_RENDERER);
		if(fadeout == false)
		{
			Mix_FadeOutMusic(2000);	
			fadeout = true;
		};
		//if(RENDERING_API == "direct3d") 
		SDL_Delay(1000/60);
	};
	M22Sound::StopMusic();
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
			M22Engine::ACTIVE_BACKGROUNDS[0].name = "graphics/backgrounds/BLACK.webp";
			M22Engine::ACTIVE_BACKGROUNDS[1].name = "graphics/backgrounds/BLACK.webp";

			SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, M22Graphics::BACKGROUND_RENDER_TARGET);
			SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Graphics::BACKGROUNDS[i], NULL, NULL);
			SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, NULL);

			SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, M22Graphics::NEXT_BACKGROUND_RENDER_TARGET);
			SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Graphics::BACKGROUNDS[i], NULL, NULL);
			SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, NULL);

			SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Graphics::BACKGROUND_RENDER_TARGET, NULL, NULL);
			break;
		};
	};
	M22Engine::LMB_Pressed = false;
	M22Script::ChangeLine(0);
	M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[0]);
	return;
};

int M22Engine::GetBackgroundIDFromName(std::string _name)
{
	for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
	{
		std::string tempPath = "graphics/backgrounds/";
		tempPath += _name;
		tempPath += ".webp";
		if(tempPath == M22Graphics::backgroundIndex.at(i))
		{
			return i;
		};
	};
	return -1;
};

void M22Engine::DestroySDLTextureVector(std::vector<SDL_Texture*>& _vector)
{
	for(size_t i = 0; i < _vector.size(); i++)
	{
		// Shouldn't this be 'nullptr'? idk, since it's a C api?
		if(_vector.at(i) != NULL)
			SDL_DestroyTexture(_vector.at(i));
	};
	_vector.clear();
	return;
};

void M22Engine::FreeSFXVector(std::vector<Mix_Chunk*>& _vector)
{
	for(size_t i = 0; i < _vector.size(); i++)
	{
		// Shouldn't this be 'nullptr'? idk, since it's a C api?
		if(_vector.at(i) != NULL)
			Mix_FreeChunk(_vector.at(i));
	};
	_vector.clear();
	return;
};

void M22Engine::FreeMusicVector(std::vector<Mix_Music*>& _vector)
{
	for(size_t i = 0; i < _vector.size(); i++)
	{
		// Shouldn't this be 'nullptr'? idk, since it's a C api?
		if(_vector.at(i) != NULL)
			Mix_FreeMusic(_vector.at(i));
	};
	_vector.clear();
	return;
};

void M22Engine::Shutdown()
{
	SDL_Quit();

	// M22Renderer
	SDL_DestroyRenderer(M22Renderer::SDL_RENDERER);
	M22Renderer::SDL_RENDERER = NULL;
	SDL_DestroyWindow(M22Renderer::SDL_SCREEN);
	M22Renderer::SDL_SCREEN = NULL;

	// M22Graphics
	M22Engine::DestroySDLTextureVector(M22Graphics::BACKGROUNDS);
	M22Engine::DestroySDLTextureVector(M22Graphics::characterFrameHeaders);
	M22Engine::DestroySDLTextureVector(M22Graphics::mainMenuBackgrounds);
	SDL_DestroyTexture(M22Graphics::textFrame);
		M22Graphics::textFrame = NULL;
	SDL_DestroyTexture(M22Graphics::activeMenuBackground.sprite);
		M22Graphics::activeMenuBackground.sprite = NULL;
	SDL_DestroyTexture(M22Graphics::menuLogo.sprite);
		M22Graphics::menuLogo.sprite = NULL;
	SDL_DestroyTexture(M22Graphics::BLACK_TEXTURE);
		M22Graphics::BLACK_TEXTURE = NULL;
	SDL_DestroyTexture(M22Graphics::OPTION_BAR);
		M22Graphics::OPTION_BAR = NULL;
	SDL_DestroyTexture(M22Graphics::BACKGROUND_RENDER_TARGET);
		M22Graphics::BACKGROUND_RENDER_TARGET = NULL;
	SDL_DestroyTexture(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET);
		M22Graphics::NEXT_BACKGROUND_RENDER_TARGET = NULL;
	SDL_DestroyTexture(M22Graphics::wipeBlack);
		M22Graphics::wipeBlack = NULL;
	TTF_CloseFont(M22Graphics::textFont);
	M22Graphics::textFont = NULL;

	//M22Engine
	M22Engine::FreeSFXVector(M22Sound::SOUND_FX);
	M22Engine::FreeMusicVector(M22Sound::MUSIC);
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.size(); i++)
	{
		M22Engine::CHARACTERS_ARRAY.at(i).emotions.clear();
		M22Engine::CHARACTERS_ARRAY.at(i).outfits.clear();
		for(size_t spr = 0; spr < M22Engine::CHARACTERS_ARRAY.at(i).sprites.size(); spr++)
		{
			M22Engine::DestroySDLTextureVector(M22Engine::CHARACTERS_ARRAY.at(i).sprites.at(spr));
		};
	};
	M22Engine::CHARACTERS_ARRAY.clear();

	M22Lua::Shutdown();
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
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
	SDL_version compiled;
	SDL_VERSION(&compiled);
	printf("[M22Engine] Initializing SDL2 v%d.%d.%d...\n", compiled.major, compiled.minor, compiled.patch);
	srand((unsigned int)time(NULL));
	SDL_Init( SDL_INIT_EVERYTHING );
	SDL_SetHint (SDL_HINT_RENDER_DRIVER, RENDERING_API);
	std::string tempTitle = _windowTitle;
	tempTitle += _version;

	if(M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::FULLSCREEN)
	{
		M22Renderer::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	}
	else if(M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS)
	{
		M22Renderer::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	}
	else
	{
		M22Renderer::SDL_SCREEN = SDL_CreateWindow(tempTitle.c_str(), (int)ScrPos.x(), (int)ScrPos.y(), (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y(), SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	};

    M22Renderer::SDL_RENDERER = SDL_CreateRenderer(M22Renderer::SDL_SCREEN, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	SDL_RenderSetLogicalSize(M22Renderer::SDL_RENDERER, (int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y());
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
				if(M22Engine::SDL_EVENTS.key.keysym.scancode == SDL_SCANCODE_RSHIFT && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME && M22Graphics::changeQueued == M22Graphics::BACKGROUND_UPDATE_TYPES::NONE && M22Script::currentLineType != M22Script::LINETYPE::MAKE_DECISION)
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
	if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_RETURN] && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME && M22Script::currentLineType != M22Script::LINETYPE::MAKE_DECISION)
	{
		M22Interface::DRAW_TEXT_AREA = true;
		M22Script::ChangeLine(++M22Script::currentLineIndex);
	};

	if(M22Script::currentLineType == M22Script::LINETYPE::MAKE_DECISION)
	{
		bool inputmade = false;

		if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_1])
		{
			M22Script::gameDecisions.back().selectedOption = 0;
			inputmade = true;
			M22Sound::PlaySting("sfx/stings/SE001.OGG", true);
		}
		else if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_2])
		{
			M22Script::gameDecisions.back().selectedOption = 1;
			inputmade = true;
			M22Sound::PlaySting("sfx/stings/SE001.OGG", true);
		}
		else if(M22Engine::SDL_KEYBOARDSTATE[SDL_SCANCODE_3])
		{
			if(M22Script::gameDecisions.back().num_of_choices < 3)
			{
				printf("[M22Script] Option 3 selected but less than two available!\n");
				return;
			}
			else
			{
				M22Script::gameDecisions.back().selectedOption = 2;
				inputmade = true;
			};
			M22Sound::PlaySting("sfx/stings/SE001.OGG", true);
		};
		
		if(inputmade == true)
		{
			M22Script::ChangeLine(++M22Script::currentLineIndex);
		};
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

M22Engine::Character M22Engine::CreateCharacter(std::string _name)
{
	M22Engine::Character tempChar;
	tempChar.name = _name;
	return tempChar;
};

short int M22Engine::InitializeM22(int ScrW, int ScrH)
{
	printf("[M22Engine] Initializing M22...\n");

	std::fstream input("scripts/CHARACTER_NAMES.txt");
	int length;
	std::string temp;
	
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
	//
	printf("[M22Engine] Loading character text frames...\n");
	for(int i = 0; i < length; i++)
	{
		temp.clear();
		temp = "graphics/text_frames/";
		temp += M22Engine::CHARACTERS_ARRAY.at(i).name;
		temp += ".png";
		SDL_Texture* tempTexture = IMG_LoadTexture(M22Renderer::SDL_RENDERER, temp.c_str());
		M22Graphics::characterFrameHeaders.push_back(tempTexture);
		temp.clear();
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
			SDL_Texture* tempBackground = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempStr.c_str());
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
	M22Graphics::menuLogo.sprite = IMG_LoadTexture(M22Renderer::SDL_RENDERER, "graphics/mainmenu/LOGO.webp");
	SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, 0 );
	SDL_SetTextureBlendMode(M22Graphics::menuLogo.sprite, SDL_BLENDMODE_BLEND);
	if(!M22Graphics::menuLogo.sprite) printf("Failed to load LOGO.png\n");

	M22Script::fontSize = (((float(ScrW) / 640.0f) + (float(ScrH) / 480.0f)) / 2.0f);

	M22Engine::ACTIVE_BACKGROUNDS.resize(2);
	M22Graphics::arrow.sprite = IMG_LoadTexture(M22Renderer::SDL_RENDERER, "graphics/arrow.png");
	M22Graphics::OPTION_BAR = IMG_LoadTexture(M22Renderer::SDL_RENDERER, "graphics/optionsmenu/bar.png");
	return 0;
};

int M22Engine::GetCharacterIndexFromName(std::wstring _input, bool _dialogue)
{
	std::string temp = M22Script::to_string(_input);
	temp.erase(std::remove_if(temp.begin(), temp.end(), isspace));
	_input = M22Script::to_wstring(temp.c_str());
	int size = _input.size();
	_input.erase(std::remove_if(_input.begin(), _input.end(), M22Script::isColon));
	if(size == _input.size() && _dialogue == true)
	{
		// colon was not found, so must be narrative, not dialogue
		return 0;
	};
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.size(); i++)
	{
		if(M22Script::to_wstring(M22Engine::CHARACTERS_ARRAY[i].name.c_str()) == _input)
		{
			return i;
		};
	};
	return -1;
};

int M22Engine::GetCharacterIndexFromName(std::string _input, bool _dialogue)
{
	if(M22Engine::CHARACTERS_ARRAY.size() == 0) return -1;
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
	return -1;
};

int M22Engine::GetOutfitIndexFromName(std::string _input, int _charIndex)
{
	if(M22Engine::CHARACTERS_ARRAY.size() == 0 || _charIndex == -1) return -1;
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.at(_charIndex).outfits.size(); i++)
	{
		if(M22Engine::CHARACTERS_ARRAY.at(_charIndex).outfits[i] == _input)
		{
			return i;
		};
	};
	return -1;
};

int M22Engine::GetEmotionIndexFromName(std::string _input, int _charIndex)
{
	if(M22Engine::CHARACTERS_ARRAY.size() == 0 || _charIndex == -1) return -1;
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.at(_charIndex).emotions.size(); i++)
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
			SDL_SetWindowFullscreen(M22Renderer::SDL_SCREEN, 0);
			break;
		case M22Engine::WINDOW_STATES::FULLSCREEN:
			SDL_SetWindowFullscreen(M22Renderer::SDL_SCREEN, SDL_WINDOW_FULLSCREEN);
			break;
		case M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS:
			SDL_SetWindowFullscreen(M22Renderer::SDL_SCREEN, SDL_WINDOW_FULLSCREEN_DESKTOP );
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

void M22Engine::SaveGame(const char* _filename)
{
	printf("[M22Engine] Saving game to %s...\n", _filename);
	std::ofstream savegame(_filename, std::ios::binary | std::ios::out);
	if(savegame)
	{

		struct SAVEGAME_STRUCTURE savegameObject;
		savegameObject.GAMESTATE = M22Engine::GAMESTATE;
		savegameObject.CURRENTBACKGROUND = M22Engine::ACTIVE_BACKGROUNDS[0].name;
		savegameObject.CURRENTMUSIC = M22Sound::MUSIC_NAMES.at(M22Sound::currentTrack);
		savegameObject.CURRENTLOOPINGSFX = "sfx/music/SILENCE.OGG";
		savegameObject.CURRENTSCRIPTFILE = M22Script::currentScriptFileName;
		savegameObject.CURRENTLINEPOSITION = M22Script::currentLineIndex;

		// save characters on screen?

		savegame << savegameObject.GAMESTATE << '\n' << savegameObject.CURRENTBACKGROUND << '\n' << savegameObject.CURRENTMUSIC << '\n' << savegameObject.CURRENTLOOPINGSFX << '\n' << savegameObject.CURRENTSCRIPTFILE << '\n' << savegameObject.CURRENTLINEPOSITION << '\n';

		savegame.close();
	};
	return;
};

void M22Engine::LoadGame(const char* _filename)
{
	printf("[M22Engine] Loading game from %s...\n", _filename);
	std::ifstream savegame(_filename, std::ios::binary | std::ios::in);
	if(savegame)
	{
		//savegame.write( reinterpret_cast <const char*> (&M22Engine::OPTIONS), sizeof(M22Engine::OPTIONS_STRUCTURE) );
		struct SAVEGAME_STRUCTURE savegameObject;
		
		int tempint = 0;

		savegame >> tempint;
		savegameObject.GAMESTATE = M22Engine::GAMESTATES(tempint);
		savegame >> savegameObject.CURRENTBACKGROUND;
		savegame >> savegameObject.CURRENTMUSIC;
		savegame >> savegameObject.CURRENTLOOPINGSFX;
		savegame >> savegameObject.CURRENTSCRIPTFILE;
		savegame >> savegameObject.CURRENTLINEPOSITION;

		savegame.close();

		M22Engine::GAMESTATE = savegameObject.GAMESTATE;
		M22Script::currentLineIndex = savegameObject.CURRENTLINEPOSITION;
		M22Sound::ChangeMusicTrack(savegameObject.CURRENTMUSIC);
		M22Sound::StopLoopedStings();
		M22Sound::PlayLoopedSting(savegameObject.CURRENTLOOPINGSFX);

		std::wstring tempStr = M22Script::to_wstring(savegameObject.CURRENTSCRIPTFILE);
		std::vector<std::wstring> tempVec;
		M22Script::SplitString(tempStr, tempVec, '/');
		//M22Script::LoadScriptToCurrent(M22Script::to_string(tempVec.back()).c_str());
		//M22Script::LoadScriptToCurrent_w(M22Script::to_string(tempVec.back()).c_str());
		M22ScriptCompiler::CompileLoadScriptFile(M22Script::to_string(tempVec.back()));

		std::vector<std::wstring> temp;
		std::wstring backgroundname;
		M22Script::SplitString(M22Script::to_wstring(savegameObject.CURRENTBACKGROUND.c_str()), temp, '/');
		backgroundname = temp.back();
		backgroundname.erase(backgroundname.end()-5,backgroundname.end());

		temp.clear();
		temp.shrink_to_fit();
		temp.push_back(M22Script::to_wstring("DrawBackground"));
		temp.push_back(backgroundname);

		//temp.push_back(temp.back());
		M22ScriptCompiler::line_c temporaryLineC;
		temporaryLineC.m_lineType = M22Script::NEW_BACKGROUND;
		temporaryLineC.m_parameters_txt.push_back(M22Script::to_string(backgroundname));
		M22ScriptCompiler::ExecuteCommand(temporaryLineC, savegameObject.CURRENTLINEPOSITION);
		//M22Script::ExecuteM22ScriptCommand(M22Script::LINETYPE::NEW_BACKGROUND, temp, savegameObject.CURRENTLINEPOSITION);


	};
	return;
};