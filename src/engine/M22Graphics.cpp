#include "../M22Engine.h"

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
SDL_Texture* M22Graphics::darkScreen;

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