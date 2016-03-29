#include "../M22Engine.h"

std::vector<M22Interface::Interface> M22Interface::storedInterfaces;
std::vector<M22Interface::Interface*> M22Interface::activeInterfaces;
bool M22Interface::DRAW_TEXT_AREA = true; 
M22Interface::BUTTON_STATES* M22Interface::skipButtonState;
M22Interface::BUTTON_STATES* M22Interface::menuButtonState;
bool M22Interface::menuOpen = false;

short int M22Interface::InitializeInterface(M22Interface::Interface* _interface, int _num_of_buttons, int _startline = 0, const std::string _filename)
{
	printf("Loading %s...\n", _filename.c_str());

	std::vector<std::string> directory;
	M22Script::SplitString(_filename, directory, '/');

	std::fstream input(_filename);
	std::string temp;
	if(input)
	{
		for( int i = 0; i < _startline; i++)
		{
			getline(input,temp);
		};
		for( int k = _startline; k < (_num_of_buttons + _startline); k++ )
		{
			getline(input,temp);
			std::vector<std::string> tempStr;
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

void M22Interface::DrawActiveInterfaces(void)
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

void M22Interface::DrawTextArea(int _ScrSizeX, int _ScrSizeY)
{
	if(M22Interface::DRAW_TEXT_AREA == true)
	{
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::textFrame, NULL, NULL);
		SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Graphics::characterFrameHeaders[M22Script::activeSpeakerIndex], NULL, NULL);
		M22Interface::DrawActiveInterfaces();
		M22Graphics::DrawArrow(_ScrSizeX, _ScrSizeY);
		M22Script::DrawCurrentLine(_ScrSizeX, _ScrSizeY);
	};
	return;
};

void M22Interface::UpdateActiveInterfaces(int _ScrSizeX, int _ScrSizeY)
{
	for(size_t i = 0; i < M22Interface::activeInterfaces.size(); i++)
	{
		for(size_t k = 0; k < M22Interface::activeInterfaces[i]->buttons.size(); k++)
		{
			Vec2* tempButtonPos = new Vec2(M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].x,M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].y);
			Vec2* tempButtonSize = new Vec2(M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].w,M22Interface::activeInterfaces[i]->buttons[k].rectDst[M22Interface::activeInterfaces[i]->buttons[k].state].h);
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
						M22Interface::menuOpen = !M22Interface::menuOpen;
					}
					else if(M22Interface::activeInterfaces[i]->buttons[k].name == "START")
					{
						M22Engine::StartGame();
					}
					else
					{
						M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::MOUSEOVER;
					};
				}
				else
				{
					M22Interface::activeInterfaces[i]->buttons[k].state = M22Interface::BUTTON_STATES::MOUSEOVER;
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
		if(M22Interface::activeInterfaces.size() > 1)
		{
			M22Interface::activeInterfaces.pop_back();
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