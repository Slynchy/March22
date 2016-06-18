#include "../M22Engine.h"

std::string M22Script::currentLine;
std::wstring M22Script::currentLine_w;
int M22Script::currentLineIndex = NULL;
std::vector<std::string> M22Script::currentScript;
std::vector<std::wstring> M22Script::currentScript_w;
int M22Script::activeSpeakerIndex = 1;
SDL_Surface* M22Script::currentLineSurface = NULL;
SDL_Surface* M22Script::currentLineSurfaceShadow = NULL;
SDL_Texture* M22Script::currentLineTexture = NULL;
SDL_Texture* M22Script::currentLineTextureShadow = NULL;
float M22Script::fontSize;
std::vector<M22Script::Decision> M22Script::gameDecisions;
M22Script::LINETYPE M22Script::currentLineType;
std::string M22Script::currentScriptFileName;
bool M22Script::updateCurrentLine = false;
SDL_Rect M22Script::currentLineTextureShadowRect = {8+1,404+1,0,0};
SDL_Rect M22Script::currentLineTextureRect = {8,404,0,0};

short int M22Script::LoadTextBoxPosition(const char* _filename)
{
	printf("[M22Script] Loading \"%s\" \n", _filename);
	std::ifstream input(_filename);
	if(input)
	{
		input >> currentLineTextureRect.x;
		input >> currentLineTextureRect.y;
		currentLineTextureShadowRect.x = (currentLineTextureRect.x + 1);
		currentLineTextureShadowRect.y = (currentLineTextureRect.y + 1);
	}
	else
	{
		printf("Failed to load decisions file: %s \n", _filename);
		return -1;
	};
	input.close();
	return 0;
};

short int M22Script::LoadGameDecisions(const char* _filename)
{
	printf("[M22Script] Loading \"%s\" \n", _filename);
	std::wifstream input(_filename);
	int length;
	std::wstring temp;
	std::vector<std::wstring> tempArr;
	if(input)
	{
		getline(input,temp);
		length = atoi(M22Script::to_string(temp).c_str());
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
			M22Script::gameDecisions.at(i).num_of_choices = atoi(M22Script::to_string(tempArr.at(1)).c_str());

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
	std::string temp;
	if(input)
	{
		M22Script::currentScriptFileName = filename;
		M22Script::currentScript.clear();
		while (std::getline(input , temp)) 
		{
			if(temp.length() >= 2)
			{
				if(temp.at(0) == '/' && temp.at(1) == '/')
				{
					// it's a comment! Store only two chars to save space
					temp = "//";
				};
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

short int M22Script::LoadScriptToCurrent_w(const char* _filename)
{
	std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>) ;
	std::string filename = "scripts/";
	filename += _filename;
	printf("[M22Script] Loading \"%s\" \n", filename.c_str());
	std::wifstream input(filename);
	input.imbue(ulocale);
	std::wstring temp;
	if(input)
	{
		M22Script::currentScriptFileName = filename;
		M22Script::currentScript_w.clear();
		while (std::getline(input , temp)) 
		{
			if(temp.length() >= 2)
			{
				if(temp.at(0) == '/' && temp.at(1) == '/')
				{
					// it's a comment! Store only two chars to save space
					temp = M22Script::to_wstring("//");
				};
			};
			M22Script::currentScript_w.push_back(temp);
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

void M22Script::DrawDecisions(M22Script::Decision* _decision,int ScrW, int ScrH)
{
	SDL_Color tempCol = {255,255,255,255};
	SDL_Color tempCol2 = {0,0,0,255};
	std::wstring choiceTextTemp;
	for(int i = 0; i < _decision->num_of_choices; i++)
	{
		choiceTextTemp += (_decision->choices.at(i) + to_wstring("\n"));
	};
	std::replace( choiceTextTemp.begin(), choiceTextTemp.end(), '_', ' ');

	SDL_Surface* tempSurfShadow;
	SDL_Surface* tempSurf;

	Uint16* stext = new Uint16[choiceTextTemp.length()+1];
	for (size_t i = 0; i < choiceTextTemp.length(); ++i) {
		stext[i] = choiceTextTemp.at(i); 
	}
	stext[choiceTextTemp.length()] = '\0';

	//tempSurfShadow =	TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, choiceTextTemp.c_str(), tempCol2,	ScrW-12 );
	//tempSurf =			TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, choiceTextTemp.c_str(), tempCol,		ScrW-12 );
	tempSurfShadow =	TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, stext, tempCol2,	ScrW-12 );
	tempSurf =			TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, stext, tempCol,	ScrW-12 );

	if(tempSurfShadow == NULL)
	{
		printf("Failed to draw message shadow!\n");
		return;
	};
	if(tempSurf == NULL)
	{
		printf("Failed to draw message!\n");
		return;
	};

	SDL_Texture* temp2 =	SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, tempSurfShadow);
	SDL_Texture* temp =		SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, tempSurf);
	
	SDL_Rect tempDst2 = {8+1,404+1,0,0};
	SDL_Rect tempDst = {8,404,0,0};
		
	SDL_QueryTexture(temp2, NULL, NULL, &tempDst2.w, &tempDst2.h);
	SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
		
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, temp2, NULL, &tempDst2);
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, temp, NULL, &tempDst);
	

	SDL_DestroyTexture(temp2);
	SDL_DestroyTexture(temp);
	SDL_FreeSurface(tempSurfShadow);
	SDL_FreeSurface(tempSurf);
	//delete tempSurf;

	return;
};

/*
	An extremely slow function (comparitively) for drawing decent-looking text.
	Draws text then a "shadow" 1px adjacent @ 640x480
*/
void M22Script::DrawCurrentLine(int ScrW, int ScrH)
{
	if(M22Script::updateCurrentLine == true)
	{
		SDL_Color tempCol = {255,255,255,255};
		SDL_Color tempCol2 = {0,0,0,255};

		Uint16* stext = new Uint16[M22Script::currentLine_w.length()+1];
		for (size_t i = 0; i < M22Script::currentLine_w.length(); ++i) {
			stext[i] = M22Script::currentLine_w[i]; 
		}
		stext[M22Script::currentLine_w.length()] = '\0';

		//M22Script::currentLineSurfaceShadow =	TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol2,	ScrW-12 );
		//M22Script::currentLineSurface =			TTF_RenderText_Blended_Wrapped( M22Graphics::textFont, M22Script::currentLine.c_str(), tempCol,		ScrW-12 );
		M22Script::currentLineSurfaceShadow =	TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, stext, tempCol2,	ScrW-12 );
		M22Script::currentLineSurface =			TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, stext, tempCol,	ScrW-12 );

		delete stext;
	
	
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
		
		SDL_DestroyTexture(M22Script::currentLineTextureShadow);
		SDL_DestroyTexture(M22Script::currentLineTexture);
		M22Script::currentLineTextureShadow =	SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, M22Script::currentLineSurfaceShadow);
		M22Script::currentLineTexture =		SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, M22Script::currentLineSurface);
		
		SDL_QueryTexture(currentLineTextureShadow, NULL, NULL, &M22Script::currentLineTextureShadowRect.w, &M22Script::currentLineTextureShadowRect.h);
		SDL_QueryTexture(currentLineTexture, NULL, NULL, &M22Script::currentLineTextureRect.w, &M22Script::currentLineTextureRect.h);
		M22Script::updateCurrentLine = false;
	};
	
//	SDL_Rect tempDst2= {(int)(ScrW*0.01875)+int(1.0f*M22Script::fontSize),(int)(ScrH*0.8416666666666667f)+int(1.0f*M22Script::fontSize),0,0};
//	SDL_Rect tempDst = {(int)(ScrW*0.01875),(int)(ScrH*0.8416666666666667f),0,0};
	
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Script::currentLineTextureShadow, NULL, &M22Script::currentLineTextureShadowRect);
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Script::currentLineTexture, NULL, &M22Script::currentLineTextureRect);

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

unsigned int M22Script::SplitString(const std::wstring &txt, std::vector<std::wstring> &strs, char ch)
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
	SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, M22Graphics::NEXT_BACKGROUND_RENDER_TARGET);
	SDL_SetRenderDrawColor(M22Renderer::SDL_RENDERER, 0,0,0,0);
	SDL_RenderClear(M22Renderer::SDL_RENDERER);
	
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Engine::ACTIVE_BACKGROUNDS[0].sprite, NULL, NULL);
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );

	SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, NULL);
	SDL_SetRenderDrawColor(M22Renderer::SDL_RENDERER, 255,255,255,255);

	M22Graphics::changeQueued = M22Graphics::BACKGROUND_UPDATE_TYPES::CHARACTER;

	return;
};

void M22Script::ChangeLine(int _newLine)
{
	// Check the line even exists
	if(size_t(_newLine) >= M22Script::currentScript.size())
	{
		printf("Cannot change line! Out of bounds!\n");
		return;
	};

	// Update currentLine variable
	M22Script::currentLine = M22Script::currentScript[_newLine];
	M22Script::currentLine_w = M22Script::currentScript_w[_newLine];

	// Check if blank
	if(M22Script::currentLine == "")
	{
		M22Script::ChangeLine(++_newLine);
		return;
	};
	M22Script::updateCurrentLine = true;

	// Get type of line
	std::vector<std::wstring> temp;
	M22Script::SplitString(M22Script::currentLine_w, temp, ' ');
	std::wstring type = temp.at(0);
	M22Script::LINETYPE LINETYPE = M22Script::CheckLineType(type);
	M22Script::currentLineType = LINETYPE;

	//if(M22Script::ExecuteM22ScriptCommand(LINETYPE, temp, _newLine) == 1)
	//	return;

	M22Script::currentLineIndex = _newLine;
	M22Script::ExecuteM22ScriptCommand(LINETYPE, temp, _newLine);
	return;
};

short int M22Script::ExecuteM22ScriptCommand(M22Script::LINETYPE LINETYPE, std::vector<std::wstring> temp, int _newLine)
{
	if(LINETYPE == M22Script::LINETYPE::NEW_BACKGROUND)
	{
		for(size_t i = 0; i < M22Graphics::backgroundIndex.size(); i++)
		{
			std::string tempPath = "graphics/backgrounds/";
			tempPath += M22Script::to_string(temp.at(1));
			tempPath += ".webp";
			if(tempPath == M22Graphics::backgroundIndex[i])
			{
				M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
				M22Engine::ACTIVE_BACKGROUNDS[0].name = tempPath;
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
		tempPath += M22Script::to_string(temp.at(1));
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
		tempPath += M22Script::to_string(temp[1]);
		tempPath += ".OGG";
		M22Sound::PlaySting(tempPath, true);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::DRAW_CHARACTER)
	{
		int charIndex;
		charIndex = M22Engine::GetCharacterIndexFromName(temp.at(1));
		M22Graphics::AddCharacterToBackgroundRenderTarget(charIndex, M22Engine::GetOutfitIndexFromName(M22Script::to_string(temp.at(2)), charIndex), M22Engine::GetEmotionIndexFromName(M22Script::to_string(temp.at(3)), charIndex), atoi(M22Script::to_string(temp.at(4)).c_str()), false);
		M22Script::ChangeLine(++_newLine);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::DRAW_CHARACTER_BRUTAL)
	{
		int charIndex;
		charIndex = M22Engine::GetCharacterIndexFromName(temp.at(1));
		M22Graphics::AddCharacterToBackgroundRenderTarget(charIndex, M22Engine::GetOutfitIndexFromName(M22Script::to_string(temp.at(2)), charIndex), M22Engine::GetEmotionIndexFromName(M22Script::to_string(temp.at(3)), charIndex), atoi(M22Script::to_string(temp.at(4)).c_str()), true);
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
		M22Engine::TIMER_TARGET = atoi(M22Script::to_string(temp.at(1)).c_str());
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::LOAD_SCRIPT)
	{
		M22Script::currentScript.clear();
		M22Script::ClearCharacters();
		LoadScriptToCurrent(M22Script::to_string(temp.at(1)).c_str());
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
		tempPath += M22Script::to_string(temp.at(1));
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
		int newLinePosition = atoi(M22Script::to_string(temp.at(1)).c_str());
		M22Script::ChangeLine(newLinePosition);
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::EXITGAME)
	{
		M22Engine::QUIT = true;
		return 0;
	}
	else if(LINETYPE == M22Script::LINETYPE::SET_DECISION)
	{

		//Wipe spaces
		for(size_t i = 0; i < temp.size(); i++)
		{
			temp[i].erase(
				std::remove_if(
					temp[i].begin(), 
					temp[i].end(), 
					isspace
				)
			);
		};

		//Check decision doesn't already exist.
		bool exitflag = false;
		for(size_t i = 0; i < M22Script::gameDecisions.size(); i++)
		{
			// if decision is found
			if(M22Script::gameDecisions.at(i).name == temp.at(1))
			{
				for(size_t k = 0; k < M22Script::gameDecisions.at(i).choices.size(); k++)
				{
					// found choice
					if(M22Script::gameDecisions.at(i).choices.at(k) == temp.at(2))
					{
						exitflag = true;
						M22Script::gameDecisions.at(i).selectedOption = k;
					};
					if(exitflag == true) break;
				};
			};
			if(exitflag == true) break;
		};
		if(exitflag == false) printf("[M22Script] Decision/choice not found @ %s line %i!\n", M22Script::currentScriptFileName, M22Script::currentLineIndex);

		M22Script::ChangeLine(++_newLine);
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

		for(size_t i = 0; i < temp.size(); i++)
		{
			temp[i].erase(
				std::remove_if(
					temp[i].begin(), 
					temp[i].end(), 
					isspace
				)
			);
		};

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
			std::vector<std::wstring> tempStrVec;
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
	else if(LINETYPE == M22Script::LINETYPE::MAKE_DECISION)
	{
		M22Script::activeSpeakerIndex = 0;
		M22Engine::skipping = false;
		for(size_t i = 0; i < temp.size(); i++)
		{
			temp[i].erase(
				std::remove_if(
					temp[i].begin(), 
					temp[i].end(), 
					isspace
				)
			);
		};

		//Check decision doesn't already exist.
		for(size_t i = 0; i < M22Script::gameDecisions.size(); i++)
		{
			if(M22Script::gameDecisions.at(i).name == temp.at(1))
			{
				M22Engine::moveVectorItemToBack(M22Script::gameDecisions, i);
				return 0;
			};
		};

		M22Script::Decision tempDecision;
		tempDecision.name = temp.at(1);
		for(size_t i = 2; i < temp.size(); i++)
		{
			tempDecision.choices.push_back(temp.at(i));
			tempDecision.num_of_choices++;
		};

		M22Script::gameDecisions.push_back(tempDecision);

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
			if(temp.at(1) == M22Graphics::TRANSITION_NAMES[i])
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
		M22Script::activeSpeakerIndex = M22Engine::GetCharacterIndexFromName(temp.at(0), true);
		if(M22Script::activeSpeakerIndex > 0)
		{
			std::wstring tempName = temp.at(0);
			M22Script::currentLine.erase(0, tempName.length());
			M22Script::currentLine_w.erase(0, tempName.length());
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
				M22Engine::ACTIVE_BACKGROUNDS[1].name = "graphics/backgrounds/BLACK.webp";
				SDL_SetTextureBlendMode(M22Engine::ACTIVE_BACKGROUNDS[1].sprite, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod( M22Engine::ACTIVE_BACKGROUNDS[1].sprite, 0 );
			}
			else
			{
				M22Engine::ACTIVE_BACKGROUNDS[0].sprite = M22Graphics::BACKGROUNDS[i];
				M22Engine::ACTIVE_BACKGROUNDS[0].name = "graphics/backgrounds/BLACK.webp";
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
M22Script::LINETYPE M22Script::CheckLineType(std::wstring _input)
{
	std::string temp = M22Script::to_string(_input);
	temp.erase(std::remove_if(temp.begin(), temp.end(), isspace));
	_input = M22Script::to_wstring(temp.c_str());
	if(_input == M22Script::to_wstring("DrawBackground"))
	{
		return M22Script::LINETYPE::NEW_BACKGROUND;
	}
	else if(_input == M22Script::to_wstring("PlayMusic"))
	{
		return M22Script::LINETYPE::NEW_MUSIC;
	}
	else if(_input == M22Script::to_wstring("PlaySting"))
	{
		return M22Script::LINETYPE::PLAY_STING;
	}
	else if(_input == M22Script::to_wstring("DrawCharacter"))
	{
		return M22Script::LINETYPE::DRAW_CHARACTER;
	}
	else if(_input == M22Script::to_wstring("DrawCharacterBrutal"))
	{
		return M22Script::LINETYPE::DRAW_CHARACTER_BRUTAL;
	}
	else if(_input == M22Script::to_wstring("ClearCharacters"))
	{
		return M22Script::LINETYPE::CLEAR_CHARACTERS;
	}
	else if(_input == M22Script::to_wstring("ClearCharactersBrutal"))
	{
		return M22Script::LINETYPE::CLEAR_CHARACTERS_BRUTAL;
	}
	else if(_input == M22Script::to_wstring("LoadScript"))
	{
		return M22Script::LINETYPE::LOAD_SCRIPT;
	}
	else if(_input == M22Script::to_wstring("FadeToBlack"))
	{
		return M22Script::LINETYPE::FADE_TO_BLACK;
	}
	else if(_input == M22Script::to_wstring("FadeToBlackFancy"))
	{
		return M22Script::LINETYPE::FADE_TO_BLACK_FANCY;
	}
	else if(_input == M22Script::to_wstring("StopMusic"))
	{
		return M22Script::LINETYPE::STOP_MUSIC;
	}
	else if(_input == M22Script::to_wstring("DarkenScreen"))
	{
		return M22Script::LINETYPE::DARK_SCREEN;
	}
	else if(_input == M22Script::to_wstring("BrightenScreen"))
	{
		return M22Script::LINETYPE::BRIGHT_SCREEN;
	}
	else if(_input == M22Script::to_wstring("Goto"))
	{
		return M22Script::LINETYPE::GOTO;
	}
	else if(_input == M22Script::to_wstring("PlayLoopedSting"))
	{
		return M22Script::LINETYPE::PLAY_STING_LOOPED;
	}
	else if(_input == M22Script::to_wstring("StopLoopedStings"))
	{
		return M22Script::LINETYPE::STOP_STING_LOOPED;
	}
	else if(_input == M22Script::to_wstring("//"))
	{
		return M22Script::LINETYPE::COMMENT;
	}
	else if(_input == M22Script::to_wstring("ExitGame"))
	{
		return M22Script::LINETYPE::EXITGAME;
	}
	else if(_input == M22Script::to_wstring("MainMenu"))
	{
		return M22Script::LINETYPE::EXITTOMAINMENU;
	}
	else if(_input == M22Script::to_wstring("SetActiveTransition"))
	{
		return M22Script::LINETYPE::SET_ACTIVE_TRANSITION;
	}
	else if(_input == M22Script::to_wstring("Wait"))
	{
		return M22Script::LINETYPE::WAIT;
	}
	else if(_input == M22Script::to_wstring("m22IF"))
	{
		return M22Script::LINETYPE::IF_STATEMENT;
	}
	else if(_input == M22Script::to_wstring("SetDecision"))
	{
		return M22Script::LINETYPE::SET_DECISION;
	}
	else if(_input == M22Script::to_wstring("MakeDecision"))
	{
		return M22Script::LINETYPE::MAKE_DECISION;
	};
	return M22Script::LINETYPE::SPEECH;
};