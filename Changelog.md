#March22 Changelog
###NOTE: Please don't take this changelog as gospel; I hardly ever remember what I add per version.

#### v0.6.5

- Started adding M22Lua
- Added new functions to m22 script language:
	* RunLuaScript [filename] - Runs specified lua script from "./scripts/lua"
- Fixed game loading/saving improperly.
- Some other stuff I've probably forgotten.
- Todo list:
	* Possibly optimize March22 script? Instead of reading each line as they come, maybe load each one into a struct and check the linetype at that point, instead of when the line changes.
	* Need to add bindings for C++/M22 functions to Lua

#### v0.6.0

- Started adding M22Renderer
	* Vague attempt to alleviate dependency on SDL so that switching to another renderer only requires modifying one class.
- Updated text renderer
	* Supports Unicode/wide characters
		+ The old, regular string functions are still there, and still used; these will need to be removed
		+ Still not fully tested, may crash if reading decisions with unicode characters.
	* State-driven; only has to draw text when it changes for much less CPU/GPU usage.
	* Added TEXT_BOX_POSITION.txt
		+ Allows customizing the position of the text box

#### v0.5.1

- Added audio feedback when selecting an option/decision

#### v0.5.0

- Implemented branching decisions and IF logic
	* Call "MakeDecision" with the following params:
		+ "MakeDecision" keyword
		+ Name of decision
		+ Choice 1 [optional]
		+ Choice 2 [optional]
		+ Choice 3 [optional]
		+ Example: MakeDecision TEST OPT1 OPT2 OPT3
		+ Alternatively: MakeDecision TEST
	* Then call m22IF like so:
		+ "m22IF" keyword
		+ Decision name
		+ Option to check if chosen
		+ Function to run if true
		+ Function parameters [optional]
		+ Example: m22IF TEST OPT3 Goto 50
- Added new functions to m22 script language:
	* MakeDecision ( _decision, _choice1, _choice2, [_choice3] ) - Creates a new decision and makes the player make a choice
	* MakeDecision ( _decision ) - Finds the decision from the decisions array and makes the player make a decision

#### v0.4.9a

- Memory leak hotfixes

#### v0.4.9

- Started implementing decisions
	* Loads decisions from text file
		+ Number of decisions (ex. "1")
		+ Name of decision + number of choices (ex. "TEST_DECISION 2")
		+ Choices seperated by newlines (ex. "YES \n NO")
- Added new functions to m22 script language:
	* m22IF ( _decision, _choice, _function_if_true ) - Basic if logic; runs the function if the specified choice is made in the specified decision.

#### v0.4.7

- Behind-the-scenes improvements to the engine
	* Added SDL_DisplayMode getter, could be useful later
	* Made Main.cpp a lot smaller, so more of the grunt is on the engine
	* Other stuff I forgot.
- Bug/to-do list:
	* Shift still crashes the game if there is no skip button; add a quick if-null check to fix.
	* More transitions
	* Add documentation for M22 language
	* Make a "skeleton" project

#### v0.4.4

- New transition system
	* Transitions are hard-coded, but relatively easy to add new ones
		+ Add an entry to the TRANSITIONS enum
		+ Add the text entry to TRANSITION_NAMES array
		+ Add a case for it in "DrawInGame()" function, similar to others
	* Transitions are state-driven, so there is an active transition that is used all the time
	* Use script functions to change this
- Added new functions to m22 script language:
	* SetActiveTransition ( _name ) - changes the active transition method
- Entirely ported the first script to M22 language; released a very cut-down demo to demonstrate release method
- Known bugs:
	* "MENU" button is not the same color as surrounding UI
	* Many options in options menu is missing
	* When quitting, you might quickly see a background after fading to black
	* All buttons except "MENU" and "OPTIONS" are removed
		+ SKIP doesn't work properly; holding shift works fine though
		+ AUTO, SAVE, LOAD, LOG does nothing anyway

#### v0.4.0

- Overhauled background drawing system
	* Instead of drawing a background and layering characters on top of it, the background is composited off-screen into a single texture
	* Background + Characters = new background
	* New background is then phased into current background
	* Current background does not get updated until it needs to be (state-based)
	* The following M22 functions are therefore deprecated for now:
		+ ClearCharactersBrutal
		+ DrawCharactersBrutal
		+ FadeToBlack
- Added basis of transition system
	* At the moment only wipe-from-right
- Fixed character fading to an acceptable standard
- Ported all of the first script with some inaccuracies
	* Incorrect/no music
	* Not all/many SFX
	* Saki is wearing incorrect indoor outfit at the school; should be wearing Indoors_2
- Added new functions to m22 script language:
	* ExitGame - Exits the game!
	* MainMenu - Exits to main menu!
- Bugs:
	* Clicking "QUIT" from main menu doesn't fade to black; solid black
	* The script problems mentioned above
	* Sometimes when transitioning between scenes, a black line is visible after the transition
	* Only one transition
		+ Add more transitions
		+ Need to add parameter to DrawBackground to specify the type of transition

#### v0.3.1

- Fixed options menu inaccuracies
- Added windowed/fullscreen to options menu
- Added options file saving/loading/updating functions
	* Also support for fullscreen/windowed-borderless
- Reverted back to OpenGL...
	* The high CPU-usage is caused by whatever method of framelimiting/VSync they use. SDL_Delay(1000/60) brings CPU usage back to normal with no damage to framerate

#### v0.3.0

- Added commenting to the M22 scripts
	* Single line, same as C++ ("//")
- Added new functions to m22 script language:
	* ClearCharactersBrutal - Clears characters with no lerp effect; just immediately clears the vector
	* DrawCharacterBrutal - Draws character with no lerp effect
	* Wait ( _millisecs ) - Waits the specified time before going to the next line
- Indirectly fixed janky character transition with the two brutal functions
- Fixed main menu buttons being clickable before appearing
- Holding shift now skips FadeToBlackFancy but this might be reverted, since it bugs out a bit
- Added new SFX/BGM
- Optimized SFX volume to be configurable on a channel basis
- Removed SFX line count
- Removed RENDERER shorthand since I never bloody used it...
- Switched back to Direct3D
	* While OpenGL makes more sense it was a CPU hog
	* Direct3D uses practically no CPU power, but is a RAM hog
	* RAM < CPU, so OGL is out for now
- Fixed bilinear filtering distorting the chat box
	* Chat box is drawn independently then blitted onscreen; probably sucks perf-wise
- Converted more of script to M22
- Todo list:
	* Finish options menu
	* More transitions
	* Unicode support
	* Write a tool that extracts/formats SFX/images from Snow Sakura

#### v0.2.4

- Interface improvements
	* Alpha'd the main menu buttons so they look nicer
	* Added TEST_SFX button to options
	* Fixed some problems with the ResetGame() function
- Added new functions to m22 script language:
	* FadeToBlackFancy - Hijacks the main thread to draw a nicer fade to black
- Converted more of the first script to M22
- Added new character parameters (Takeaki)
- Todo list:
	* Unicode support
	* More transitions than simply lerping opacity
		+ Like wiping from left to right while chatbox is down.
	* Finish options menu
		+ Slider bar implementation
		+ Saving/Loading options
		+ Tick box implementation
		+ Convert volume/lerp speed to a modifiable value, instead of a define
	* Quit quits too quickly; popup that says "are you sure?" should fix it; can hook into the SDL_Quit event too
	* Bugs:
		+ Fading characters in and out looks janky. Not as noticable when 1 to 1 changing, but gets worse with scene complexity.
		+ Holding shift doesn't skip FadeToBlackFancy
		+ Can click main menu options before they appear; simple check to see if opacity is > 255*0.75?

#### v0.2.3

- Interface improvements
	* Added an options menu
		+ Only has a quit-to-title option at the moment
	* Added quit and options buttons to main menu
		+ All main menu buttons need to be alpha'd; otherwise they need to be perfectly aligned...
	* Every interface now has a background; if "BLANK" is specified, it just uses a blank pixel
- Updated main menu transitions to and from.
- Added new functions to m22 script language:
	* PlayLoopedSting ( _name ) - Plays the specified sting sound on repeat in a seperate channel
	* StopLoopedStings - Stops all tracks in the looped sting channel
- Fixed more text files not needing a line count prefix
- Added more characters/character names
- Added more M22 to the first script
- Switched to OpenGL; has built-in vsync plus is multi-platform
- Added bilinear filtering option
	* "Destabilises" the text box composition; perhaps render this outside of the main renderer w/o filtering then blit to screen?
- Todo list:
	* Update main menu buttons to have a transparent background
	* Finish options menu implementation
	* STILL NEED TO CONVERT TO UNICODE.

#### v0.2.0

- Interface improvements/implementation
	* An interface is composed of buttons
		+ Button functions need to be pre-programmed in C++, fix or not?
	* Added skip, auto and menu buttons into ingame
- Main menu button support
- Added new functions to m22 script language:
	* DarkenScreen - Darkens the screen
	* BrightenScreen - Restores screen to normal from DarkenScreen
	* Goto ( _n ) - Goes straight to line N; for debugging purposes
- Bug fixes:
	* Prevent enter, space and shift being pressed on the main menu
	* Removed line count at the top of m22 script files and added an iterator to count lines
		+ Need to add this to other .txt files
- Not mentioned in previous commit, but resources were removed from the Github so as to not potentially infringe copyright.
	* At some point, will need to write a script that goes through extracted 32bit BMP files and sorts them into correct folders and converts accordingly.
- Probably some more things, I forget alot.
- Todo list:
	* Still need to add unicode support... see v0.1.4
	* Add fade to white from main menu and fade-in to scene
	* Add transitions between scripts
	* Finish adding main menu buttons
	* Add in pause menu stuff such as options, backlog, etc.
		+ Backlog needs a logical limit; no more than 10-15 lines?


#### v0.1.4

- Added basic fade-in/out of backgrounds and characters
- Shift key to skip dialogue faster
- Basic main menu functionality
	* Very. Basic.
- Added event support for mouse position updating (mouseover) and mousedown updating (clicking)
- Resolution scaling support (4:3 ratio only, since that's what most run at anyway)
- Added new functions to m22 script language:
	* FadeToBlack - Shorthand for DrawBackground BLACK
- Todo list:
	* Seperate engine components into their own header files for ease of access
	* Finish implementing UI system
		+ Test it on main menu?
	* Unicode support on text
		+ Need a way of converting string/cstring to Uint16 array (algorithmically go through every char in string and add them to a Uint16 array?)