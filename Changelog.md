#||=============================||
|| M22/Snow Sakura++ Changelog ||
||=============================||
	 NOTE: Please don't take 
	this changelog as gospel;
	 I hardly ever remember 
	 what I add per version.

## v0.1.4

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
			~ Test it on main menu?
		* Unicode support on text
			~ Need a way of converting string/cstring to Uint16 array (algorithmically go through every char in string and add them to a Uint16 array?)
