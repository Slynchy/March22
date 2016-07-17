#include <engine/M22Engine.h>

std::vector<M22ScriptCompiler::line_c> M22ScriptCompiler::currentScript_c;
M22ScriptCompiler::line_c* M22ScriptCompiler::CURRENT_LINE;

int M22ScriptCompiler::CompileLoadScriptFile(std::string _filename)
{
	std::locale ulocale(std::locale(), new std::codecvt_utf8<wchar_t>) ;
	std::string filename = "scripts/";
	filename += _filename;
	printf("[M22ScriptCompiler] Loading \"%s\" \n", filename.c_str());
	std::wifstream input(filename);
	input.imbue(ulocale);
	std::wstring temp;
	std::vector<std::wstring> scriptLines;
	std::vector<std::wstring> CURRENT_LINE_SPLIT;

	if(input)
	{
		printf("[M22ScriptCompiler] Compiling \"%s\" \n", filename.c_str());
		M22Script::currentScriptFileName = filename;

		while (std::getline(input , temp)) 
		{
			if(temp.length() >= 2)
			{
				if(temp.at(0) == '/' && temp.at(1) == '/')
				{
					// it's a comment! Deleeeete!
					temp = M22Script::to_wstring("");
				};
			};
			if(temp != M22Script::to_wstring("")) scriptLines.push_back(temp);
		};
	}
	else
	{
		printf("[M22ScriptCompiler] Failed to load script file: %s \n", filename.c_str());
		return -1;
	};
	input.close();

	for(size_t i = 0; i < scriptLines.size(); i++)
	{
		M22Script::SplitString(scriptLines.at(i), CURRENT_LINE_SPLIT, ' ');
		M22ScriptCompiler::line_c tempLine_c;
		tempLine_c.m_lineType = M22Script::CheckLineType(CURRENT_LINE_SPLIT.at(0));
		if(tempLine_c.m_lineType == M22Script::SPEECH)
		{
			tempLine_c.m_lineContents = scriptLines.at(i);
			tempLine_c.m_speaker = M22Engine::GetCharacterIndexFromName(CURRENT_LINE_SPLIT.at(0), true);
			if(tempLine_c.m_speaker > 0)
			{
				tempLine_c.m_lineContents.erase(0, CURRENT_LINE_SPLIT.at(0).length());
			};
		}
		else
		{
			switch(tempLine_c.m_lineType)
			{
				case M22Script::DRAW_CHARACTER:
					tempLine_c.m_parameters.push_back(
						M22Engine::GetOutfitIndexFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)), 
							tempLine_c.m_speaker)
						);
					tempLine_c.m_parameters.push_back(
						M22Engine::GetEmotionIndexFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(3)), 
							tempLine_c.m_speaker)
						);
					tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(4)).c_str()));
					break;
				case M22Script::GOTO:
				case M22Script::WAIT:
					tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)).c_str()));
					break;
				case M22Script::NEW_BACKGROUND:
					tempLine_c.m_parameters.push_back(M22Engine::GetBackgroundIDFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));
					break;
				case M22Script::NEW_MUSIC:
					tempLine_c.m_parameters.push_back(M22Sound::FindMusicFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));
					break;
				case M22Script::PLAY_STING:
				case M22Script::PLAY_STING_LOOPED:
					tempLine_c.m_parameters.push_back(M22Sound::FindStingFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));
					break;
				case M22Script::SET_ACTIVE_TRANSITION:
					for(size_t i = 0; i < M22Graphics::TRANSITIONS::NUMBER_OF_TRANSITIONS; i++)
					{
						if(CURRENT_LINE_SPLIT.at(1) == M22Graphics::TRANSITION_NAMES[i])
						{
							tempLine_c.m_parameters.push_back(i);
							break;
						};
					};
					break;
				case M22Script::RUN_LUA_SCRIPT:
				case M22Script::LOAD_SCRIPT:
					tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)));
					break;
				case M22Script::IF_STATEMENT:
				case M22Script::MAKE_DECISION:
					for(size_t k = 1; k < CURRENT_LINE_SPLIT.size(); k++)
					{
						tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(k)));
					};
					break;
			};
		};
		M22ScriptCompiler::currentScript_c.push_back(tempLine_c);
	};

	return 0;
};

int M22ScriptCompiler::RunLine(int _line)
{
	if((size_t)_line > M22ScriptCompiler::currentScript_c.size())
	{
		printf("[M22ScriptCompiler] Out of lines!\n");
		return -2;
	};

	M22ScriptCompiler::line_c* ACTV_LINE = &M22ScriptCompiler::currentScript_c.at(_line);
	std::string tempPath;
	M22Script::Decision tempDecision;
	bool exitflag = false;
	short int specifiedDecision = -1;
	short int specifiedChoice = -1;
	switch(ACTV_LINE->m_lineType)
	{
		case M22Script::NEW_BACKGROUND:
			tempPath = "graphics/backgrounds/";
			tempPath += M22Graphics::backgroundIndex.at(ACTV_LINE->m_parameters.at(0));
			tempPath += ".webp";
			M22Engine::ACTIVE_BACKGROUNDS.at(0).sprite = M22Graphics::BACKGROUNDS.at(ACTV_LINE->m_parameters.at(0));
			M22Engine::ACTIVE_BACKGROUNDS.at(0).name = tempPath;
			SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS.at(0).sprite, 255);
			SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS.at(1).sprite, 255);
			SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
			SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
			M22Graphics::UpdateBackgroundRenderTarget();
			SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
			SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
			return 0;

		case M22Script::NEW_MUSIC:
			M22Sound::ChangeMusicTrack(ACTV_LINE->m_parameters.at(0));
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::STOP_MUSIC:
			M22Sound::StopMusic();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::FADE_TO_BLACK:
			M22Script::FadeToBlack();
			return 0;

		case M22Script::PLAY_STING:
			M22Sound::PlaySting(ACTV_LINE->m_parameters.at(0), true);
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::DRAW_CHARACTER:
			M22Graphics::AddCharacterToBackgroundRenderTarget(ACTV_LINE->m_parameters.at(0),ACTV_LINE->m_parameters.at(1), ACTV_LINE->m_parameters.at(2), ACTV_LINE->m_parameters.at(3), false);
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::DRAW_CHARACTER_BRUTAL:
			M22Graphics::AddCharacterToBackgroundRenderTarget(ACTV_LINE->m_parameters.at(0),ACTV_LINE->m_parameters.at(1), ACTV_LINE->m_parameters.at(2), ACTV_LINE->m_parameters.at(3), true);
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::CLEAR_CHARACTERS:
			M22Script::ClearCharacters();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::WAIT:
			M22Engine::TIMER_CURR = 0;
			M22Engine::TIMER_TARGET = ACTV_LINE->m_parameters.at(0);
			return 0;

		case M22Script::LOAD_SCRIPT:
			M22ScriptCompiler::currentScript_c.clear();
			M22Script::ClearCharacters();
			//M22Script::LoadScriptToCurrent(M22Script::to_string(temp.at(1)).c_str());
			//M22Script::LoadScriptToCurrent_w(M22Script::to_string(temp.at(1)).c_str());
			M22ScriptCompiler::CompileLoadScriptFile(ACTV_LINE->m_parameters_txt.at(0));
			M22Script::ChangeLine(0);
			return 0;

		case M22Script::DARK_SCREEN:
			SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, M22Script::DARKEN_SCREEN_OPACITY );
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::BRIGHT_SCREEN:
			SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::PLAY_STING_LOOPED:
			M22Sound::PlayLoopedSting(ACTV_LINE->m_parameters.at(0));
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::STOP_STING_LOOPED:
			M22Sound::StopLoopedStings();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::GOTO:
			if(ACTV_LINE->m_parameters.at(0) <= (int)M22ScriptCompiler::currentScript_c.size()) 
			{
				M22Script::ChangeLine(ACTV_LINE->m_parameters.at(0));
			}
			else
			{
				printf("[M22ScriptCompiler] Goto error! Line number exceeds size of script!\n");
			};
			return 0;

		case M22Script::EXITGAME:
			M22Engine::QUIT = true;
			return 0;

		case M22Script::SET_DECISION:
			//Wipe spaces
			for(size_t i = 0; i < ACTV_LINE->m_parameters_txt.size(); i++)
			{
				ACTV_LINE->m_parameters_txt.at(i).erase(
					std::remove_if(
						ACTV_LINE->m_parameters_txt.at(i).begin(), 
						ACTV_LINE->m_parameters_txt.at(i).end(), 
						isspace
					)
				);
			};

			//Check decision doesn't already exist.
			for(size_t i = 0; i < M22Script::gameDecisions.size(); i++)
			{
				// if decision is found
				if(M22Script::gameDecisions.at(i).name == M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(0)))
				{
					for(size_t k = 0; k < M22Script::gameDecisions.at(i).choices.size(); k++)
					{
						// found choice
						if(M22Script::gameDecisions.at(i).choices.at(k) == M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(1)))
						{
							exitflag = true;
							M22Script::gameDecisions.at(i).selectedOption = k;
						};
						if(exitflag == true) break;
					};
				};
				if(exitflag == true) break;
			};
			if(exitflag == false) printf("[M22ScriptCompiler] Decision/choice not found @ %s line %i!\n", M22Script::currentScriptFileName, _line);

			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::EXITTOMAINMENU:
			M22Engine::ResetGame();
			return 0;

		case M22Script::IF_STATEMENT:
			for(size_t i = 0; i < ACTV_LINE->m_parameters_txt.size(); i++)
			{
				ACTV_LINE->m_parameters_txt.at(i).erase(
					std::remove_if(
						ACTV_LINE->m_parameters_txt.at(i).begin(), 
						ACTV_LINE->m_parameters_txt.at(i).end(), 
						isspace
					)
				);
			};

			for(size_t i = 0; i < M22Script::gameDecisions.size(); i++)
			{
				if(M22Script::gameDecisions.at(i).name == M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(0)))
				{
					// found decision
					specifiedDecision = i;
					break;
				};
			};

			if(specifiedDecision == -1)
			{
				// INVALID DECISION
				M22Script::ChangeLine(++_line);
				return 0;
			};

			for(size_t i = 0; i < M22Script::gameDecisions.at(specifiedDecision).choices.size(); i++)
			{
				if(M22Script::gameDecisions.at(specifiedDecision).choices.at(i) == M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(1)))
				{
					specifiedChoice = i;
				};
			};

			if(specifiedChoice == -1)
			{
				// INVALID CHOICE
				M22Script::ChangeLine(++_line);
				return 0;
			};

			if(M22Script::gameDecisions.at(specifiedDecision).selectedOption == specifiedChoice)
			{
				//IF STATEMENT RETURNS TRUE
				M22Script::LINETYPE tempType = M22Script::CheckLineType(M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(2)));
				std::vector<std::wstring> tempStrVec;
				for(size_t i = 3; i < ACTV_LINE->m_parameters_txt.size(); i++)
				{
					tempStrVec.push_back(M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(i)));
				};
				M22Script::ExecuteM22ScriptCommand(tempType,tempStrVec,_line);
				return 0;
			}
			else
			{
				// RETURN FALSE
				M22Script::ChangeLine(++_line);
			};
			return 0;

		case M22Script::MAKE_DECISION:
			M22Script::activeSpeakerIndex = 0;
			M22Engine::skipping = false;
			for(size_t i = 0; i < ACTV_LINE->m_parameters_txt.size(); i++)
			{
				ACTV_LINE->m_parameters_txt.at(i).erase(
					std::remove_if(
						ACTV_LINE->m_parameters_txt.at(i).begin(), 
						ACTV_LINE->m_parameters_txt.at(i).end(), 
						isspace
					)
				);
			};

			//Check decision doesn't already exist.
			for(size_t i = 0; i < M22Script::gameDecisions.size(); i++)
			{
				if(M22Script::gameDecisions.at(i).name == M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(0)))
				{
					M22Engine::moveVectorItemToBack(M22Script::gameDecisions, i);
					return 0;
				};
			};

			tempDecision.name = M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(0));
			for(size_t i = 1; i < ACTV_LINE->m_parameters_txt.size(); i++)
			{
				tempDecision.choices.push_back(M22Script::to_wstring(ACTV_LINE->m_parameters_txt.at(i)));
				tempDecision.num_of_choices++;
			};

			M22Script::gameDecisions.push_back(tempDecision);

			return 0;

		case M22Script::RUN_LUA_SCRIPT:
			tempPath = "./scripts/lua/";
			tempPath += ACTV_LINE->m_parameters_txt.at(0);
			luaL_dofile(M22Lua::STATE, tempPath.c_str());
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::SET_ACTIVE_TRANSITION:
			M22Graphics::activeTransition = ACTV_LINE->m_parameters.at(0);
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::CLEAR_CHARACTERS_BRUTAL:
			M22Script::ClearCharacters();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::FADE_TO_BLACK_FANCY:
			M22Graphics::FadeToBlackFancy();
			M22Script::ChangeLine(++_line);
			return 0;

		default:
			M22Script::activeSpeakerIndex = ACTV_LINE->m_speaker;
			return 0;

	};
};