#include <engine/M22Engine.h>

using namespace March22;

std::vector<M22ScriptCompiler::line_c> M22ScriptCompiler::currentScript_c;
M22ScriptCompiler::line_c* M22ScriptCompiler::CURRENT_LINE;
std::vector<M22ScriptCompiler::script_checkpoint> M22ScriptCompiler::currentScript_checkpoints;

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
		M22Script::currentScriptFileName = _filename;

		// Clear the loaded backgrounds
		for(size_t i = 0; i < M22Graphics::BACKGROUNDS.size(); i++)
		{
			SDL_DestroyTexture(M22Graphics::BACKGROUNDS.at(i));
		};
		M22Graphics::BACKGROUNDS.clear();
		M22Graphics::backgroundIndex.clear();

		// Clear the loaded character sprites
		for(size_t i = 0; i < M22Engine::CHARACTERS_ARRAY.size(); i++)
		{
			M22Engine::CHARACTERS_ARRAY.at(i).emotions.clear();
			M22Engine::CHARACTERS_ARRAY.at(i).outfits.clear();
			for(size_t spr = 0; spr < M22Engine::CHARACTERS_ARRAY.at(i).sprites.size(); spr++)
			{
				M22Engine::DestroySDLTextureVector(M22Engine::CHARACTERS_ARRAY.at(i).sprites.at(spr));
			};
			M22Engine::CHARACTERS_ARRAY.at(i).sprites.clear();
		};

		M22ScriptCompiler::currentScript_c.clear();

		unsigned int linenumber = 0;
		while (std::getline(input , temp)) 
		{
			if(temp.length() >= 2)
			{
				if(temp.at(0) == '/' && temp.at(1) == '/')
				{
					// it's a comment! Deleeeete!
					temp = M22Script::to_wstring("");
				}
				else if(temp.at(0) == '-' && temp.at(1) == '-')
				{
					// it's a checkpoint!
					temp.erase(0, 2);
					M22ScriptCompiler::script_checkpoint tempCheckpoint;
					tempCheckpoint.m_name = M22Script::to_string(temp);
					tempCheckpoint.m_position = linenumber;
					M22ScriptCompiler::currentScript_checkpoints.push_back(tempCheckpoint);
					temp = M22Script::to_wstring("");
				};
			};
			if(temp != M22Script::to_wstring(""))
			{
				scriptLines.push_back(temp);
				linenumber++;
			};
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
			}
			else if(tempLine_c.m_speaker == -1)
			{
				CURRENT_LINE_SPLIT.at(0).erase(std::remove_if(CURRENT_LINE_SPLIT.at(0).begin(), CURRENT_LINE_SPLIT.at(0).end(), isspace));
				CURRENT_LINE_SPLIT.at(0).erase(std::remove_if(CURRENT_LINE_SPLIT.at(0).begin(), CURRENT_LINE_SPLIT.at(0).end(), M22Script::isColon));
				printf("[M22ScriptCompiler] Character index \"%s\" not found!\n", M22Script::to_string(CURRENT_LINE_SPLIT.at(0)).c_str());
			};
		}
		else
		{
			CompileLine(tempLine_c, CURRENT_LINE_SPLIT);
		};

		M22ScriptCompiler::currentScript_c.push_back(tempLine_c);
	};
	return 0;
};

int M22ScriptCompiler::CompileLine(M22ScriptCompiler::line_c &tempLine_c, std::vector<std::wstring> CURRENT_LINE_SPLIT)
{
	std::vector<int> tempint;
	bool failed_char = false;
	bool failed_emot = false;
	bool failed_otft = false;
	switch(tempLine_c.m_lineType)
	{
		case M22Script::DRAW_CHARACTER_BRUTAL:
		case M22Script::DRAW_CHARACTER:
			for(size_t i = 0; i < CURRENT_LINE_SPLIT.size(); i++)
			{
				CURRENT_LINE_SPLIT.at(i).erase(std::remove_if(CURRENT_LINE_SPLIT.at(i).begin(), CURRENT_LINE_SPLIT.at(i).end(), isspace));
			};
			tempint.push_back(
					M22Engine::GetCharacterIndexFromName(CURRENT_LINE_SPLIT.at(1))
				);
			tempint.push_back(
					M22Engine::GetOutfitIndexFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)), 
					tempint.at(0))
				);
			tempint.push_back(
					M22Engine::GetEmotionIndexFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(3)), 
					tempint.at(0))
				);
			tempint.push_back(
					atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(4)).c_str())
				);

			// Failed to find character, so load it
			if(tempint.at(0) == -1)
			{
				M22Engine::CHARACTERS_ARRAY.push_back(M22Engine::CreateCharacter(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));	
				tempint.at(0) = (M22Engine::CHARACTERS_ARRAY.size()-1);
				failed_char = true;
			};
			if(tempint.at(1) == -1)
			{
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).outfits.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)));
				tempint.at(1) = (M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).outfits.size()-1);
				failed_otft = true;
			};
			if(tempint.at(2) == -1)
			{
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).emotions.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(3)));
				tempint.at(2) = (M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).emotions.size()-1);
				failed_emot = true;
			};

			if(failed_char == true || failed_emot == true || failed_otft == true)
			{
				/*if(failed_char == true)
				{
					tempLine_c.m_parameters_txt.push_back(
						"graphics/text_frames/" + 
						M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).name + 
						".png"
					);
					SDL_Texture* tempTexture = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempLine_c.m_parameters_txt.back().c_str());
					if(!tempTexture) printf("[M22ScriptCompiler] Failed to load character textbox %s!\n", tempLine_c.m_parameters_txt.back().c_str());
					M22Graphics::characterFrameHeaders.push_back(tempTexture);
					tempLine_c.m_parameters_txt.pop_back();
				};*/

				tempLine_c.m_parameters_txt.push_back(
					"graphics/characters/" + 
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).name + 
					"/" + 
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).outfits.back() + 
					"/" +
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).emotions.back() +
					".png"
				);
				SDL_Texture* tempTex = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempLine_c.m_parameters_txt.back().c_str());
				if(!tempTex) printf("[M22ScriptCompiler] Failed to load character sprite %s!\n", tempLine_c.m_parameters_txt.back().c_str());
				SDL_SetTextureAlphaMod( tempTex, 0 );
				SDL_SetTextureBlendMode(tempTex, SDL_BLENDMODE_BLEND);
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).sprites.resize(M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).outfits.size());
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).sprites.at(tempint.at(1)).resize(M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).emotions.size());
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).sprites.at(tempint.at(1)).at(tempint.at(2)) = tempTex;
				tempLine_c.m_parameters_txt.pop_back();
			};

			if(!M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).sprites.at(tempint.at(1)).at(tempint.at(2)))
			{
				tempLine_c.m_parameters_txt.push_back(
					"graphics/characters/" + 
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).name + 
					"/" + 
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).outfits.back() + 
					"/" +
					M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).emotions.back() +
					".png"
				);
				SDL_Texture* tempTex = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempLine_c.m_parameters_txt.back().c_str());
				if(!tempTex) printf("[M22ScriptCompiler] Failed to load character sprite %s!\n", tempLine_c.m_parameters_txt.back().c_str());
				SDL_SetTextureAlphaMod( tempTex, 0 );
				SDL_SetTextureBlendMode(tempTex, SDL_BLENDMODE_BLEND);
				M22Engine::CHARACTERS_ARRAY.at(tempint.at(0)).sprites.at(tempint.at(1)).at(tempint.at(2)) = (tempTex);
			};

			tempLine_c.m_parameters.push_back(tempint.at(0));
			tempLine_c.m_parameters.push_back(tempint.at(1));
			tempLine_c.m_parameters.push_back(tempint.at(2));
			tempLine_c.m_parameters.push_back(tempint.at(3));
			break;
		case M22Script::GOTO_DEBUG:
		case M22Script::WAIT:
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)).c_str()));
			break;
		case M22Script::NEW_BACKGROUND_STEALTH:
		case M22Script::NEW_BACKGROUND:
			tempint.push_back(
				M22Engine::GetBackgroundIDFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)))
			);

			// If the ID wasn't found, load it
			if(tempint.back() == -1)
			{
				// Use the m_parameters_txt for temporary string storage
				tempLine_c.m_parameters_txt.push_back( "graphics/backgrounds/" + M22Script::to_string(CURRENT_LINE_SPLIT.at(1)) + ".png" );
				
				// Load the texture to M22Graphics::BACKGROUNDS
				SDL_Texture *temp = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempLine_c.m_parameters_txt.back().c_str());
				if(!temp)
				{
					printf("[M22ScriptCompiler] Failed to load background %s!\n", tempLine_c.m_parameters_txt.back().c_str());
				};
				M22Graphics::BACKGROUNDS.push_back(temp);

				// Push back the filename to the index
				M22Graphics::backgroundIndex.push_back(tempLine_c.m_parameters_txt.back());
				if(tempLine_c.m_parameters_txt.back() == "graphics/backgrounds/BLACK.webp") 
				{
					temp = NULL;
					temp = IMG_LoadTexture(M22Renderer::SDL_RENDERER, tempLine_c.m_parameters_txt.back().c_str());
					M22Graphics::BLACK_TEXTURE = temp;
					SDL_SetTextureBlendMode(M22Graphics::BLACK_TEXTURE, SDL_BLENDMODE_BLEND);
					SDL_SetTextureAlphaMod( M22Graphics::BLACK_TEXTURE, 0 );
					temp = NULL;
				};

				//Delete the temporary string and push back the index location
				tempLine_c.m_parameters_txt.pop_back();
				tempint.back() = (M22Graphics::backgroundIndex.size()-1);
			};
			tempLine_c.m_parameters.push_back(tempint.at(0));
			break;
		case M22Script::NEW_MUSIC:
			tempint.push_back(M22Sound::FindMusicFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));
			if(tempint.at(0) == -1) printf("[M22ScriptCompiler] Failed to find music file \"%s\"!\n",		M22Script::to_string(CURRENT_LINE_SPLIT.at(1)).c_str());
			tempLine_c.m_parameters.push_back(tempint.at(0));
			break;
		case M22Script::PLAY_STING:
		case M22Script::PLAY_STING_LOOPED:
			tempint.push_back(M22Sound::FindStingFromName(M22Script::to_string(CURRENT_LINE_SPLIT.at(1))));
			if(tempint.at(0) == -1) printf("[M22ScriptCompiler] Failed to find sting \"%s\"!\n",M22Script::to_string(CURRENT_LINE_SPLIT.at(1)).c_str());
			tempLine_c.m_parameters.push_back(tempint.at(0));
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
		case M22Script::LOAD_SCRIPT_GOTO:
			tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)));
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)).c_str()));
			break;
		case M22Script::RUN_LUA_SCRIPT:
		case M22Script::GOTO:
		case M22Script::LOAD_SCRIPT:
			tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)));
			break;
		case M22Script::DRAW_SPRITE:
			for (size_t i = 0; i < CURRENT_LINE_SPLIT.size(); i++)
			{
				CURRENT_LINE_SPLIT.at(i).erase(std::remove_if(CURRENT_LINE_SPLIT.at(i).begin(), CURRENT_LINE_SPLIT.at(i).end(), isspace));
			};
			tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)));
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)).c_str()));
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(3)).c_str()));
			M22Graphics::LOADED_SPRITES.push_back(
				*(
					new M22Graphics::M22Sprite(
						tempLine_c.m_parameters_txt.at(0),
						M22Renderer::SDL_RENDERER,
						tempLine_c.m_parameters.at(0), 
						tempLine_c.m_parameters.at(1))
					)
			);
			break;
		// DrawAnimSprite anus 5
		// "anus_0.png" -> "anus_4.png"
		case M22Script::DRAW_SPRITE_ANIMATED:
			tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(1)));				// 0
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(2)).c_str()));	// 0
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(3)).c_str()));	// 1
			// because it only supports integers, save the float as a string									
			tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(4)));				// 1
			tempLine_c.m_parameters.push_back(atoi(M22Script::to_string(CURRENT_LINE_SPLIT.at(5)).c_str()));	// 2
			M22Graphics::LOADED_SPRITES.push_back(
				*(
					new M22Graphics::M22Sprite(
						tempLine_c.m_parameters_txt.at(0),
						M22Renderer::SDL_RENDERER,
						tempLine_c.m_parameters.at(0),
						tempLine_c.m_parameters.at(1),
						true,
						(float)atof(tempLine_c.m_parameters_txt.at(1).c_str()),
						tempLine_c.m_parameters.at(2))
					)
			);
			break;
		case M22Script::IF_STATEMENT:
		case M22Script::MAKE_DECISION:
			for(size_t k = 1; k < CURRENT_LINE_SPLIT.size(); k++)
			{
				tempLine_c.m_parameters_txt.push_back(M22Script::to_string(CURRENT_LINE_SPLIT.at(k)));
			};
			break;
	};
	return 0;
};

int M22ScriptCompiler::ExecuteCommand(M22ScriptCompiler::line_c _linec, int _line)
{
	M22ScriptCompiler::line_c* ACTV_LINE = &_linec;
	std::string tempPath;
	M22Script::Decision tempDecision;
	bool exitflag = false;
	short int specifiedDecision = -1;
	short int specifiedChoice = -1;

	switch(ACTV_LINE->m_lineType)
	{
		case M22Script::NEW_BACKGROUND_STEALTH:
		case M22Script::NEW_BACKGROUND:
			//tempPath = "graphics/backgrounds/";
			//tempPath += M22Graphics::backgroundIndex.at(ACTV_LINE->m_parameters.at(0));
			//tempPath += ".webp";
			M22Engine::ACTIVE_BACKGROUNDS.at(0).sprite = M22Graphics::BACKGROUNDS.at(ACTV_LINE->m_parameters.at(0));
			M22Engine::ACTIVE_BACKGROUNDS.at(0).name = M22Graphics::backgroundIndex.at(ACTV_LINE->m_parameters.at(0));
			SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS.at(0).sprite, 255);
			SDL_SetTextureAlphaMod(M22Engine::ACTIVE_BACKGROUNDS.at(1).sprite, 255);
			SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
			SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
			M22Graphics::UpdateBackgroundRenderTarget();
			SDL_SetTextureAlphaMod(M22Graphics::BACKGROUND_RENDER_TARGET, 255);
			SDL_SetTextureAlphaMod(M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, 255);
			//if(ACTV_LINE->m_lineType == M22Script::NEW_BACKGROUND_STEALTH)
			//	M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::NEW_PAGE:
			M22Script::typewriter_currPos = 0;
			M22Script::typewriter_text.clear();
			M22Script::typewriter_text.resize(0);
			M22Script::ChangeLine(++_line);
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
			M22Script::ChangeLine(++_line);
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

		case M22Script::LOAD_SCRIPT_GOTO:
			M22ScriptCompiler::currentScript_c.clear();
			M22Script::ClearCharacters();
			M22ScriptCompiler::CompileLoadScriptFile(ACTV_LINE->m_parameters_txt.at(0));
			M22Script::ChangeLine(ACTV_LINE->m_parameters.at(0));
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
			M22ScriptCompiler::FindCheckpoint(ACTV_LINE->m_parameters_txt.at(0), _line);
			M22Script::ChangeLine(_line);
			return 0;

		case M22Script::GOTO_DEBUG:
			if(ACTV_LINE->m_parameters.at(0) <= (int)M22ScriptCompiler::currentScript_c.size()) 
			{
				M22Script::ChangeLine(ACTV_LINE->m_parameters.at(0));
			}
			else
			{
				printf("[M22ScriptCompiler] Goto_debug error! Line number exceeds size of script!\n");
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
							M22Script::gameDecisions.at(i).selectedOption = (short int)k;
						};
						if(exitflag == true) break;
					};
				};
				if(exitflag == true) break;
			};
			if(exitflag == false) printf("[M22ScriptCompiler] Decision/choice not found @ %s line %i!\n", M22Script::currentScriptFileName.c_str(), _line);
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
					specifiedDecision = (short int)i;
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
					specifiedChoice = (short int)i;
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
				M22ScriptCompiler::line_c temporaryLineC;
				temporaryLineC.m_lineType = tempType;
				tempStrVec.insert(tempStrVec.begin(), M22Script::to_wstring(""));
				M22ScriptCompiler::CompileLine(temporaryLineC, tempStrVec);
				M22ScriptCompiler::ExecuteCommand(temporaryLineC, _line);
				//M22Script::ExecuteM22ScriptCommand(tempType,tempStrVec,_line);
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

		case M22Script::CLEAR_SPRITES:
			M22Graphics::ACTIVE_SPRITES.clear();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::FADE_TO_BLACK_FANCY:
			M22Graphics::FadeToBlackFancy();
			M22Script::ChangeLine(++_line);
			return 0;

		case M22Script::DRAW_SPRITE_ANIMATED:
		case M22Script::DRAW_SPRITE:
			for (size_t i = 0; i < M22Graphics::LOADED_SPRITES.size(); i++)
			{
				if (M22Graphics::LOADED_SPRITES.at(i).CompareName(ACTV_LINE->m_parameters_txt.at(0)))
				{
					M22Graphics::ACTIVE_SPRITES.push_back(&M22Graphics::LOADED_SPRITES.at(i));
				};
			}
			M22Script::ChangeLine(++_line);
			return 0;

		default:
			M22Script::activeSpeakerIndex = ACTV_LINE->m_speaker;
			return 0;

	};
};

int M22ScriptCompiler::FindCheckpoint(std::string _chkpnt, int &_line)
{
	for(size_t i = 0; i < M22ScriptCompiler::currentScript_checkpoints.size(); i++)
	{
		if(_chkpnt == M22ScriptCompiler::currentScript_checkpoints.at(i).m_name)
		{
			_line = M22ScriptCompiler::currentScript_checkpoints.at(i).m_position;
			return 0;
		};
	};

	printf("[M22ScriptCompiler] FindCheckpoint error! Could not find: %s\n", _chkpnt.c_str());
	return -1;
};