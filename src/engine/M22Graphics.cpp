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
SDL_Texture* M22Graphics::BLACK_TEXTURE;

void M22Graphics::FadeToBlackFancy(void)
{
	M22Interface::DRAW_TEXT_AREA = false;
	float fade_to_black_alpha = 0;
	bool fadeout = false;
	while(fade_to_black_alpha < 250.0f || Mix_PlayingMusic())
	{
		fade_to_black_alpha = M22Graphics::Lerp( fade_to_black_alpha, 255.0f, DEFAULT_LERP_SPEED / 8);
		SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, Uint8(fade_to_black_alpha) );
		M22Graphics::DrawInGame();
		SDL_RenderPresent(M22Engine::SDL_RENDERER);
		if(fadeout == false)
		{
			Mix_FadeOutMusic(2000);	
			fadeout = true;
		};
		if(RENDERING_API == "direct3d") SDL_Delay(1000/60);
	};
	for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
	{
		if(M22Graphics::backgroundIndex[i] == "graphics/backgrounds/BLACK.webp")
		{
			M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
			M22Engine::ACTIVE_BACKGROUNDS[1].sprite = NULL;
		};
	};
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
	M22Interface::DRAW_TEXT_AREA = true;
	return;
};

void M22Graphics::DrawInGame(bool _draw_black)
{
	M22Graphics::DrawBackground(M22Engine::ACTIVE_BACKGROUNDS[0].sprite);
	if(M22Engine::ACTIVE_BACKGROUNDS[1].sprite != NULL) M22Graphics::DrawBackground(M22Engine::ACTIVE_BACKGROUNDS[1].sprite);
	if(M22Graphics::activeCharacters.size() != 0)
	{
		for(size_t i = 0; i < M22Graphics::activeCharacters.size(); i++)
		{
			SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::activeCharacters[i].sprite, NULL, &M22Graphics::activeCharacters[i].rect);
		};
	};
	if(_draw_black) SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::BLACK_TEXTURE, NULL, NULL);
	M22Interface::DrawTextArea((int)M22Engine::ScrSize.x(), (int)M22Engine::ScrSize.y());
	return;
};

short int M22Graphics::LoadBackgroundsFromIndex(const char* _filename)
{
	std::fstream input(_filename);
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
			M22Graphics::backgroundIndex.push_back(currentfile);
			SDL_Texture *temp = IMG_LoadTexture(M22Engine::SDL_RENDERER, currentfile.c_str());
			if(!temp)
			{
				std::cout << "Failed to load file: " << currentfile << std::endl;
				return -1;
			};
			M22Graphics::BACKGROUNDS.push_back(temp);

			// The following code is disgusting but solves conflict issues between the black background and the misc black texture
			if(currentfile == "graphics/backgrounds/BLACK.webp") 
			{
				temp = NULL;
				temp = IMG_LoadTexture(M22Engine::SDL_RENDERER, currentfile.c_str());
				M22Graphics::BLACK_TEXTURE = temp;
				SDL_SetTextureBlendMode(M22Graphics::BLACK_TEXTURE, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
				temp = NULL;
			};
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
		else if(M22Graphics::menuLogo.alpha < 245)
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
			M22Interface::activeInterfaces[0]->FadeInAllButtons();
		};
	}
	else
	{
		if(M22Engine::ACTIVE_BACKGROUNDS[1].sprite != NULL)
		{
			if(M22Engine::ACTIVE_BACKGROUNDS[1].alpha < 254)
			{
				M22Engine::ACTIVE_BACKGROUNDS[1].alpha = M22Graphics::Lerp(M22Engine::ACTIVE_BACKGROUNDS[1].alpha, 255.0f, DEFAULT_LERP_SPEED*1.5);
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

void M22Graphics::AddActiveCharacter(int _charindex, int _outfitindex, int _emotionindex, int _xPosition, bool _brutal)
{
	M22Engine::CharacterReference tempChar;
	tempChar.sprite = M22Engine::CHARACTERS_ARRAY[_charindex].sprites[_outfitindex][_emotionindex];
	SDL_QueryTexture(tempChar.sprite, NULL, NULL, &tempChar.rect.w, &tempChar.rect.h);
	tempChar.rect.x = _xPosition;
	tempChar.rect.y = 0;
	if(_brutal)
	{
		tempChar.alpha = 255.0f;
	};
	M22Graphics::activeCharacters.push_back(tempChar);
	return;
};

void M22Graphics::UpdateCharacters(void)
{
	if(M22Graphics::activeCharacters.size() == 0)
	{
		return;
	};
	for(size_t i = 0; i < M22Graphics::activeCharacters.size(); i++)
	{
		//int i = k-1;
		if(M22Graphics::activeCharacters[i].clearing == false)
		{
			if(M22Graphics::activeCharacters[i].alpha < 254.5f)
			{
				M22Graphics::activeCharacters[i].alpha = M22Graphics::Lerp(M22Graphics::activeCharacters[i].alpha, 255.0f, DEFAULT_LERP_SPEED);
				SDL_SetTextureAlphaMod(M22Graphics::activeCharacters[i].sprite, Uint8(M22Graphics::activeCharacters[i].alpha) );
			}
			else
			{
				M22Graphics::activeCharacters[i].alpha = 255.0f;
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
	SDL_Rect tempDst = { 616, 458, 0, 0};
	SDL_QueryTexture(M22Graphics::arrow.sprite, NULL, NULL, &tempDst.w, &tempDst.h);

	tempDst.w /= 7;

	SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::arrow.sprite, &tempSrc, &tempDst);
	return;
};