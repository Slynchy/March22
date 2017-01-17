#include <engine/M22Engine.h>

using namespace March22;

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
std::wstring M22Script::typewriter_text;
size_t M22Script::typewriter_currPos;
NFont* M22Script::font;

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
	
	Uint16* UNICODETEXT = M22Script::to_Uint16(choiceTextTemp);
	tempSurfShadow =	TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, UNICODETEXT, tempCol2,	ScrW-12 );
	tempSurf =			TTF_RenderUNICODE_Blended_Wrapped( M22Graphics::textFont, UNICODETEXT, tempCol,	ScrW-12 );
	delete [] UNICODETEXT;

	if(tempSurfShadow == NULL)
	{
		printf("[M22Script] Failed to draw message shadow!\n");
		return;
	};
	if(tempSurf == NULL)
	{
		printf("[M22Script] Failed to draw message!\n");
		return;
	};

	SDL_Texture* temp2 =	SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, tempSurfShadow);
	SDL_Texture* temp =		SDL_CreateTextureFromSurface(M22Renderer::SDL_RENDERER, tempSurf);
	
	SDL_Rect tempDst2 = {8+1,404+1,0,0};
	SDL_Rect tempDst = {8,404,0,0};
		
	//SDL_QueryTexture(temp2, NULL, NULL, &tempDst2.w, &tempDst2.h);
	//SDL_QueryTexture(temp, NULL, NULL, &tempDst.w, &tempDst.h);
		
	SDL_QueryTexture(temp2, NULL, NULL, &M22Script::currentLineTextureShadowRect.w, &M22Script::currentLineTextureShadowRect.h);
	SDL_QueryTexture(temp, NULL, NULL, &M22Script::currentLineTextureRect.w, &M22Script::currentLineTextureRect.h);
		
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, temp2, NULL, &currentLineTextureShadowRect);
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, temp, NULL, &currentLineTextureRect);
	

	SDL_DestroyTexture(temp2);
	SDL_DestroyTexture(temp);
	SDL_FreeSurface(tempSurfShadow);
	SDL_FreeSurface(tempSurf);
	//delete tempSurf;

	return;
};

std::string wstring_to_utf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

/*
	An extremely slow function (comparitively) for drawing decent-looking text.
	Draws text then a "shadow" 1px adjacent @ 640x480
*/
void M22Script::DrawCurrentLine(int ScrW, int ScrH)
{
	bool finished = false;
	if(M22Script::updateCurrentLine == true)
	{
		SDL_Color tempCol2 = {0,0,0,255};       // Black
		SDL_Color tempCol = {255,255,255,255};  // White

		try
		{
			//if (M22Script::currentLine_w.at(typewriter_currPos) == 0x2019)
			//{
			//	M22Script::typewriter_text += M22Script::currentLine_w.at(typewriter_currPos++);
			//}
			M22Script::typewriter_text += M22Script::currentLine_w.at(typewriter_currPos++);
		}
		catch (std::exception& e)
		{
			M22Script::updateCurrentLine = false;
			M22Script::typewriter_currPos = 0;
			finished = true;
		}
	};

	(*font).drawColumn(M22Renderer::SDL_RENDERER, 55 + 2, 50 + 2, ScrW - 90, NFont::Color(0, 0, 0, 255), wstring_to_utf8(M22Script::typewriter_text).c_str());
	(*font).drawColumn(M22Renderer::SDL_RENDERER, 55, 50, ScrW - 90, wstring_to_utf8(M22Script::typewriter_text).c_str());


	if(finished==true)
		if (M22Script::currentLineType == March22::M22Script::LINETYPE::NARRATIVE ||
			M22Script::currentLineType == March22::M22Script::LINETYPE::SPEECH)
				typewriter_text += L"\n\n";

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
	
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, M22Engine::ACTIVE_BACKGROUNDS.at(0).sprite, NULL, NULL);
	SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );

	SDL_SetRenderTarget(M22Renderer::SDL_RENDERER, NULL);
	SDL_SetRenderDrawColor(M22Renderer::SDL_RENDERER, 255,255,255,255);

	M22Graphics::changeQueued = M22Graphics::BACKGROUND_UPDATE_TYPES::CHARACTER;

	return;
};

void M22Script::ChangeLine(int _newLine)
{
	if((size_t)_newLine > M22ScriptCompiler::currentScript_c.size())
	{
		printf("[M22Script] Out of bounds line change!\n");
		return;
	};

	// Update currentLine variable
	M22Script::currentLine = M22Script::to_string(M22ScriptCompiler::currentScript_c.at(_newLine).m_lineContents);
	M22Script::currentLine_w = M22ScriptCompiler::currentScript_c.at(_newLine).m_lineContents;

	M22Script::updateCurrentLine = true;

	M22ScriptCompiler::CURRENT_LINE = &M22ScriptCompiler::currentScript_c.at(_newLine);
	M22Script::currentLineType = M22ScriptCompiler::CURRENT_LINE->m_lineType;

	M22Script::currentLineIndex = _newLine;
	M22ScriptCompiler::ExecuteCommand(M22ScriptCompiler::currentScript_c.at(M22Script::currentLineIndex), M22Script::currentLineIndex);
	return;
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
	else if (_input == M22Script::to_wstring("DrawBackgroundStealth"))
	{
		return M22Script::LINETYPE::NEW_BACKGROUND_STEALTH;
	}
	else if (_input == M22Script::to_wstring("//"))
	{
		return M22Script::LINETYPE::COMMENT;
	}
	else if (_input == M22Script::to_wstring("DrawSprite"))
	{
		return M22Script::LINETYPE::DRAW_SPRITE;
	}
	else if (_input == M22Script::to_wstring("DrawAnimSprite"))
	{
		return M22Script::LINETYPE::DRAW_SPRITE_ANIMATED;
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
	else if(_input == M22Script::to_wstring("LoadScriptGoto"))
	{
		return M22Script::LINETYPE::LOAD_SCRIPT_GOTO;
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
	else if(_input == M22Script::to_wstring("Goto_debug"))
	{
		return M22Script::LINETYPE::GOTO_DEBUG;
	}
	else if(_input == M22Script::to_wstring("PlayLoopedSting"))
	{
		return M22Script::LINETYPE::PLAY_STING_LOOPED;
	}
	else if(_input == M22Script::to_wstring("StopLoopedStings"))
	{
		return M22Script::LINETYPE::STOP_STING_LOOPED;
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
	else if (_input == M22Script::to_wstring("ClearSprites"))
	{
		return M22Script::LINETYPE::CLEAR_SPRITES;
	}
	else if(_input == M22Script::to_wstring("RunLuaScript"))
	{
		return M22Script::LINETYPE::RUN_LUA_SCRIPT;
	}
	else if(_input == M22Script::to_wstring("MakeDecision"))
	{
		return M22Script::LINETYPE::MAKE_DECISION;
	}
	else if (_input == M22Script::to_wstring("NewPage"))
	{
		return M22Script::LINETYPE::NEW_PAGE;
	};
	return M22Script::LINETYPE::SPEECH;
};