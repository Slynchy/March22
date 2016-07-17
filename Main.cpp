/*
	March22 Engine Blank Project

	Code by Sam Lynch, 
		Amduat Games
*/

#include <engine/M22Engine.h>

#define DEBUG_ENABLED false

#define WINDOW_TITLE		"M22Engine "
#define VERSION				"v0.7.0"

#define FPS 60

Vec2 ScrPos(600,200);

void InitializeEverything(Vec2 _ScrPos);
void InitializeInterfaces(void);

#undef main
int main(int argc, char* argv[]) 
{
	InitializeEverything(ScrPos);

	if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::INGAME)
	{
		M22Engine::StartGame();
	}
	else if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU)
	{
		M22Sound::ChangeMusicTrack("sfx/music/MENU.OGG");
		M22Interface::activeInterfaces.push_back(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC]);
	};

	while( !M22Engine::QUIT )
	{
		M22Engine::UpdateDeltaTime();
		M22Engine::UpdateEvents();
		M22Sound::UpdateSound();

		if(M22Engine::skipping)
		{
			M22Script::ChangeLine(++M22Script::currentLineIndex);
		};
		if(M22Engine::TIMER_TARGET != 0)
		{
			if(M22Engine::TIMER_CURR < M22Engine::TIMER_TARGET)
			{
				M22Engine::TIMER_CURR += M22Engine::DELTA_TIME;
			}
			else
			{
				M22Engine::TIMER_CURR = 0;
				M22Engine::TIMER_TARGET = 0;
				M22Script::ChangeLine(++M22Script::currentLineIndex);
			};
		};
		if(M22Engine::GAMESTATE == M22Engine::GAMESTATES::MAIN_MENU) M22Graphics::UpdateBackgrounds();
		M22Graphics::UpdateCharacters();
		M22Interface::UpdateActiveInterfaces( int(M22Engine::ScrSize.x()), int(M22Engine::ScrSize.y()) );
		
		M22Renderer::RenderClear();

		switch(M22Engine::GAMESTATE)
		{
			case M22Engine::GAMESTATES::MAIN_MENU:
				M22Renderer::RenderCopy(M22Graphics::activeMenuBackground);
				M22Renderer::RenderCopy(M22Graphics::menuLogo);
				M22Interface::DrawActiveInterfaces();
				break;
			case M22Engine::GAMESTATES::INGAME:
				M22Graphics::DrawInGame();
				break;
			default:
				break;
		};

		M22Engine::LMB_Pressed = false;
		if(!M22Engine::QUIT) M22Renderer::RenderPresent();
		M22Renderer::Delay(1000/FPS); // 1000ms / 60 = 16.66ms delay (60FPS)
	};

	M22Engine::Shutdown();
    return 0;
}

void InitializeEverything(Vec2 _ScrPos)
{
	int ERROR_CODE = 0;
	// Initialize Lua
	ERROR_CODE = M22Lua::Initialize();

	// Initialize the options file
	M22Engine::OptionsFileInitializer();

	// Initialize SDL with specified title, version and at the specified position of the screen (if windowed)
	ERROR_CODE = M22Engine::InitializeSDL(WINDOW_TITLE, VERSION, _ScrPos);

	// Initialize sound engine (setup mixers, etc.)
	ERROR_CODE = M22Sound::InitializeSound();

	// Initialize the M22 engine (loads all required files, returns -1 if error encountered)
	ERROR_CODE = M22Engine::InitializeM22(int(M22Engine::ScrSize.x()),int(M22Engine::ScrSize.y()));
	
	// Loads all the background files from the specified index file
	ERROR_CODE = M22Graphics::LoadBackgroundsFromIndex("graphics/backgrounds/index.txt");
	
	// Initializes the text box (loads appropriate files)
	M22Interface::InitTextBox();
	
	// Loads list of game decisions from the decisions file
	ERROR_CODE = M22Script::LoadGameDecisions("scripts/DECISIONS.txt");

	// Load the text box position
	ERROR_CODE = M22Script::LoadTextBoxPosition("graphics/TEXT_BOX_POSITION.txt");
	
	// Loads the script file into the current file
	//ERROR_CODE = M22Script::LoadScriptToCurrent("START_SCRIPT.txt");
	//ERROR_CODE = M22Script::LoadScriptToCurrent_w("START_SCRIPT.txt");
	ERROR_CODE = M22ScriptCompiler::CompileLoadScriptFile("START_SCRIPT.txt");
	
	// Load the desired font
	M22Graphics::textFont = TTF_OpenFont( "graphics/FONT.ttf", 19);
	
	// Initialize the black wipe files/textures
	M22Graphics::wipeBlack = M22Renderer::LoadTexture("graphics/wipeblack.png");
	M22Renderer::GetTextureInfo(M22Graphics::wipeBlack, M22Graphics::wipeBlackRect.w, M22Graphics::wipeBlackRect.h);
	M22Graphics::wipeBlackRect.w /= 3;

	InitializeInterfaces();

	M22Renderer::SetDrawColor(255, 255, 255, 255);

	M22Graphics::wipePosition = new SDL_Rect();
	M22Graphics::wipePosition->x = (0 - 640);
	M22Graphics::wipePosition->y = 0;
	M22Graphics::wipePosition->w = 640;
	M22Graphics::wipePosition->h = 480;

	if(ERROR_CODE != 0) printf("Error detected! Expect problems!\n");
	return;
};

void InitializeInterfaces(void)
{
	M22Interface::storedInterfaces.resize(M22Interface::INTERFACES::NUM_OF_INTERFACES);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::INGAME_INTRFC], 2, 0, "graphics/interface/GAME_BUTTONS.txt", true, M22Interface::INTERFACES::INGAME_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MENU_BUTTON_INTRFC], 4, 0, "graphics/interface/MENU_BUTTONS.txt", true, M22Interface::INTERFACES::MENU_BUTTON_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::MAIN_MENU_INTRFC], 3, 0, "graphics/mainmenu/BUTTONS.txt", false, M22Interface::INTERFACES::MAIN_MENU_INTRFC);
	M22Interface::InitializeInterface(&M22Interface::storedInterfaces[M22Interface::INTERFACES::OPTIONS_MENU_INTRFC], 7, 0, "graphics/optionsmenu/BUTTONS.txt", true, M22Interface::INTERFACES::OPTIONS_MENU_INTRFC);
	return;
};