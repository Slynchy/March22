/*
	March22 Interactive Novel Engine Prototype

	By Sam Lynch
*/

#include <engine/M22Engine.h>

#define DEBUG_ENABLED false

#define WINDOW_TITLE		"M22Engine "

#define FPS 60

Vec2 ScrPos(50,50);

void InitializeEverything(Vec2 _ScrPos);
void InitializeInterfaces(void);

#undef main
int main(int argc, char* argv[]) 
{
	InitializeEverything(ScrPos);
	March22::M22Script::font = new NFont(March22::M22Renderer::SDL_RENDERER, "graphics/FONT.ttf", 29, NFont::Color(255, 255, 255, 255));

	if(March22::M22Engine::GAMESTATE == March22::M22Engine::GAMESTATES::INGAME)
	{
		March22::M22Engine::StartGame();
	}
	else if(March22::M22Engine::GAMESTATE == March22::M22Engine::GAMESTATES::MAIN_MENU)
	{
		March22::M22Sound::ChangeMusicTrack("sfx/music/MENU.OGG");
		March22::M22Interface::activeInterfaces.push_back(&March22::M22Interface::storedInterfaces[March22::M22Interface::INTERFACES::MAIN_MENU_INTRFC]);
	};

	while( !March22::M22Engine::QUIT )
	{
		March22::M22Engine::UpdateDeltaTime();
		March22::M22Engine::UpdateEvents();
		March22::M22Sound::UpdateSound();

		if(March22::M22Engine::skipping)
		{
			March22::M22Script::ChangeLine(++March22::M22Script::currentLineIndex);
		};
		if(March22::M22Engine::TIMER_TARGET != 0)
		{
			if(March22::M22Engine::TIMER_CURR < March22::M22Engine::TIMER_TARGET)
			{
				March22::M22Engine::TIMER_CURR += March22::M22Engine::DELTA_TIME;
			}
			else
			{
				March22::M22Engine::TIMER_CURR = 0;
				March22::M22Engine::TIMER_TARGET = 0;
				March22::M22Script::ChangeLine(++March22::M22Script::currentLineIndex);
			};
		};
		if(March22::M22Engine::GAMESTATE == March22::M22Engine::GAMESTATES::MAIN_MENU) March22::M22Graphics::UpdateBackgrounds();
		March22::M22Graphics::UpdateCharacters();
		//March22::M22Interface::UpdateActiveInterfaces( int(March22::M22Engine::ScrSize.x()), int(March22::M22Engine::ScrSize.y()) );
		
		March22::M22Renderer::RenderClear();

		switch(March22::M22Engine::GAMESTATE)
		{
			case March22::M22Engine::GAMESTATES::MAIN_MENU:
				March22::M22Renderer::RenderCopy(March22::M22Graphics::activeMenuBackground);
				March22::M22Renderer::RenderCopy(March22::M22Graphics::menuLogo);
				March22::M22Interface::DrawActiveInterfaces();
				break;
			case March22::M22Engine::GAMESTATES::INGAME:
				March22::M22Graphics::DrawInGame();
				break;
			default:
				break;
		};

		March22::M22Engine::LMB_Pressed = false;
		if(!March22::M22Engine::QUIT) March22::M22Renderer::RenderPresent();
		March22::M22Renderer::Delay(1000/FPS); 
	};

	March22::M22Engine::Shutdown();
    return 0;
}

void InitializeEverything(Vec2 _ScrPos)
{
	int ERROR_CODE = 0;
	// Initialize Lua
	ERROR_CODE = March22::M22Lua::Initialize();

	// Initialize the options file
	March22::M22Engine::OptionsFileInitializer();

	// Initialize SDL with specified title, version and at the specified position of the screen (if windowed)
	ERROR_CODE = March22::M22Engine::InitializeSDL(WINDOW_TITLE, _ScrPos);

	// Initialize sound engine (setup mixers, etc.)
	ERROR_CODE = March22::M22Sound::InitializeSound();

	// Initialize the M22 engine (loads all required files, returns -1 if error encountered)
	ERROR_CODE = March22::M22Engine::InitializeM22(int(March22::M22Engine::ScrSize.x()),int(March22::M22Engine::ScrSize.y()));
	
	// Loads all the background files from the specified index file
	March22::M22Graphics::BACKGROUND_RENDER_TARGET = SDL_CreateTexture( March22::M22Renderer::SDL_RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET , 1920, 1080 );
	SDL_SetTextureBlendMode(March22::M22Graphics::BACKGROUND_RENDER_TARGET, SDL_BLENDMODE_BLEND);
	March22::M22Graphics::NEXT_BACKGROUND_RENDER_TARGET = SDL_CreateTexture( March22::M22Renderer::SDL_RENDERER, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET , 1920, 1080);
	SDL_SetTextureBlendMode(March22::M22Graphics::NEXT_BACKGROUND_RENDER_TARGET, SDL_BLENDMODE_BLEND);
	
	// Initializes the text box (loads appropriate files)
	March22::M22Interface::InitTextBox();
	
	// Loads list of game decisions from the decisions file
	ERROR_CODE = March22::M22Script::LoadGameDecisions("scripts/DECISIONS.txt");

	// Load the text box position
	ERROR_CODE = March22::M22Script::LoadTextBoxPosition("graphics/TEXT_BOX_POSITION.txt");
	
	// Loads the script file into the current file
	//ERROR_CODE = March22::M22ScriptCompiler::CompileLoadScriptFile("START_SCRIPT.txt");
	
	// Load the desired font
	March22::M22Graphics::textFont = TTF_OpenFont( "graphics/FONT.ttf", 19);
	
	// Initialize the black wipe files/textures
	March22::M22Graphics::wipeBlack = March22::M22Renderer::LoadTexture("graphics/wipeblack.png");
	March22::M22Renderer::GetTextureInfo(March22::M22Graphics::wipeBlack, March22::M22Graphics::wipeBlackRect.w, March22::M22Graphics::wipeBlackRect.h);
	March22::M22Graphics::wipeBlackRect.w /= 3;

	InitializeInterfaces();

	March22::M22Renderer::SetDrawColor(255, 255, 255, 255);

	March22::M22Graphics::wipePosition = new SDL_Rect();
	March22::M22Graphics::wipePosition->x = (0 - 1920);
	March22::M22Graphics::wipePosition->y = 0;
	March22::M22Graphics::wipePosition->w = 1920;
	March22::M22Graphics::wipePosition->h = 1080;

	if(ERROR_CODE != 0) printf("Error detected! Expect problems!\n");
	return;
};

void InitializeInterfaces(void)
{
	March22::M22Interface::storedInterfaces.resize(March22::M22Interface::INTERFACES::NUM_OF_INTERFACES);
	March22::M22Interface::InitializeInterface(&March22::M22Interface::storedInterfaces[March22::M22Interface::INTERFACES::INGAME_INTRFC], 2, 0, "graphics/interface/GAME_BUTTONS.txt", true, March22::M22Interface::INTERFACES::INGAME_INTRFC);
	March22::M22Interface::InitializeInterface(&March22::M22Interface::storedInterfaces[March22::M22Interface::INTERFACES::MENU_BUTTON_INTRFC], 4, 0, "graphics/interface/MENU_BUTTONS.txt", true, March22::M22Interface::INTERFACES::MENU_BUTTON_INTRFC);
	March22::M22Interface::InitializeInterface(&March22::M22Interface::storedInterfaces[March22::M22Interface::INTERFACES::MAIN_MENU_INTRFC], 3, 0, "graphics/mainmenu/BUTTONS.txt", false, March22::M22Interface::INTERFACES::MAIN_MENU_INTRFC);
	March22::M22Interface::InitializeInterface(&March22::M22Interface::storedInterfaces[March22::M22Interface::INTERFACES::OPTIONS_MENU_INTRFC], 7, 0, "graphics/optionsmenu/BUTTONS.txt", true, March22::M22Interface::INTERFACES::OPTIONS_MENU_INTRFC);
	return;
};