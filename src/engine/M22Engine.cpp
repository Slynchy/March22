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

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float M22Sound::MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
float M22Sound::SFX_VOLUME = (float)DEFAULT_SFX_VOLUME_MULT;
int M22Sound::currentTrack = 0;
std::vector<std::string> M22Sound::MUSIC_NAMES;
std::vector<std::string> M22Sound::SFX_NAMES;

std::vector<SDL_Texture*> M22Graphics::BACKGROUNDS;
SDL_Texture* M22Graphics::textFrame;
std::vector<SDL_Texture*> M22Graphics::characterFrameHeaders;
TTF_Font* M22Graphics::textFont = NULL;
std::vector<std::string> M22Graphics::backgroundIndex;
std::vector<M22Engine::CharacterReference> M22Graphics::activeCharacters;

std::string M22Script::currentLine;
int M22Script::currentLineIndex = NULL;
std::vector<std::string> M22Script::currentScript;
M22Engine::CHARACTERS M22Script::activeSpeakerIndex = M22Engine::CHARACTERS::YUUJI;
SDL_Surface* M22Script::currentLineSurface = NULL;
SDL_Surface* M22Script::currentLineSurfaceShadow = NULL;

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
	Works fine for visual novels though, since there's one piece of text drawn at any time.
*/
void M22Script::DrawCurrentLine(void)
{
	SDL_Color tempCol = {255,255,255,255};
	SDL_Color tempCol2 = {0,0,0,255};

	M22Script::currentLineSurfaceShadow =	TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol2,	640-12 );
	M22Script::currentLineSurface =			TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol,		640-12 );
	
	if(M22Script::currentLineSurfaceShadow == NULL)
	{
		printf("Failed to draw message!\n");
		return;
	};
	if(M22Script::currentLineSurface == NULL)
	{
		printf("Failed to draw message!\n");
		return;
	};
	
	SDL_Texture* temp2 =	SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurfaceShadow);
	SDL_Texture* temp =		SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurface);
	
	SDL_Rect tempDst2= {13,405,0,0};
	SDL_Rect tempDst = {12,404,0,0};
	
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
		emotionIndex = M22Engine::GetEmotionIndexFromName(temp[3]);
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
		M22Graphics::activeCharacters.clear();
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
		M22Script::activeSpeakerIndex = M22Script::CheckCharacter(&type);
	};

	if(M22Script::activeSpeakerIndex > 0)
	{
		M22Script::currentLine.erase(0, M22Engine::CHARACTER_NAMES[M22Script::activeSpeakerIndex].length());
	};

	M22Script::currentLineIndex = _newLine;
	return;
};

/*
	Checks the string against database of character names
	and returns the character's enumerator
*/
M22Engine::CHARACTERS M22Script::CheckCharacter(std::string* _input)
{
	for(size_t i = 0; i < M22Engine::CHARACTERS::TOTAL_CHARACTERS; i++)
	{
		std::string temp = M22Engine::CHARACTER_NAMES[i];
		temp += char(32);
		if(temp.compare(*_input) == 0)
		{
			// MATCH!
			return M22Engine::CHARACTERS(i);
		};
	};
	return M22Engine::CHARACTERS(0);
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
	};
	return M22Script::LINETYPE::SPEECH;
};

short int M22Engine::InitializeM22(void)
{
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

	for(int i = 0; i < length; i++)
	{
		temp.clear();
		temp = "graphics/text_frames/";
		temp += std::to_string(i);
		temp += ".png";
		M22Graphics::characterFrameHeaders.push_back(IMG_LoadTexture(M22Engine::SDL_RENDERER, temp.c_str()));
		temp.clear();
	};

	input.open("graphics/characters/EMOTIONS.txt");
	if(input)
	{
		input >> length;
		getline(input,temp);
		M22Engine::CHARACTER_EMOTIONS.clear();
		for(int i = 0; i < length; i++)
		{
			getline(input,temp);
			M22Engine::CHARACTER_EMOTIONS.push_back(temp);
		};
	}
	else
	{
		std::cout << "Failed to load script file: " << "graphics/characters/EMOTIONS.txt" << std::endl;
		return -1;
	};
	input.close();

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

	for(size_t i = 0; i < CHARACTERS_ARRAY.size(); i++)
	{
		CHARACTERS_ARRAY[i].sprites.resize(CHARACTERS_ARRAY[i].outfits.size());
		for(size_t p = 0; p < CHARACTERS_ARRAY[i].outfits.size(); p++)
		{
			for(size_t k = 0; k < CHARACTER_EMOTIONS.size(); k++)
			{
				std::string tempPath = "graphics/characters/";
				tempPath += CHARACTERS_ARRAY[i].name;
				tempPath += "/";
				tempPath += CHARACTERS_ARRAY[i].outfits[p];
				tempPath += "/";
				tempPath += CHARACTER_EMOTIONS[k];
				tempPath += ".png";
				M22Engine::CHARACTERS_ARRAY[i].sprites[p].push_back(IMG_LoadTexture(M22Engine::SDL_RENDERER, tempPath.c_str()));
			};
		};
	};
	
	M22Engine::ACTIVE_BACKGROUNDS.resize(2);
	return 0;
};

int M22Engine::GetCharacterIndexFromName(std::string _input)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
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

int M22Engine::GetEmotionIndexFromName(std::string _input)
{
	_input.erase(std::remove_if(_input.begin(), _input.end(), isspace));
	for(size_t i = 0; i < M22Engine::CHARACTER_EMOTIONS.size(); i++)
	{
		if(M22Engine::CHARACTER_EMOTIONS[i] == _input)
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
	if(M22Engine::ACTIVE_BACKGROUNDS[1].sprite != NULL)
	{
		if(M22Engine::ACTIVE_BACKGROUNDS[1].alpha < 255)
		{
			M22Engine::ACTIVE_BACKGROUNDS[1].alpha = unsigned short int(M22Graphics::Lerp(M22Engine::ACTIVE_BACKGROUNDS[1].alpha, 255.0f, DEFAULT_LERP_SPEED));
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
	return;
};

float M22Graphics::Lerp(float _var1, float _var2, float _t)
{
	float tempfl = _var1*(1-_t) + _var2*_t;
	return tempfl;
};