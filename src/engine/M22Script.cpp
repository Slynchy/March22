#include "../M22Engine.h"

std::string M22Script::currentLine;
int M22Script::currentLineIndex = NULL;
std::vector<std::string> M22Script::currentScript;
int M22Script::activeSpeakerIndex = 1;
SDL_Surface* M22Script::currentLineSurface = NULL;
SDL_Surface* M22Script::currentLineSurfaceShadow = NULL;
float M22Script::fontSize;
std::vector<M22Script::Decision> M22Script::gameDecisions;

short int M22Script::LoadGameDecisions(const char* _filename)
{
	printf("[M22Script] Loading \"%s\" \n", _filename);
	std::fstream input(_filename);
	int length;
	std::string temp;
	std::vector<std::string> tempArr;
	if(input)
	{
		getline(input,temp);
		length = atoi(temp.c_str());
		M22Script::gameDecisions.resize(length);

		for(int i = 0; i < length; i++)
		{
			//Get name of decision + number of decisions
			getline(input,temp);

			//Split string and remove spaces
			M22Script::SplitString(temp, tempArr, ' ');
			tempArr[0].erase(std::remove_if(tempArr[0].begin(), tempArr[0].end(), isspace));
			tempArr[1].erase(std::remove_if(tempArr[1].begin(), tempArr[1].end(), isspace));

			//Push back the name of the decision to array
			M22Script::gameDecisions.at(i).name = tempArr.at(0);

			//Get number of decisions for upcoming for loop
			M22Script::gameDecisions.at(i).num_of_choices = atoi(tempArr.at(1).c_str());

			//For number of decisions, push back the decision
			for(int k = 0; k < M22Script::gameDecisions.at(i).num_of_choices; k++)
			{
				getline(input,temp);
				M22Script::gameDecisions.at(i).choices.push_back(temp);
			};
		};
	}
	else
	{
		printf("Failed to load decisions file: %s \n", _filename);
		return -1;
	};
	input.close();
	return 0;
};

short int M22Script::LoadScriptToCurrent(const char* _filename)
{
	std::string filename = "scripts/";
	filename += _filename;
	printf("[M22Script] Loading \"%s\" \n", filename.c_str());
	std::fstream input(filename);
	int length;
	std::string temp;
	if(input)
	{
		length=int(std::count(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>(), '\n'));
		length++; // Linecount is number of '\n' + 1
		input.seekg(0, std::ios::beg);
		M22Script::currentScript.clear();
		for(int i = 0; i < length; i++)
		{
			getline(input,temp);
			if(temp[0] == '/' && temp[1] == '/')
			{
				// it's a comment! Store only two chars to save space
				temp = "//";
			};
			M22Script::currentScript.push_back(temp);
		};
	}
	else
	{
		printf("Failed to load script file: %s \n", filename);
		return -1;
	};
	input.close();
	return 0;
};

/*
	An extremely slow function (comparitively) for drawing decent-looking text.
	Draws text then a "shadow" 1px adjacent @ 640x480
*/
void M22Script::DrawCurrentLine(int ScrW, int ScrH)
{
	SDL_Color tempCol = {255,255,255,255};
	SDL_Color tempCol2 = {0,0,0,255};

	M22Script::currentLineSurfaceShadow =	TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol2,	ScrW-12 );
	M22Script::currentLineSurface =			TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol,		ScrW-12 );
	
	if(M22Script::currentLineSurfaceShadow == NULL && M22Script::currentLine != "")
	{
		printf("Failed to draw message shadow!\n");
		return;
	};
	if(M22Script::currentLineSurface == NULL && M22Script::currentLine != "")
	{
		printf("Failed to draw message!\n");
		return;
	};
	
	SDL_Texture* temp2 =	SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurfaceShadow);
	SDL_Texture* temp =		SDL_CreateTextureFromSurface(M22Engine::SDL_RENDERER, M22Script::currentLineSurface);
	
//	SDL_Rect tempDst2= {(int)(ScrW*0.01875)+int(1.0f*M22Script::fontSize),(int)(ScrH*0.8416666666666667f)+int(1.0f*M22Script::fontSize),0,0};
//	SDL_Rect tempDst = {(int)(ScrW*0.01875),(int)(ScrH*0.8416666666666667f),0,0};
	SDL_Rect tempDst2 = {8+1,404+1,0,0};
	SDL_Rect tempDst = {8,404,0,0};
	
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

void M22Script::ClearCharacters(void)
{
	SDL_SetRenderTarget(M22Engine::SDL_RENDERER, M22Graphics::NEXT_BACKGROUND_RENDER_TARGET);
	SDL_SetRenderDrawColor(M22Engine::SDL_RENDERER, 0,0,0,0);
	SDL_RenderClear(M22Engine::SDL_RENDERER);
	
	SDL_RenderCopy(M22Engine::SDL_RENDERER, M22Engine::ACTIVE_BACKGROUNDS[0].sprite, NULL, NULL);
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );

	SDL_SetRenderTarget(M22Engine::SDL_RENDERER, NULL);
	SDL_SetRenderDrawColor(M22Engine::SDL_RENDERER, 255,255,255,255);

	M22Graphics::changeQueued = M22Graphics::BACKGROUND_UPDATE_TYPES::CHARACTER;

	return;
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

	//if(M22Script::ExecuteM22ScriptCommand(LINETYPE, temp, _newLine) == 1)
	//	return;
	M22Script::ExecuteM22ScriptCommand(LINETYPE, temp, _newLine);

	M22Script::currentLineIndex = _newLine;
	return;
};

short int M22Script::ExecuteM22ScriptCommand(M22Script::LINETYPE LINETYPE, std::vector<std::string> temp, int _newLine)
{
	if(LINETYPE == M22Script::LINETYPE::NEW_BACKGROUND)
	{
		for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
		{
			std::string tempPath = "graphics/backgrounds/";
			tempPath += temp[1];
			tempPath += ".webp";
			if(tempPath == M22Graphics::backgroundIndex[i])
			{
				M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
				SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS[0].sprite, 255);
				SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS[1].sprite, 255);
				SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
				SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
				M22Graphics::UpdateBackgroundRenderTarget();
				SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
				SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
				//M22Script::ChangeLine(++_newLine);
				return 0;
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
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::STOP_MUSIC)
	{
		M22Sound::ChangeMusicTrack("sfx/music/SILENCE.OGG");
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::FADE_TO_BLACK)
	{
		M22Script::FadeToBlack();
		//M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::PLAY_STING)
	{
		std::string tempPath = "sfx/stings/";
		tempPath += temp[1];
		tempPath += ".OGG";
		M22Sound::PlaySting(tempPath, true);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::DRAW_CHARACTER)
	{
		int charIndex;
		charIndex = M22Engine::GetCharacterIndexFromName(temp[1]);
		M22Graphics::AddCharacterToBackgroundRenderTarget(charIndex, M22Engine::GetOutfitIndexFromName(temp[2], charIndex), M22Engine::GetEmotionIndexFromName(temp[3], charIndex), atoi(temp[4].c_str()), false);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::DRAW_CHARACTER_BRUTAL)
	{
		int charIndex;
		charIndex = M22Engine::GetCharacterIndexFromName(temp[1]);
		M22Graphics::AddCharacterToBackgroundRenderTarget(charIndex, M22Engine::GetOutfitIndexFromName(temp[2], charIndex), M22Engine::GetEmotionIndexFromName(temp[3], charIndex), atoi(temp[4].c_str()), true);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::CLEAR_CHARACTERS)
	{
		M22Script::ClearCharacters();
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::WAIT)
	{
		M22Script::currentLine = "";
		M22Engine::TIMER_CURR = 0;
		M22Engine::TIMER_TARGET = atoi(temp[1].c_str());
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::LOAD_SCRIPT)
	{
		M22Script::currentScript.clear();
		M22Script::ClearCharacters();
		LoadScriptToCurrent(temp[1].c_str());
		M22Script::ChangeLine(0);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::DARK_SCREEN)
	{
		SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, M22Script::DARKEN_SCREEN_OPACITY );
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::BRIGHT_SCREEN)
	{
		SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::PLAY_STING_LOOPED)
	{
		std::string tempPath = "sfx/stings/";
		tempPath += temp[1];
		tempPath += ".OGG";
		M22Sound::PlayLoopedSting(tempPath);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::STOP_STING_LOOPED)
	{
		M22Sound::StopLoopedStings();
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::GOTO)
	{
		int newLinePosition = atoi(temp[1].c_str());
		M22Script::ChangeLine(newLinePosition);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::EXITGAME)
	{
		M22Engine::QUIT = true;
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::EXITTOMAINMENU)
	{
		M22Engine::ResetGame();
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::IF_STATEMENT)
	{
		// m22IF TEST_DECISION YES Goto 500
		// param 1 = which decision
		// param 2 = which choice of decision
		// param 3 = what to do if true

		// temp[0] == m22IF
		// temp[1] == TEST_DECISION
		// temp[2] == YES

		short int specifiedDecision = -1;
		short int specifiedChoice = -1;

		for(size_t i = 0; i < gameDecisions.size(); i++)
		{
			if(gameDecisions.at(i).name == temp.at(1))
			{
				// found decision
				specifiedDecision = i;
				break;
			};
		};

		if(specifiedDecision == -1)
		{
			// INVALID DECISION
			M22Script::ChangeLine(++_newLine);
			return 0;
		};

		for(size_t i = 0; i < gameDecisions.at(specifiedDecision).choices.size(); i++)
		{
			if(gameDecisions.at(specifiedDecision).choices.at(i) == temp.at(2))
			{
				specifiedChoice = i;
			};
		};

		if(specifiedChoice == -1)
		{
			// INVALID CHOICE
			M22Script::ChangeLine(++_newLine);
			return 0;
		};

		if(gameDecisions.at(specifiedDecision).selectedOption == specifiedChoice)
		{
			//IF STATEMENT RETURNS TRUE
			M22Script::LINETYPE tempType = M22Script::CheckLineType(temp.at(3));
			std::vector<std::string> tempStrVec;
			for(size_t i = 3; i < temp.size(); i++)
			{
				tempStrVec.push_back(temp.at(i));
			};
			M22Script::ExecuteM22ScriptCommand(tempType,tempStrVec,_newLine);
			return 0;
		}
		else
		{
			// RETURN FALSE
			M22Script::ChangeLine(++_newLine);
		};
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::COMMENT)
	{
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::SET_ACTIVE_TRANSITION)
	{
		for(size_t i = 0; i < M22Graphics::TRANSITIONS::NUMBER_OF_TRANSITIONS; i++)
		{
			if(temp[1] == M22Graphics::TRANSITION_NAMES[i])
			{
				M22Graphics::activeTransition = i;
				break;
			};
		};
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::CLEAR_CHARACTERS_BRUTAL)
	{
		ClearCharacters();
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::FADE_TO_BLACK_FANCY)
	{
		//if(!M22Engine::skipping)
		//{
			M22Graphics::FadeToBlackFancy();
		//}
		//else
		//{
		//	M22Script::FadeToBlack();
		//};
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else
	{
		M22Script::activeSpeakerIndex = M22Engine::GetCharacterIndexFromName(temp[0], true);
		if(M22Script::activeSpeakerIndex > 0)
		{
			std::string tempName = temp.at(0);
			M22Script::currentLine.erase(0, tempName.length());
		};
	};
	return 1;
};

bool M22Script::isColon(int _char)
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

void M22Script::FadeToBlack(void)
{
	for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
	{
		if(M22Graphics::backgroundIndex[i] == "graphics/backgrounds/BLACK.webp")
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
			return;
		};
	};
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
	else if(_input == std::string("DrawCharacterBrutal"))
	{
		return M22Script::LINETYPE::DRAW_CHARACTER_BRUTAL;
	}
	else if(_input == std::string("ClearCharacters"))
	{
		return M22Script::LINETYPE::CLEAR_CHARACTERS;
	}
	else if(_input == std::string("ClearCharactersBrutal"))
	{
		return M22Script::LINETYPE::CLEAR_CHARACTERS_BRUTAL;
	}
	else if(_input == std::string("LoadScript"))
	{
		return M22Script::LINETYPE::LOAD_SCRIPT;
	}
	else if(_input == std::string("FadeToBlack"))
	{
		return M22Script::LINETYPE::FADE_TO_BLACK;
	}
	else if(_input == std::string("FadeToBlackFancy"))
	{
		return M22Script::LINETYPE::FADE_TO_BLACK_FANCY;
	}
	else if(_input == std::string("StopMusic"))
	{
		return M22Script::LINETYPE::STOP_MUSIC;
	}
	else if(_input == std::string("DarkenScreen"))
	{
		return M22Script::LINETYPE::DARK_SCREEN;
	}
	else if(_input == std::string("BrightenScreen"))
	{
		return M22Script::LINETYPE::BRIGHT_SCREEN;
	}
	else if(_input == std::string("Goto"))
	{
		return M22Script::LINETYPE::GOTO;
	}
	else if(_input == std::string("PlayLoopedSting"))
	{
		return M22Script::LINETYPE::PLAY_STING_LOOPED;
	}
	else if(_input == std::string("StopLoopedStings"))
	{
		return M22Script::LINETYPE::STOP_STING_LOOPED;
	}
	else if(_input == std::string("//"))
	{
		return M22Script::LINETYPE::COMMENT;
	}
	else if(_input == std::string("ExitGame"))
	{
		return M22Script::LINETYPE::EXITGAME;
	}
	else if(_input == std::string("MainMenu"))
	{
		return M22Script::LINETYPE::EXITTOMAINMENU;
	}
	else if(_input == std::string("SetActiveTransition"))
	{
		return M22Script::LINETYPE::SET_ACTIVE_TRANSITION;
	}
	else if(_input == std::string("Wait"))
	{
		return M22Script::LINETYPE::WAIT;
	}
	else if(_input == std::string("m22IF"))
	{
		return M22Script::LINETYPE::IF_STATEMENT;
	};
	return M22Script::LINETYPE::SPEECH;
};