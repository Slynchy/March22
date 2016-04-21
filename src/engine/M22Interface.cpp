#include "../M22Engine.h"

std::vector<M22Interface::Interface> M22Interface::storedInterfaces;
std::vector<M22Interface::Interface*> M22Interface::activeInterfaces;
bool M22Interface::DRAW_TEXT_AREA = true; 
M22Interface::BUTTON_STATES* M22Interface::skipButtonState;
M22Interface::BUTTON_STATES* M22Interface::menuButtonState;
bool M22Interface::menuOpen = false;
SDL_Texture* M22Interface::ChatBoxRenderer = NULL;

short int M22Interface::InitializeInterface(M22Interface::Interface* _interface, int _num_of_buttons, int _startline, const std::string _filename, bool _opaque, M22Interface::INTERFACES _type)
{
	printf("Loading %s...\n", _filename.c_str());

	std::vector<std::string> directory;
	M22Script::SplitString(_filename, directory, '/');

	if(_opaque == true)
	{
		_interface->alpha = 255.0f;
	};
	_interface->type = _type;
	std::fstream input(_filename);
	std::string temp;
	std::vector<std::string> tempStr;

	getline(input,temp);
	M22Script::SplitString(temp, tempStr, ' ');
	if(tempStr[1] == "BLANK")
	{
		_interface->spriteSheet = IMG_LoadTexture(M22Engine::SDL_RENDERER, "graphics/BLANK.png");
	}
	else
	{
		temp = "";
		for(size_t i = 0; i < (directory.size()-1); i++)
		{
			temp += directory[i];
		};
		temp += tempStr[1];
		_interface->spriteSheet = IMG_LoadTexture(M22Engine::SDL_RENDERER, temp.c_str());
	};

	if(input)
	{
		for( int i = 0; i < _startline; i++)
		{
			getline(input,temp);
		};
		for( int k = _startline; k < (_num_of_buttons + _startline); k++ )
		{
			getline(input,temp);
			tempStr.clear();
			M22Script::SplitString(temp, tempStr, ' ');

			std::string fileName;

			for(size_t i = 0; i < size_t(directory.size()-1); i++)
			{
				fileName += directory[i];
			};

			tempStr[0].erase(std::remove_if(tempStr[0].begin(), tempStr[0].end(), isspace));
			fileName += tempStr[0];
			fileName += ".webp";
			
			tempStr[1].erase(std::remove_if(tempStr[1].begin(), tempStr[1].end(), isspace));

			_interface->buttons.resize(_num_of_buttons);
			_interface->buttons[k-_startline].sheet = IMG_LoadTexture(M22Engine::SDL_RENDERER, fileName.c_str());
			SDL_SetTextureBlendMode(_interface->buttons[k-_startline].sheet, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod( _interface->buttons[k-_startline].sheet, Uint8(_interface->alpha) );
			_interface->buttons[k-_startline].name = tempStr[0];
			if(!_interface->buttons[k-_startline].sheet)
			{
				printf("Failed to load a button file: %s\n", fileName.c_str());
				return -2;
			};

			if( tempStr[1] == "vertical" )
			{
				int _X,_Y,_PosX,_PosY;
				_X = std::stoi(tempStr[2]);
				_Y = std::stoi(tempStr[3]);
				_PosX = std::stoi(tempStr[4]);
				_PosY = std::stoi(tempStr[5]);
				for( int i = 0; i < M22Interface::BUTTON_STATES::NUM_OF_BUTTON_STATES; i++ )
				{
					SDL_Rect tempSrc = { 0 , 0 + ( _Y * i ) , _X , _Y };
					SDL_Rect tempDst = { _PosX , _PosY , _X , _Y };
					_interface->buttons[k-_startline].rectSrc[i] = tempSrc;
					_interface->buttons[k-_startline].rectDst[i] = tempDst;
				};
			}
			else
			{
				int _X,_Y,_PosX,_PosY;
				_X = std::stoi(tempStr[2]);
				_Y = std::stoi(tempStr[3]);
				_PosX = std::stoi(tempStr[4]);
				_PosY = std::stoi(tempStr[5]);
				for( int i = 0; i < M22Interface::BUTTON_STATES::NUM_OF_BUTTON_STATES; i++ )
				{
					SDL_Rect tempSrc = { 0 + ( _X * i ) , 0 , _X , _Y };
					SDL_Rect tempDst = { _PosX , _PosY , _X , _Y };
					_interface->buttons[k-_startline].rectSrc[i] = tempSrc;
					_interface->buttons[k-_startline].rectDst[i] = tempDst;
				};
			};

			if(tempStr[0] == "SKIP")
			{
				M22Interface::skipButtonState = &_interface->buttons[k-_startline].state;
			};
			if(tempStr[0] == "M_MENU")
			{
				M22Interface::menuButtonState = &_interface->buttons[k-_startline].state;
			};
		};
	}
	else
	{
		printf("Failed to load script file: %s", _filename.c_str());
		return -1;
	};
	input.close();
	return 0;	
};

void M22Interface::DrawActiveInterfacesButtons(void)
{
	for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
	{
		for(size_t k = 0; k < M22Interface::activeInterfaces[i]->buttons.size(); k++)
		{
			SDL_RenderCopyEx(
				M22Engine::SDL_RENDERER, M22Interface::activeInterfaces[i]->buttons[k].sheet, 
				&M22Interface::activeInterfaces[i]->buttons[k].rectSrc[M22Interface::activeInterfaces[i]->buttons[k].state], 
				&M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state], 
				NULL,
				NULL,
				SDL_FLIP_NONE);
		};
	};
	return;
};

void M22Interface::DrawActiveInterfaces(void)
{
	for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
	{
		if(M22Interface::activeInterfaces[i]->spriteSheet != NULL)
		{
			SDL_RenderCopy( M22Engine::SDL_RENDERER, M22Interface::activeInterfaces[i]->spriteSheet, NULL, NULL);
		};
		for(size_t k = 0; k < M22Interface::activeInterfaces[i]->buttons.size(); k++)
		{
			SDL_RenderCopyEx(
				M22Engine::SDL_RENDERER, M22Interface::activeInterfaces[i]->buttons[k].sheet, 
				&M22Interface::activeInterfaces[i]->buttons[k].rectSrc[M22Interface::activeInterfaces[i]->buttons[k].state], 
				&M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state], 
				NULL,
				NULL,
				SDL_FLIP_NONE);
		};
	};
	return;
};

void M22Interface::InitTextBox(void)
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

void M22Interface::DrawTextArea(int _ScrSizeX, int _ScrSizeY)
{
	if(M22Interface::DRAW_TEXT_AREA == true)
	{
		int width = 640, height = 480;

		SDL_SetRenderTarget(M22Engine::SDL_RENDERER, M22Interface::ChatBoxRenderer);

		SDL_SetRenderDrawColor(M22Engine::SDL_RENDERER, 0,0,0,0);
		SDL_RenderClear(M22Engine::SDL_RENDERER);
		
		SDL_Rect textbox = {0, 480, 0, 0};
		SDL_QueryTexture(M22Graphics::textFrame, NULL, NULL, &textbox.w, &textbox.h);
		textbox.y = 480 - textbox.h;
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::textFrame, NULL, &textbox);

		SDL_Rect characterName = {0,368,0,0};
		SDL_QueryTexture(M22Graphics::characterFrameHeaders[M22Script::activeSpeakerIndex], NULL, NULL, &characterName.w, &characterName.h);
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::characterFrameHeaders[M22Script::activeSpeakerIndex], NULL, &characterName);

		M22Graphics::DrawArrow(width, height);
		M22Script::DrawCurrentLine(width, height);
		for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
		{
			if(M22Interface::activeInterfaces[i]->spriteSheet != NULL)
			{
				SDL_RenderCopy( M22Engine::SDL_RENDERER, M22Interface::activeInterfaces[i]->spriteSheet, NULL, NULL);
			};
		};
		M22Interface::DrawActiveInterfacesButtons();

		SDL_SetRenderTarget(M22Engine::SDL_RENDERER, NULL);

		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Interface::ChatBoxRenderer, NULL, NULL);
		SDL_SetRenderDrawColor(M22Engine::SDL_RENDERER, 255,255,255,255);
	};
	return;
};

void M22Interface::UpdateActiveInterfaces(int _ScrSizeX, int _ScrSizeY)
{
	for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
	{
		if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU)
		{
			if(M22Interface::activeInterfaces[i]->alpha < (255*0.6))
			{
				return;
			};
		};

		for(size_t k = 0; k < M22Interface::activeInterfaces[i]->buttons.size(); k++)
		{
			Vec2* tempButtonPos = new Vec2(0.0,0.0);
			tempButtonPos->x(M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].x);
			tempButtonPos->y(M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].y);
			Vec2* tempButtonSize = new Vec2(M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].w,
											M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].h
											);
			if(M22Interface::CheckOverlap(	M22Engine::MousePos, 
											*tempButtonPos, 
											*tempButtonSize
										 ) == true)
			{
				// mouseover
				if(M22Engine::LMB_Pressed)
				{
					if(M22Interface::activeInterfaces[i]->buttons[k].name == "SKIP")
					{
						M22Engine::skipping = !M22Engine::skipping;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "M_MENU")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Interface::menuOpen = !M22Interface::menuOpen;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "START")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Engine::StartGame();
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "QUIT" || M22Interface::activeInterfaces[i]->buttons[k].name == "QUIT_GAME")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Graphics::FadeToBlackFancy();
						M22Engine::QUIT = true;
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "TEST_SFX")
					{
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
						M22Sound::PlaySting(rand()%M22Sound::SOUND_FX.size(), true);
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "OPTIONS")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Interface::activeInterfaces.clear();
						M22Interface::menuOpen = false;
						M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::OPTIONS_MENU_INTRFC]);
						return;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "RETURN_TO_TITLE")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Engine::ResetGame();
						return;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "FULLSCREEN")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Engine::OPTIONS.WINDOWED = M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS;
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
						return;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "WINDOWED")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						M22Engine::OPTIONS.WINDOWED = M22Engine::WINDOW_STATES::WINDOWED;
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
						return;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "CANCEL_SHADED" || M22Interface::activeInterfaces[i]->buttons[k].name == "OK_SHADED")
					{
						M22Sound::PlaySting("sfx/stings/buttonclick.OGG", true);
						if(M22Interface::activeInterfaces[i]->buttons[k].name == "OK_SHADED")
						{
							M22Engine::SaveOptions();
							M22Engine::UpdateOptions();
						};
						M22Interface::activeInterfaces.clear();
						if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
						{
							M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::INGAME_INTRFC]);
							M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MENU_BUTTON_INTRFC]);
							M22Interface::menuOpen = true;
						}
						else if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU)
						{
							M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC]);
						};
						return;
					}
					else
					{
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::MOUSEOVER;
					};
				}
				else
				{
					if(M22Interface::activeInterfaces[i]->buttons[k].state == M22Interface::BUTTON_STATES::RESTING)
					{
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::MOUSEOVER;
						M22Sound::PlaySting("sfx/stings/mouseover.OGG", true);
					};
				};
				//break;
			}
			else
			{
				M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::RESTING;
			};
			delete tempButtonPos;
			delete tempButtonSize;
		};
	};
	
	if(M22Engine::skipping == true)
	{
		*M22Interface::skipButtonState = M22Interface::BUTTON_STATES::CLICKED;
	};
	if(M22Interface::menuOpen == true)
	{
		*M22Interface::menuButtonState = M22Interface::BUTTON_STATES::CLICKED;
		if(M22Interface::activeInterfaces.size() < 2)
		{
			M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[1]);
		};
	}
	else
	{
		if(M22Interface::activeInterfaces.size() > 1 && M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
		{
			M22Interface::activeInterfaces.pop_back();
		};
	};


	for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
	{
		for(size_t k = 0; k < M22Interface::activeInterfaces[i]->buttons.size(); k++)
		{
			if(M22Interface::activeInterfaces[i]->buttons[k].name == "WINDOWED" && M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::WINDOWED)
			{
				M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
				i = INT_MAX;
				break;
			};
			if(M22Interface::activeInterfaces[i]->buttons[k].name == "FULLSCREEN" && M22Engine::OPTIONS.WINDOWED == M22Engine::WINDOW_STATES::FULLSCREEN_BORDERLESS)
			{
				M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::CLICKED;
				i = INT_MAX;
				break;
			};
		};
	};

	return;
};

bool M22Interface::CheckOverlap(Vec2 _pos1, Vec2 _pos2, Vec2 _size)
{
	//Let's examine the x axis first:
	//If the leftmost or rightmost point of the first sprite lies somewhere inside the second, continue.
	if( (_pos1.x() >= _pos2.x() && _pos1.x() <= (_pos2.x() + _size.x())) ||
		((_pos1.x()) >= _pos2.x() && (_pos1.x()) <= (_pos2.x() + _size.x())) ){
		//Now we look at the y axis:
		if( (_pos1.y() >= _pos2.y() && _pos1.y() <= (_pos2.y() + _size.y())) ||
		    ((_pos1.y()) >= _pos2.y() && (_pos1.y()) <= (_pos2.y() + _size.y())) ){
			//The sprites appear to overlap.
			return true;
		};
	};
	//The sprites do not overlap:
	return false;
};

void M22Interface::ResetStoredInterfaces(void)
{
	for(size_t i = 0; i < M22Interface::storedInterfaces.size(); i++)
	{
		M22Interface::storedInterfaces[i].alpha = 255.0f;
	};
	for(size_t i = 0; i < M22Interface::storedInterfaces[MAIN_MENU_INTRFC].buttons.size(); i++)
	{
		SDL_SetTextureAlphaMod( M22Interface::storedInterfaces[MAIN_MENU_INTRFC].buttons[i].sheet, 0 );
	};
	return;
};