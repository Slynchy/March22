#include "M22Engine.h"

M22Engine::GAMESTATES M22Engine::GAMESTATE = M22Engine::GAMESTATES::INGAME;
SDL_Window* M22Engine::SDL_SCREEN = NULL;
SDL_Renderer *M22Engine::SDL_RENDERER = NULL;
SDL_Event M22Engine::SDL_EVENTS;
const Uint8 *M22Engine::SDL_KEYBOARDSTATE = NULL;
std::vector<std::string> M22Engine::CHARACTER_NAMES;
unsigned short int M22Engine::ACTIVE_BACKGROUND_INDEX = 0;

std::vector<Mix_Chunk*> M22Sound::SOUND_FX;
std::vector<Mix_Music*> M22Sound::MUSIC;
float M22Sound::MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
float M22Sound::SFX_VOLUME = (float)DEFAULT_SFX_VOLUME_MULT;
int M22Sound::currentTrack = 0;

std::vector<SDL_Texture*> M22Graphics::BACKGROUNDS;
SDL_Texture* M22Graphics::textFrame;
std::vector<SDL_Texture*> M22Graphics::characterFrameHeaders;
TTF_Font* M22Graphics::textFont = NULL;

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

void M22Script::ChangeLine(int _newLine)
{
	if(size_t(_newLine) > M22Script::currentScript.size())
	{
		printf("Cannot change line! Out of bounds!\n");
		return;
	};

	M22Script::currentLine = M22Script::currentScript[_newLine];

	std::vector<std::string> temp;
	M22Script::SplitString(M22Script::currentLine, temp, ' ');
	std::string character = temp[0];

	M22Script::activeSpeakerIndex = M22Script::CheckCharacter(&character);

	if(M22Script::activeSpeakerIndex > 0)
	{
		M22Script::currentLine.erase(0, M22Engine::CHARACTER_NAMES[M22Script::activeSpeakerIndex].length());
	};

	M22Script::currentLineIndex = _newLine;
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

	return 0;
};