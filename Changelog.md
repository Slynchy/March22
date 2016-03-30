#M22/Snow Sakura++ Changelog
###NOTE: Please don't take this changelog as gospel; I hardly ever remember what I add per version.

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