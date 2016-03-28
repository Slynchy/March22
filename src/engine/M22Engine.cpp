#include "M22Engine.h"

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::INGAME;
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

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float M22Sound::MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
float M22Sound::SFX_VOLUME = (float)DEFAULT_SFX_VOLUME_MULT;
int M22Sound::currentTrack = 0;
std::vector<std::string> M22Sound::MUSIC_NAMES;
std::vector<std::string> M22Sound::SFX_NAMES;

std::vector<SDL_Texture*> M22Graphics::BACKGROUNDS;
SDL_Texture* M22Graphics::textFrame;
M22Graphics::ArrowObj M22Graphics::arrow;
std::vector<SDL_Texture*> M22Graphics::characterFrameHeaders;
TTF_Font* M22Graphics::textFont = NULL;
std::vector<std::string> M22Graphics::backgroundIndex;
std::vector<M22Engine::CharacterReference> M22Graphics::activeCharacters;
std::vector<SDL_Texture*> M22Graphics::mainMenuBackgrounds;
M22Engine::Background M22Graphics::activeMenuBackground;
M22Engine::Background M22Graphics::menuLogo;

std::string M22Script::currentLine;
int M22Script::currentLineIndex = NULL;
std::vector<std::string> M22Script::currentScript;
int M22Script::activeSpeakerIndex = 1;
SDL_Surface* M22Script::currentLineSurface = NULL;
SDL_Surface* M22Script::currentLineSurfaceShadow = NULL;
float M22Script::fontSize;

std::vector<M22Interface::Interface> M22Interface::activeInterfaces;

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
			M22Graphics::backgroundIndex.push_back(currentfile);
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
	std::string filename = "scripts/";
	filename += _filename;
	std::fstream input(filename);
	int length;
	std::string temp;
	if(input)
	{
		input >> length;
		getline(input,temp);
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

/*
	An extremely slow function (comparitively) for drawing decent-looking text.
	Goes from 9999+ FPS to 7000FPS when uncapped.
	Works fine for visual novels though, since there's two pieces of text drawn at any time.
*/
void M22Script::DrawCurrentLine(int ScrW, int ScrH)
{
	SDL_Color tempCol = {255,255,255,255};
	SDL_Color tempCol2 = {0,0,0,255};

	M22Script::currentLineSurfaceShadow =	TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol2,	ScrW-12 );
	M22Script::currentLineSurface =			TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol,		ScrW-12 );
	
	if(M22Script::currentLineSurfaceShadow == NULL)
	{
		printf("Failed to draw message shadow!\n");
		return;
	};
	if(M22Script::currentLineSurface == NULL)
	{
		printf("Failed to draw message!\n");
		return;
	};
	
	SDL_Texture* temp2 =	SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurfaceShadow);
	SDL_Texture* temp =		SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurface);
	
	SDL_Rect tempDst2= {(int)(ScrW*0.01875)+int(1.0f*M22Script::fontSize),(int)(ScrH*0.8416666666666667f)+int(1.0f*M22Script::fontSize),0,0};
	SDL_Rect tempDst = {(int)(ScrW*0.01875),(int)(ScrH*0.8416666666666667f),0,0};
	
	SDL_QueryTexture(temp2, NULL, NULL, &tempDst2.w, &tempDst2.h);
	SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
	
	SDL_RenderCopy(M22Engine::SDL_RENDERER, temp2, NULL, &tempDst2);
	SDL_RenderCopy(M22Engine::SDL_RENDERER, temp, NULL, &tempDst);

	SDL_DestroyTexture(temp2);
	SDL_DestroyTexture(temp);
	SDL_FreeSurface(currentLineSurfaceShadow);
	SDL_FreeSurface(currentLineSurface);

	return;
};

/*
	By Baltasarq from: http://stackoverflow.com/questions/5888022/split-string-by-single-spaces
*/
unsigned int M22Script::SplitString(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    unsigned int pos = txt.find( ch );
    unsigned int initialPos = 0;
    strs.clear();

    // Decompose statement
    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos + 1 ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }

    // Add the last one
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
};

void M22Script::ChangeLine(int _newLine)
{
	// Check the line even exists
	if(size_t(_newLine) > M22Script::currentScript.size())
	{
		printf("Cannot change line! Out of bounds!\n");
		return;
	};

	// Update currentLine variable
	M22Script::currentLine = M22Script::currentScript[_newLine];

	// Get type of line
	std::vector<std::string> temp;
	M22Script::SplitString(M22Script::currentLine, temp, ' ');
	std::string type = temp[0];

	M22Script::LINETYPE LINETYPE = M22Script::CheckLineType(type);

	if(LINETYPE == M22Script::LINETYPE::NEW_BACKGROUND)
	{
		for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
		{
			std::string tempPath = "graphics/backgrounds/";
			tempPath += temp[1];
			tempPath += ".webp";
			if(tempPath == M22Graphics::backgroundIndex[i])
			{
				if(M22Engine::ACTIVE_BACKGROUNDS[0].sprite)
				{
					M22Engine::ACTIVE_BACKGROUNDS[1].sprite = M22Graphics::BACKGROUNDS[i];
					SDL_SetTextureBlendMode(M22Engine::ACTIVE_BACKGROUNDS[1].sprite, SDL_BLENDMODE_BLEND);
					SDL_SetTextureAlphaMod( M22Engine::ACTIVE_BACKGROUNDS[1].sprite, 0 );
				}
				else
				{
					M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
				};
				M22Script::ChangeLine(++_newLine);
				return;
			};
		};
	}
	else if(LINETYPE == M22Script::LINETYPE::NEW_MUSIC)
	{
		std::string tempPath = "sfx/music/";
		tempPath += temp[1];
		tempPath += ".OGG";
		M22Sound::ChangeMusicTrack(tempPath);
		M22Script::ChangeLine(++_newLine);
		return;
	}
	else if(LINETYPE == M22Script::LINETYPE::STOP_MUSIC)
	{
		M22Sound::ChangeMusicTrack("sfx/music/SILENCE.OGG");
		M22Script::ChangeLine(++_newLine);
		return;
	}
	else if(LINETYPE == M22Script::LINETYPE::FADE_TO_BLACK)
	{
		for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
		{
			if("graphics/backgrounds/BLACK.webp" == M22Graphics::backgroundIndex[i])
			{
				if(M22Engine::ACTIVE_BACKGROUNDS[0].sprite)
				{
					M22Engine::ACTIVE_BACKGROUNDS[1].sprite = M22Graphics::BACKGROUNDS[i];
					SDL_SetTextureBlendMode(M22Engine::ACTIVE_BACKGROUNDS[1].sprite, SDL_BLENDMODE_BLEND);
					SDL_SetTextureAlphaMod( M22Engine::ACTIVE_BACKGROUNDS[1].sprite, 0 );
				}
				else
				{
					M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
				};
				M22Script::ChangeLine(++_newLine);
				return;
			};
		};
	}
	else if(LINETYPE == M22Script::LINETYPE::PLAY_STING)
	{
		std::string tempPath = "sfx/stings/";
		tempPath += temp[1];
		tempPath += ".OGG";
		M22Sound::PlaySting(tempPath, true);
		M22Script::ChangeLine(++_newLine);
		return;
	}
	else if(LINETYPE == M22Script::LINETYPE::DRAW_CHARACTER)
	{
		M22Engine::CharacterReference tempChar;
		int charIndex, outfitIndex, emotionIndex;
		charIndex = M22Engine::GetCharacterIndexFromName(temp[1]);
		outfitIndex = M22Engine::GetOutfitIndexFromName(temp[2], charIndex);
		emotionIndex = M22Engine::GetEmotionIndexFromName(temp[3], charIndex);
		tempChar.sprite = M22Engine::CHARACTERS_ARRAY[charIndex].sprites[outfitIndex][emotionIndex];

		SDL_QueryTexture(tempChar.sprite, NULL, NULL, &tempChar.rect.w, &tempChar.rect.h);
		tempChar.rect.x = atoi(temp[4].c_str());
		tempChar.rect.y = 0;

		M22Graphics::activeCharacters.push_back(tempChar);
		M22Script::ChangeLine(++_newLine);
		return;
	}
	else if(LINETYPE == M22Script::LINETYPE::CLEAR_CHARACTERS)
	{
		//M22Graphics::activeCharacters.clear();
		for(size_t i = 0; i < M22Graphics::activeCharacters.size(); i++)
		{
			M22Graphics::activeCharacters[i].clearing = true;
		};
		M22Script::ChangeLine(++_newLine);
		return;
	}
	else if(LINETYPE == M22Script::LINETYPE::LOAD_SCRIPT)
	{
		M22Script::currentScript.clear();
		LoadScriptToCurrent(temp[1].c_str());
		M22Script::ChangeLine(0);
		return;
	}
	else
	{
		M22Script::activeSpeakerIndex = M22Engine::GetCharacterIndexFromName(type);
	};

	if(M22Script::activeSpeakerIndex > 0)
	{
		std::string tempName = type;
		M22Script::currentLine.erase(0, tempName.length());
	};

	M22Script::currentLineIndex = _newLine;
	return;
};

/*
	Checks the string against database of character names
	and returns the character's enumerator
*/
M22Script::LINETYPE M22Script::CheckLineType(std::string _input)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	if(_input == std::string("DrawBackground"))
	{
		return M22Script::LINETYPE::NEW_BACKGROUND;
	}
	else if(_input == std::string("PlayMusic"))
	{
		return M22Script::LINETYPE::NEW_MUSIC;
	}
	else if(_input == std::string("PlaySting"))
	{
		return M22Script::LINETYPE::PLAY_STING;
	}
	else if(_input == std::string("DrawCharacter"))
	{
		return M22Script::LINETYPE::DRAW_CHARACTER;
	}
	else if(_input == std::string("ClearCharacters"))
	{
		return M22Script::LINETYPE::CLEAR_CHARACTERS;
	}
	else if(_input == std::string("LoadScript"))
	{
		return M22Script::LINETYPE::LOAD_SCRIPT;
	}
	else if(_input == std::string("FadeToBlack"))
	{
		return M22Script::LINETYPE::FADE_TO_BLACK;
	}
	else if(_input == std::string("StopMusic"))
	{
		return M22Script::LINETYPE::STOP_MUSIC;
	};
	return M22Script::LINETYPE::SPEECH;
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

bool isColon(int _char)
{
	if(_char == ':') 
	{
		return true;
	}
	else
	{
		return false;
	};
};

int M22Engine::GetCharacterIndexFromName(std::string _input)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	_input.erase(std::remove_if(_input.begin(), _input.end(), isColon));
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

void M22Graphics::DrawBackground(SDL_Texture* _target)
{
	if(_target)
	{
		// Backgrounds match the 640x480 resolution, so no need for scaling... for now.
		SDL_RenderCopy(M22Engine::SDL_RENDERER, _target, NULL, NULL);
	}
	else
	{
		// non existent
	};
	return;
};

void M22Graphics::UpdateBackgrounds(void)
{
	if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU)
	{
		if(M22Graphics::activeMenuBackground.alpha < 254)
		{
			M22Graphics::activeMenuBackground.alpha = M22Graphics::Lerp(M22Graphics::activeMenuBackground.alpha, 255.0f, DEFAULT_LERP_SPEED/4);
			SDL_SetTextureAlphaMod( M22Graphics::activeMenuBackground.sprite, Uint8(M22Graphics::activeMenuBackground.alpha) );
		}
		else if(M22Graphics::menuLogo.alpha < 254)
		{
			M22Graphics::activeMenuBackground.alpha = 255;
			SDL_SetTextureAlphaMod( M22Graphics::activeMenuBackground.sprite, Uint8(M22Graphics::activeMenuBackground.alpha) );
			M22Graphics::menuLogo.alpha = M22Graphics::Lerp(M22Graphics::menuLogo.alpha, 255.0f, DEFAULT_LERP_SPEED/8);
			SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, Uint8(M22Graphics::menuLogo.alpha) );
		}
		else
		{
			M22Graphics::menuLogo.alpha = 255;
			SDL_SetTextureAlphaMod( M22Graphics::menuLogo.sprite, Uint8(M22Graphics::menuLogo.alpha) );
		};
	}
	else
	{
		if(M22Engine::ACTIVE_BACKGROUNDS[1].sprite != NULL)
		{
			if(M22Engine::ACTIVE_BACKGROUNDS[1].alpha < 255)
			{
				M22Engine::ACTIVE_BACKGROUNDS[1].alpha = M22Graphics::Lerp(M22Engine::ACTIVE_BACKGROUNDS[1].alpha, 255.0f, DEFAULT_LERP_SPEED);
			}
			else
			{
				M22Engine::ACTIVE_BACKGROUNDS[0] = M22Engine::ACTIVE_BACKGROUNDS[1];
				M22Engine::ACTIVE_BACKGROUNDS[1].sprite = NULL;
				M22Engine::ACTIVE_BACKGROUNDS[1].alpha = NULL;
			};
		};
		SDL_SetTextureAlphaMod( M22Engine::ACTIVE_BACKGROUNDS[0].sprite, Uint8(M22Engine::ACTIVE_BACKGROUNDS[0].alpha) );
		SDL_SetTextureAlphaMod( M22Engine::ACTIVE_BACKGROUNDS[1].sprite, Uint8(M22Engine::ACTIVE_BACKGROUNDS[1].alpha) );
	};
	return;
};

void M22Graphics::UpdateCharacters(void)
{
	if(M22Graphics::activeCharacters.size() == 0) return;
	for(size_t k = M22Graphics::activeCharacters.size(); k > 0; k--)
	{
		int i = k-1;
		if(M22Graphics::activeCharacters[i].clearing == false)
		{
			if(M22Graphics::activeCharacters[i].alpha < 255.0f)
			{
				M22Graphics::activeCharacters[i].alpha = M22Graphics::Lerp(M22Graphics::activeCharacters[i].alpha, 255.0f, DEFAULT_LERP_SPEED);
				SDL_SetTextureAlphaMod(M22Graphics::activeCharacters[i].sprite, Uint8(M22Graphics::activeCharacters[i].alpha) );
			};
		}
		else
		{
			if(M22Graphics::activeCharacters[i].alpha > 0.5f)
			{
				M22Graphics::activeCharacters[i].alpha = M22Graphics::Lerp(M22Graphics::activeCharacters[i].alpha, 0.0f, DEFAULT_LERP_SPEED);
				SDL_SetTextureAlphaMod(M22Graphics::activeCharacters[i].sprite, Uint8(M22Graphics::activeCharacters[i].alpha) );
			}
			else
			{
				M22Graphics::activeCharacters.erase(M22Graphics::activeCharacters.begin()+i);
				M22Graphics::activeCharacters.shrink_to_fit();
			};
		};
	};
};

float M22Graphics::Lerp(float _var1, float _var2, float _t)
{
	float tempfl = _var1*(1-_t) + _var2*_t;
	return tempfl;
};

void M22Graphics::DrawArrow(int ScrW, int ScrH)
{
	M22Graphics::arrow.frame = M22Graphics::arrow.frame + ( 1.0f / 10.0f );
	if(M22Graphics::arrow.frame >= 6.0f)
	{
		M22Graphics::arrow.frame = 0.0f;
	};

	/*
		These beautiful magic numbers are the ratio of the desired value at the desired aspect ratio.
		By multiplying by the new resolution, we can get the scale/size/position at any scale (but not aspect)
	*/
	SDL_Rect tempSrc = { 22*((int)std::floor(M22Graphics::arrow.frame)+1), 0, 22, 22};
	SDL_Rect tempDst = { int(ScrW*0.9625f), int(ScrH*0.9541666666666667f), int(ScrW*0.034375f), int(ScrH*0.0458333333333333f)};

	SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::arrow.sprite, &tempSrc, &tempDst);
	return;
};