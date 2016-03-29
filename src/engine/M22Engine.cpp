#include "M22Engine.h"

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::MAIN_MENU;
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

void M22Engine::StartGame(void)
{
	M22Engine::GAMESTATE = M22Engine::GAMESTATES::INGAME;
	SDL_SetTextureAlphaMod( M22Graphics::activeMenuBackground.sprite, 0 );
	SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, 0 );
	M22Interface::activeInterfaces.clear();
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
		input >> length;
		getline(input,temp);
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
		temp += std::to_string(i);
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

int M22Engine::GetCharacterIndexFromName(std::string _input)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	_input.erase(std::remove_if(_input.begin(), _input.end(), M22Script::isColon));
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