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
	M22Script::ChangeLine(0);
	M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[0]);
	return;
};

short int M22Engine::InitializeM22(int ScrW, int ScrH)
{
	printf("Initializing M22...\n");

	printf("Loading CHARACTER_NAMES...\n");
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
		std::cout << "Failed to load script file: " << "scripts/CHARACTER_NAMES.txt" << std::endl;
		return -1;
	};
	input.close();
	
	printf("Loading CHARACTERS.txt...\n");
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
		std::cout << "Failed to load script file: " << "graphics/characters/CHARACTERS.txt" << std::endl;
		return -1;
	};
	input.close();
	
	printf("Loading character text frames...\n");
	for(int i = 0; i < length; i++)
	{
		temp.clear();
		temp = "graphics/text_frames/";
		temp += M22Engine::CHARACTERS_ARRAY[i].name;
		temp += ".png";
		M22Graphics::characterFrameHeaders.push_back(IMG_LoadTexture(M22Engine::SDL_RENDERER, temp.c_str()));
		temp.clear();
	};
	
	printf("Loading EMOTIONS.txt...\n");
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
	
	printf("Loading OUTFITS.txt...\n");
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
	
	printf("Loading character sprites...\n");
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
	
	printf("Loading main menu backgrounds...\n");
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
		std::cout << "Failed to load script file: " << "graphics/characters/CHARACTERS.txt" << std::endl;
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
	*M22Interface::skipButtonState = M22Interface::BUTTON_STATES::RESTING;
	*M22Interface::menuButtonState = M22Interface::BUTTON_STATES::RESTING;
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