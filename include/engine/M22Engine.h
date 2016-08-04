/// \file 		M22Engine.h
/// \author 	Sam Lynch
/// \brief 		Header file for entire M22 engine
/// \version 	0.7.4
/// \date 		July 2016
/// \details	The header file for declaring the engine and its functions/variables.

#ifndef M22ENGINE_H
#define M22ENGINE_H

#define MAJOR 0
#define MINOR 7
#define PATCH 4

#define SPRITE_DEFAULT_FORMAT ".webp"

#define DEFAULT_MUSIC_VOLUME_MULT 0.25f
	/*!< The default volume of music playback. */
#define DEFAULT_SFX_VOLUME_MULT 0.35f
	/*!< The default volume of \a SFX. */
#define DEFAULT_LERP_SPEED 0.15f
	/*!< Defines the speed of any \a lerp function. */

#define RENDERING_API		"opengl"
	/*!< Defines which rendering API to use; generally \a direct3d or \a opengl. */
#define BILINEAR_FILTERING	"1"
	/*!< Set to 1 for bilinear filtering, 0 for not. Must be a string. */

#define DECISION_CHOICE_TEXT_SPACING 20
	/*!< Defines how far apart the decision texts are in pixels */


#include <SDL.h>
#include <lua/lua.hpp>
#include <lua/lauxlib.h>
#include <lua/lualib.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <engine/Vectors.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>
#include <chrono>
#include <codecvt>
#include <sstream>

namespace March22
{ 

	/// \class 		M22Engine M22Engine.h "include/M22Engine.h"
	/// \brief 		The main class of M22.
	///
	/// \details 	This class holds most barebones functions/variables for basic operation (SDL window, renderer, etc.)
	///
	class M22Engine
	{
		private:
		public:

			/// Different window states for the renderer
			enum WINDOW_STATES
			{
				WINDOWED,
					///< Screen is windowed and resizable.
				FULLSCREEN,
					///< Screen is fullscreen at the application resolution.
				FULLSCREEN_BORDERLESS,
					///< Screen is windowed but fullscreen-borderless.
				NUM_OF_STATES
			};
		
			/// Data structure of the options file
			struct OPTIONS_STRUCTURE
			{
				Uint8 WINDOWED;
					///< Refers to M22Engine::WINDOW_STATES for what state the window should be in.
				float AUTO_SPEED;
					///< Speed to auto-click/progress at. \a CURRENTLY UNUSED.
				float MUSIC_VOLUME;
					///< Volume to play music at.
				float SFX_VOLUME;
					///< Volume to play \a SFX at.
				OPTIONS_STRUCTURE()
				{
					WINDOWED = 0;
					AUTO_SPEED = 1.0f;
					MUSIC_VOLUME = DEFAULT_MUSIC_VOLUME_MULT;
					SFX_VOLUME = DEFAULT_SFX_VOLUME_MULT;
				};
			};

			static OPTIONS_STRUCTURE OPTIONS; ///< Current game options, initialized to default, later loaded.

			/// Structure for version control
			struct M22Version
			{
				Uint8 major,minor,patch;
				bool Compare(M22Version _other)
				{
					return (_other.major == this->major && _other.minor == this->minor && _other.patch == this->patch);
				};
				friend std::string &operator+=( std::string &out, const M22Version &ver )
				{ 
					std::ostringstream convert;
					convert << "v" << (int)ver.major << "." << (int)ver.minor << "." << (int)ver.patch << '\0';
					out += convert.str();
					return out;            
				};
			};

			/// Current version of engine, defined in the header
			static M22Engine::M22Version M22VERSION;

			/// Data structure of loaded characters
			struct Character
			{
				std::string name;
					///< Characters name (e.g. "Yuuji")
				std::vector<std::vector<SDL_Texture*>> sprites;
					///< 2D array of sprites; y = outfit, x = emotion
				std::vector<std::string> emotions;
					///< Emotion names (for file-loading, e.g. "Happy_1" - "Happy_1.png")
				std::vector<std::string> outfits;
					///< Outfit names (for file-loading, e.g. "School" - "School/Happy_1.png")
			};
			/// Structure for characters to draw on-screen.
			struct CharacterReference
			{
				SDL_Texture* sprite;
					///< The active sprite
				SDL_Rect rect;
					///< Position to draw to screen
				float alpha;
					///< Current alpha amount
				bool clearing;
					///< Is this character currently being erased off the screen?
				CharacterReference()
				{
					alpha = 0.0f;
					clearing = false;
					sprite = NULL;
				};
			};
			/// Structure for backgrounds
			struct Background
			{
				SDL_Texture* sprite;
					///< The background image
				float alpha;
					///< Current alpha amount
				std::string name;
					///< Name of background
			};
			/// Possible gamestates
			enum GAMESTATES
			{
				MAIN_MENU,
					///< Is on the main menu
				INGAME
					///< Is in-game
			};

			struct SAVEGAME_STRUCTURE
			{
				GAMESTATES		GAMESTATE;
				std::string		CURRENTBACKGROUND;
				std::string		CURRENTMUSIC;
				std::string		CURRENTLOOPINGSFX;
				std::string		CURRENTSCRIPTFILE;
				Uint32			CURRENTLINEPOSITION;
			};
		
			static Vec2 MousePos; 		///< Current mouse position
			static bool LMB_Pressed; 	///< Is LMB currently pressed?

			static bool skipping;		///< Is player currently skipping dialogue?
		
			static bool QUIT;			///< Exit the program?
		
			template <typename T>

			/// Places the specified vector item to the back
			/// \author http://stackoverflow.com/questions/23789498/moving-a-vector-element-to-the-back-of-the-vector
			/// \param v Vector.
			/// \param itemIndex Position of item to place to back
			static void moveVectorItemToBack(std::vector<T>& v, size_t itemIndex)
			{
				auto it = v.begin() + itemIndex;
				std::rotate(it, it + 1, v.end());
			};
		

			/// Creates a new character
			///
			/// \param _name Name of character
			/// \return The created character
			static M22Engine::Character CreateCharacter(std::string _name);
		
			/// Finds the index of the background from the string
			///
			/// \param _name Filename
			/// \return Index of background, -1 if not found
			static int GetBackgroundIDFromName(std::string _name);

			/// Finds the index of the character from the string
			///
			/// \param _name Character's name.
			/// \param _dialogue Don't know; just leave it blank!
			/// \return Index of character, 0 if narrative
			static int GetCharacterIndexFromName(std::string, bool _dialogue = false); 

			/// Finds the index of the character from the string
			///
			/// \param _name Character's name.
			/// \param _dialogue Don't know; just leave it blank!
			/// \return Index of character, 0 if narrative
			static int GetCharacterIndexFromName(std::wstring, bool _dialogue = false); 

			/// Finds the outfit index for the specified character
			///
			/// \param _input Outfit's name.
			/// \param _charIndex Index of character
			/// \return Index of outfit
			static int GetOutfitIndexFromName(std::string, int);
		
			/// Finds the emotion index for the specified character
			///
			/// \param _input Emotion name.
			/// \param _charIndex Index of character
			/// \return Index of emotion
			static int GetEmotionIndexFromName(std::string, int _charIndex);

			/// Resets the game + variables
			static void ResetGame(void);

			/// Starts the game
			static void StartGame(void);

			/// Saves the game state
			static void SaveGame(const char* _filename);

			/// Loads the game state
			static void LoadGame(const char* _filename);

			/// Saves the current configuration of options to OPTIONS.SAV
			static void SaveOptions(void);

			/// Loads the current configuration of options to OPTIONS.SAV
			static void LoadOptions(void);
		
			/// Updates OPTIONS struct + file with new settings
			static void UpdateOptions(void);
		
			/// Shuts down the engine, SDL, and destroys variables
			static void Shutdown(void);

			/// Iterates through the vector and destroys the textures, then clears the vector to get rid of dangling ptrs
			static void DestroySDLTextureVector(std::vector<SDL_Texture*>& _vector);

			/// Iterates through the vector and destroys the music, then clears the vector
			static void FreeMusicVector(std::vector<Mix_Music*>& _vector);

			/// Iterates through the vector and destroys the SFX, then clears the vector
			static void FreeSFXVector(std::vector<Mix_Chunk*>& _vector);

			/// Updates delta time variables
			static void UpdateDeltaTime(void);
		
			/// Updates SDL Events
			static void UpdateEvents(void);
		
			/// Updates keyboard input array
			static void UpdateKeyboard(void);
		
			/// Initializes OPTIONS data by saving if it doesn't exist, loading if it does
			static void OptionsFileInitializer(void);

			static Vec2 ScrSize; 		///< Logical screen resolution to render at

			static unsigned short int ACTIVE_BACKGROUND_INDEX;		///< The index of the current background, relative to \a ACTIVE_BACKGROUNDS;
			static std::vector<Background> ACTIVE_BACKGROUNDS;		///< Array of backgrounds to be drawn (should be no larger than two)

			static std::vector<Character> CHARACTERS_ARRAY;			///< Array of loaded characters and their sprites
			static std::vector<std::string> CHARACTER_EMOTIONS;		///< Array of character emotions \a(UNUSED?)

			static Uint32 last, DELTA_TIME; 						///< Delta time variables
			static Uint32 TIMER_CURR, TIMER_TARGET;					///< Delta time variables

			static M22Engine::GAMESTATES GAMESTATE;					///< Active gamestate from \a M22Engine::
			static SDL_Event SDL_EVENTS;							///< Context for the SDL Events
			static const Uint8 *SDL_KEYBOARDSTATE;					///< Current keyboard button states
			static SDL_DisplayMode SDL_DISPLAYMODE;
		
			/// Initializes the M22 engine
			///
			/// \param ScrW Screen width
			/// \param ScrH Screen height
			/// \return Error code, if 0 then init'd fine
			static short int InitializeM22(int ScrW, int ScrH);
		
			/// Initializes the SDL part of the engine
			///
			/// \param _windowTitle Title of window
			/// \param ScrPos Position window should be when initialized if windowed
			/// \return Error code, if 0 then init'd fine
			static short int InitializeSDL(const std::string _windowTitle, Vec2 ScrPos);
	};

	/// \class 		M22Graphics M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for graphics drawing
	///
	/// \details 	This class is responsible for drawing backgrounds and characters (graphics-related operations).
	///
	class M22Graphics
	{
		private:
		public:
			/// The type of update the background is doing
			enum BACKGROUND_UPDATE_TYPES
			{
				NONE,
				BACKGROUND,				///< Changing the background
				CHARACTER				///< Updating the characters
			};
			/// Data structure for the animated arrow for text progression
			struct ArrowObj
			{
				SDL_Texture* sprite;	///< Sprite
				float frame;			///< The frame that the arrow is on
				ArrowObj()
				{
					sprite = NULL;
					frame = 0.0f;
				};
			};

			class M22Sprite
			{
				private:
					bool m_animated;							///< Is the sprite animated?
					float m_frame;								///< The current frame of the animation (ignore if not animated)
					std::vector<SDL_Texture*> m_sprites;		///< The actual sprites
					float m_speed;								///< How fast to animate the sprite
					SDL_Rect m_coords;							///< Position and size of sprite
					bool m_active;								///< Is the sprite active? (for preloading)
				public:
					SDL_Texture** currSprite;
					inline bool IsActive()
					{
						return m_active;
					}
					inline bool IsAnimated()
					{
						return m_animated;
					};
					inline float frame()
					{
						return m_frame;
					};
					inline int x()
					{
						return m_coords.x;
					}
					inline int y()
					{
						return m_coords.y;
					}
					inline int w()
					{
						return m_coords.w;
					}
					inline int h()
					{
						return m_coords.h;
					}
					inline void Update()
					{
						m_frame += m_speed;
						currSprite = &m_sprites.at((int)m_frame);
					}
					inline void Draw(SDL_Renderer* _renderer)
					{
						SDL_RenderCopyEx(
							_renderer,
							*currSprite,
							NULL,
							&m_coords,
							NULL,
							NULL,
							SDL_FLIP_NONE
						);
					}
					M22Sprite( std::string _file, SDL_Renderer* _renderer, int _x = 0, int _y = 0, bool _animated = false, float _speed = 1.0f,  unsigned short int _num_of_frames = 1)
					{
						m_coords.x = _x;
						m_active = false;
						m_coords.y = _y;
						m_frame = 0.0f;
						m_animated = _animated;
						m_speed = _speed;
						m_sprites.resize(_num_of_frames);
						currSprite = &m_sprites.at(0);

						if (_animated)
						{
							for (size_t i = 0; i < m_sprites.size(); i++)
							{
								std::string tempStr = "./graphics/sprites/";
								tempStr += _file + "_";
								tempStr += (i + SPRITE_DEFAULT_FORMAT);
								m_sprites.at(i) = IMG_LoadTexture(_renderer, tempStr.c_str());
								if (!m_sprites.at(i)) printf("[M22Sprite] Failed to load sprite file %s!\n", tempStr.c_str());
							}
						}
						else
						{
							std::string tempStr = "./graphics/sprites/";
							tempStr += (_file + SPRITE_DEFAULT_FORMAT);
							*currSprite = IMG_LoadTexture(_renderer, tempStr.c_str());
							if (!*currSprite) printf("[M22Sprite] Failed to load sprite file %s!\n", tempStr.c_str());
						}

						SDL_QueryTexture(*currSprite, NULL, NULL, &m_coords.w, &m_coords.h);
					}
					~M22Sprite()
					{
						for (size_t i = 0; i < m_sprites.size(); i++)
						{
							SDL_DestroyTexture(m_sprites.at(i));
						}
						m_sprites.clear();
						m_sprites.resize(0);
						currSprite = nullptr;
						m_frame = 0.0f;
						m_speed = 0.0f;
					}
			};

			static SDL_Texture* BACKGROUND_RENDER_TARGET;						///< The off-screen render target for the background
			static SDL_Texture* NEXT_BACKGROUND_RENDER_TARGET;					///< The off-screen render target for the next background
			static float NEXT_BACKGROUND_ALPHA;									///< The alpha of the next background (for fading in)
			static BACKGROUND_UPDATE_TYPES changeQueued;						///< The type of the background change scheduled

			static std::vector<M22Sprite> ACTIVE_SPRITES;						///< Sprites to draw

			static SDL_Texture* textFrame;										///< Texture for the primary text frame
			static ArrowObj arrow;												///< The text arrow object
			static std::vector<SDL_Texture*> characterFrameHeaders;				///< The array of sprites for character names when they talk
			static std::vector<M22Engine::CharacterReference> activeCharacters;	///< Array of active characters to draw to the screen
			static std::vector<SDL_Texture*> mainMenuBackgrounds;				///< The possible backgrounds for the main menu to use, loaded into this array
			static M22Engine::Background activeMenuBackground;					///< The active background for the main menu
			static M22Engine::Background menuLogo;								///< The game's logo to draw onto the main menu
			static SDL_Texture* OPTION_BAR;										///< The texture for bars in the options, like volume (UNUSED)

			static SDL_Texture* BLACK_TEXTURE;									///< A simple solid black texture for fading to black (0,0,0,255)

			static std::vector<SDL_Texture*> BACKGROUNDS;						///< Loaded background textures
			static std::vector<std::string> backgroundIndex;					///< Names of the backgrounds to know which one to load for scripts
		
			/// Loads backgrounds into loaded backgrounds array from a filepath
			///
			/// \param _filename File path
			/// \return Error code, if 0 then init'd fine
			static short int LoadBackgroundsFromIndex(const char* _filename);
		
			/// Draws the active background to a render target
			/// \param _target Render target
			static void DrawBackground(SDL_Texture* _target);

			/// Updates background states/alpha
			static void UpdateBackgrounds(void);

			/// Update character states/alpha
			static void UpdateCharacters(void);
		
			/// Draw the ingame screen (background, text box, interfaces, etc.)
			static void DrawInGame(bool _draw_black = true);

			/// Hijack the renderer and fade to black slowly
			static void FadeToBlackFancy(void);
		
			/// Adds the specified character to the active characters to draw
			///
			/// \param _charindex Index of character to add to active
			/// \param _outfitindex Index of character's outfit
			/// \param _emotionindex Index of character's emotion
			/// \param _xPosition Position on the X-axis
			/// \param _brutal Fade-in or just insert?
			static void AddActiveCharacter(int _charindex, int _outfitindex, int _emotionindex, int _xPosition, bool _brutal = false);
		
			/// Updates/resets the render target
			static void UpdateBackgroundRenderTarget(void);
		
			/// Adds the specified character to the background render target (hard-drawn to the background)
			///
			/// \param _charindex Index of character to add to active
			/// \param _outfitindex Index of character's outfit
			/// \param _emotionindex Index of character's emotion
			/// \param _xPosition Position on the X-axis
			/// \param _brutal Fade-in or just insert?
			static void AddCharacterToBackgroundRenderTarget(int _charindex, int _outfitindex, int _emotionindex, int _xPosition, bool _brutal = false);
		
			/// Draws the animated arrow at correct location, taking into account width/height of target
			///
			/// \param ScrW Screen width
			/// \param ScrH Screen height
			static void DrawArrow(int ScrW, int ScrH);
		
			/// Basic lerp function
			///
			/// \param _var1 Original value
			/// \param _var2 Target value
			/// \param _t Delta time
			/// \return Lerp'd value
			static float Lerp(float _var1, float _var2, float _t); 

			static TTF_Font *textFont;											///< The TTF font to use for speech/narrative text.

			static SDL_Rect* wipePosition;										///< The current position of the active transition wipe
			static SDL_Texture* wipeBlack;										///< A texture for wiping black; just \a BLACK_TEXTURE reference
			static SDL_Rect wipeBlackRect;										///< The current position of the black wipe
		
			/// Type of transition
			enum TRANSITIONS
			{
				SWIPE_TO_RIGHT,													///< Swipe from left of the screen to right
				SWIPE_DOWN,														///< Swipe from top of the screen to bottom
				SWIPE_TO_LEFT,													///< Swipe from right of the screen to left
				FADEIN,															///< Lerp/fade between the backgrounds
				NUMBER_OF_TRANSITIONS
			};

			static const std::wstring TRANSITION_NAMES[NUMBER_OF_TRANSITIONS];	///< Name of transitions for scripts to use

			static Uint8 activeTransition;										///< Which transition to use, refering to \a TRANSITIONS enum
	};

	/// \class 		M22Sound M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for sound handling
	///
	/// \details 	This class is responsible for music/sound playback/loading/managing.
	///
	class M22Sound
	{
		private:
		public:
			/// Enumerator for different mixers/channels for specific types of sound
			enum MIXERS
			{
				BGM,											///< Background music
				SFX,											///< Sound FX
				VOICE,											///< Voice (UNUSED CURRENTLY)
				LOOPED_SFX										///< SFX that is to loop continuously
			};
			static std::vector<Mix_Chunk*> SOUND_FX;			///< Array of loaded SFX files
			static std::vector<Mix_Music*> MUSIC;				///< Array of loaded music files
			static std::vector<std::string> MUSIC_NAMES;		///< Array of loaded music file names (for scripts)
			static std::vector<std::string> SFX_NAMES;			///< Array of loaded SFX file names (for scripts)
			static float* MUSIC_VOLUME;							///< Current volume for music playback
			static float* SFX_VOLUME;							///< Current volume for SFX playback
			static int currentTrack;							///< The active track to play in \a MUSIC array. 0 = silence

			/// Finds and returns the ID of the specified track
			///
			/// \param _name Name of sound file to find
			/// \return -1 if error encountered, ID of sound file if found
				inline static int FindMusicFromName(std::string _name)
				{
					std::string tempPath = "sfx/music/";
					tempPath += _name;
					tempPath += ".OGG";
					for(size_t i = 0; i < M22Sound::MUSIC.size(); i++)
					{
						if(tempPath == M22Sound::MUSIC_NAMES.at(i))
						{
							return i;
						};
					};
					return -1;
				};

			/// Finds and returns the ID of the specified sting
			///
			/// \param _name Name of sound file to find
			/// \return -1 if error encountered, ID of sound file if found
				inline static int FindStingFromName(std::string _name)
				{
					std::string tempPath = "sfx/stings/";
					tempPath += _name;
					tempPath += ".OGG";
					for(size_t i = 0; i < M22Sound::SOUND_FX.size(); i++)
					{
						if(tempPath == M22Sound::SFX_NAMES.at(i))
						{
							return i;
						};
					};
					return -1;
				};

			/// Plays a SFX once, doesn't play if a SFX is already playing
			///
			/// \param _position Index of sound file from \a SOUND_FX array.
			/// \return Error code if problem encountered, 0 if fine
				static short int PlaySting(short int);
			
			/// Plays a SFX once, can be forced to playback if true
			///
			/// \param _position Index of sound file from \a SOUND_FX array.
			/// \param _forceplayback If true, will force the sound effect to play as high priority.
			/// \return Error code if problem encountered, 0 if fine
				static short int PlaySting(short int, bool);
			
			/// Searches for the SFX from the string, and plays back.
			///
			/// \param _name Name of sound file from \a SFX_NAMES array.
			/// \param _forceplayback If true, will force the sound effect to play as high priority.
			/// \return Error code if problem encountered, 0 if fine
				static short int PlaySting(std::string, bool);
			
			/// Plays back the specified sting on a continuous loop.
			///
			/// \param _position Position of sound file from \a SFX_NAMES array.
			/// \return Error code if problem encountered, 0 if fine
				static short int PlayLoopedSting(short int);
			
			/// Searches for the SFX from the string, and plays back on a continuous loop.
			///
			/// \param _name Name of sound file from \a SFX_NAMES array.
			/// \return Error code if problem encountered, 0 if fine
				static short int PlayLoopedSting(std::string);
			
			/// Stops anything playing in the \a LOOPED_SFX mixer
				static void StopLoopedStings(void);

			/// Changes the active music track
			///
			/// \param _position Index of sound file in \a MUSIC array.
			/// \return Error code if problem encountered, 0 if fine
				static short int ChangeMusicTrack(short int _position);
			
			/// Searches for track from string, changes the active music track to result
			///
			/// \param _name Name of sound file from \a MUSIC_NAMES array.
			/// \return Error code if problem encountered, 0 if fine
				static short int ChangeMusicTrack(std::string _name);
			
			/// Stops anything playing in the \a BGM mixer
				static void StopMusic();
			/// Pauses anything playing in the \a LOOPED_SFX mixer
				static void PauseMusic();
			/// Resumes whatever is paused in the \a LOOPED_SFX mixer
				static void ResumeMusic();

			/// Updates the sound, checking whether to loop
				static void UpdateSound(void);

			/// Initializes music+SFX
			/// \return -1 if music fails, -2 if SFX fails, 0 if fine
				static short int InitializeSound(void);

			/// Initializes music
			/// \return Anything other than 0 means an error
				static short int InitializeMusic(void);

			/// Initializes SFX
			/// \return Anything other than 0 means an error
				static short int InitializeSFX(void);
	};

	/// \class 		M22Script M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for script handling
	///
	/// \details 	This class is responsible for loading and handling script files for M22.
	///
	class M22Script
	{
		private:
		public:
			/// Enumerator for the type of line the script is on
			enum LINETYPE
			{
				NEW_BACKGROUND,					///< Changes the background using active transition
				FADE_TO_BLACK,					///< Fade the background to black
				FADE_TO_BLACK_FANCY,			///< Fade the background to black; hijacks the thread for a nicer effect.
				NEW_MUSIC,						///< Changes the current music track
				DARK_SCREEN,					///< Darkens the screen slightly
				BRIGHT_SCREEN,					///< Restores the screen brightness from dark screen
				STOP_MUSIC,						///< Stops music playback
				PLAY_STING,						///< Plays the specified SFX
				PLAY_STING_LOOPED,				///< Plays the specified SFX on loop
				STOP_STING_LOOPED,				///< Stops any looped SFX
				GOTO_DEBUG,						///< Goes straight to specified line
				GOTO,							///< Finds the specified checkpoint and goes to it
				DRAW_CHARACTER,					///< Add new character to active characters
				CLEAR_CHARACTERS,				///< Remove all characters from active characters
				CLEAR_CHARACTERS_BRUTAL,		///< Remove all characters from active characters without transition
				DRAW_CHARACTER_BRUTAL,			///< Add new character to active characters without transition
				LOAD_SCRIPT,					///< Terminate the current script and load the specified one
				LOAD_SCRIPT_GOTO,				///< Combination of LOAD_SCRIPT and GOTO
				SPEECH,							///< Speech from a character
				COMMENT,						///< Code comment
				WAIT,							///< Waits N milliseconds before loading the next line
				EXITGAME,						///< Exit game
				SET_ACTIVE_TRANSITION,			///< Changes the active transition
				EXITTOMAINMENU,					///< Exit to main menu
				IF_STATEMENT,					///< Basic if statement
				MAKE_DECISION,					///< Triggers decision-making interface
				SET_DECISION,					///< Sets a decision into the array
				RUN_LUA_SCRIPT,					///< Runs a lua script
				DRAW_SPRITE,					///< Draws the specified sprite file
				DRAW_SPRITE_ANIMATED,			///< Draws the specified sprite file, animated
				NARRATIVE						///< Speech without chat box (thoughts of main character; narrative)
			};

			/// Data structure for decisions
			struct Decision
			{
				std::wstring name;							///< Name of decision (for scripts)
				short unsigned int num_of_choices;			///< Number of possible choices
				std::vector<std::wstring> choices;			///< Array of choice names (for scripts)
				short int selectedOption;					///< The index from \a choices of the choice that the player selected
				Decision()
				{
					name.clear();
					num_of_choices = 0;
					selectedOption = -1;
				};
				~Decision()
				{
					name.clear();
					num_of_choices = 0;
					selectedOption = -1;
					choices.clear();
				};
			};

			static const unsigned short int DARKEN_SCREEN_OPACITY = 100;	///< Current opacity of the darken screen effect

			static std::string currentLine;									///< Current line from script, loaded into string
			static std::wstring currentLine_w;								///< Current line from script, loaded into wide string
			static int currentLineIndex;									///< Current line index in \a currentScript
			static LINETYPE currentLineType;
			static std::vector<std::string> currentScript;					///< Active script, loaded each line as an array of strings
			static std::vector<std::wstring> currentScript_w;				///< Active script, loaded each line as an array of wide strings
			static std::string currentScriptFileName;						///< Active script's filename
			static int activeSpeakerIndex;									///< The index of the active speaker, for chat box names
			static SDL_Surface *currentLineSurface;							///< Current line surface, for drawing the text off-screen
			static SDL_Surface *currentLineSurfaceShadow;					///< Current line surface, for drawing the text shadow off-screen
			static SDL_Texture *currentLineTexture;							///< Current line texture, for drawing the text off-screen
			static SDL_Texture *currentLineTextureShadow;					///< Current line texture, for drawing the text shadow off-screen
			static SDL_Rect currentLineTextureShadowRect;
			static SDL_Rect currentLineTextureRect;

			static float fontSize;											///< The size of the text font; not sure if still used?

			static std::vector<Decision> gameDecisions;						///< Array of game decisions
		
			/// Loads the decisions file into \a gameDecisions array
			/// \deprecated This is unnecessary since decisions are loaded from script into memory, then stored in savegame for later use.
			/// \param _filename File path/name of decisions file
			/// \return Error code if problem encountered, 0 if fine
			static short int LoadGameDecisions(const char* _filename);
		
			/// Loads the X and Y position for game text into \a currentLineTextureRect
			/// 
			/// \param _filename File path/name of text box position file
			/// \return Error code if problem encountered, 0 if fine
			static short int LoadTextBoxPosition(const char* _filename);
			
			/// Draws the contents of \a currentLine to screen
			///
			/// \param ScrW Screen width resolution
			/// \param ScrH Screen height resolution
			static void DrawCurrentLine(int ScrW, int ScrH);

			static bool updateCurrentLine;
			
			/// Draws the specified decision options to screen
			///
			/// \param _decision Specified decision
			/// \param ScrW Screen width resolution
			/// \param ScrH Screen height resolution
			static void DrawDecisions(M22Script::Decision* _decision,int ScrW, int ScrH);
			
			/// Changes currentLine to target line index
			///
			/// \param _newLine Index of new target line
			static void ChangeLine(int _newLine);
			
			/// Splits string into parts between specified character into an array
			///
			/// \param txt Target string to split
			/// \param strs Address of string array to split into
			/// \param ch Character to split between
			static unsigned int SplitString(const std::string&, std::vector<std::string>&, char);
		
			/// Splits string into parts between specified character into an array
			///
			/// \param txt Target string to split
			/// \param strs Address of string array to split into
			/// \param ch Character to split between
			static unsigned int SplitString(const std::wstring&, std::vector<std::wstring>&, char);
			
			/// Checks and returns the type of the string from \a LINETYPE
			///
			/// \param _input String to check
			/// \return Type of line as \a LINETYPE enumerator
			static M22Script::LINETYPE CheckLineType(std::wstring);
			
			/// Checks and returns if the character is a colon ( : )
			///
			/// \param _char Character to check
			static bool isColon(int _char);
			
			/// Clears active characters array
			static void ClearCharacters(void);
			
			/// Fades to screen black
			static void FadeToBlack(void);
		
			/// Converts a wstring to a normal string.
			///
			/// \param _input wstring to convert
			inline static std::string to_string(std::wstring _input)
			{
				std::string output;
				std::wstring input = _input;
				output.assign(input.begin(), input.end());
				return output;
			};
		
			/// Converts a wstring to a Uint16 array (for SDL_TTF)
			///
			/// \param _wstr wstring to convert
			/// \param _size Reference to size of array variable
			inline static Uint16* to_Uint16(std::wstring _wstr, int &_size)
			{
				if(_wstr.length() != 0)
				{
					Uint16* stext = new Uint16[_wstr.length()+1];
					_size = _wstr.length()+1;
					for (size_t i = 0; i < _wstr.length(); ++i) {
						stext[i] = _wstr.at(i); 
					}
					stext[_wstr.length()] = '\0';
					return stext;
				} 
				else 
				{
					return NULL;
				};
			};
		
			/// Converts a wstring to a Uint16 array (for SDL_TTF)
			///
			/// \param _wstr wstring to convert
			inline static Uint16* to_Uint16(std::wstring _wstr)
			{
				if(_wstr.length() != 0)
				{
					Uint16* stext = new Uint16[_wstr.length()+1];
					for (size_t i = 0; i < _wstr.length(); ++i) {
						stext[i] = _wstr.at(i); 
					}
					stext[_wstr.length()] = '\0';
					return stext;
				} 
				else 
				{
					return NULL;
				};
			};
		
			/// Converts a c-string to a wstring.
			///
			/// \param _input c-string to convert
			inline static std::wstring to_wstring(const char* _input)
			{
				std::wstring output;
				std::string input = _input;
				output.assign(input.begin(), input.end());
				return output;
			};
		
			/// Converts a string to a wstring.
			///
			/// \param _input string to convert
			inline static std::wstring to_wstring(std::string _input)
			{
				std::wstring output;
				output.assign(_input.begin(), _input.end());
				return output;
			};
	};

	/// \class 		M22ScriptCompiler M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for loading then compiling script files
	///
	class M22ScriptCompiler
	{
	private:

	public:
	
		/// Structure for compiled lines of M22Script
		struct line_c
		{
			M22Script::LINETYPE m_lineType;					///< The type of line
			std::vector<int> m_parameters;					///< The parameters, if the linetype is not speech or narrative
			std::vector<std::string> m_parameters_txt;		///< The parameters in string format, for loading Lua or M22Scripts
			std::wstring m_lineContents;					///< The speech, if the linetype is speech or narrative
			int m_speaker;									///< Who's speaking, if the linetype is speech
			int m_ID;										///< ID of whatever the linetype is (e.g. if LINETYPE is DrawBackground, then it's the ID of the background)
			line_c()
			{
				m_speaker,m_ID = 0;
				m_lineType = M22Script::SPEECH;
			};
			~line_c()
			{
				m_parameters.clear();
				m_lineContents.clear();
			};
		};

		struct script_checkpoint
		{
			int m_position;
			std::string m_name;
			script_checkpoint()
			{
				m_position = 0;
				m_name = "";
			};
			~script_checkpoint()
			{
				m_name.clear();
				m_position = 0;
			};
		};

		static std::vector<line_c> currentScript_c;																			///< The current script, compiled
		static line_c* CURRENT_LINE;																						///< A pointer to the current line, for shorthand
		static std::vector<script_checkpoint> currentScript_checkpoints;													///< Array of checkpoint positions

		static int CompileLoadScriptFile(std::string _filename);															///< Compiles the specified script file and loads it into currentScript_c
		static int ExecuteCommand(M22ScriptCompiler::line_c _linec, int _line);												///< Runs the specified command
		static int FindCheckpoint(std::string _chkpnt, int &_line);															///< Searches currentScript_checkpoints for the specified checkpoint
		static int CompileLine(M22ScriptCompiler::line_c &tempLine_c, std::vector<std::wstring> CURRENT_LINE_SPLIT);		///< Compiles the parameterised line_c variable
	};

	/// \class 		M22Interface M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for interfaces/mouse input
	///
	/// \details 	This class is responsible for loading, running, displaying and updating interfaces.
	///
	class M22Interface
	{
		private:
		public:
			/// Enumerator for the different states of interface button
			enum BUTTON_STATES
			{
				NOMOUSEOVER,						///< Mouse is over a different button (UNUSED?)
				RESTING,							///< Normal state
				MOUSEOVER,							///< Mouse is over this button
				CLICKED,							///< This button has been clicked
				NUM_OF_BUTTON_STATES
			};
			/// Enumerator for the different interfaces
			enum INTERFACES
			{
				INGAME_INTRFC,						///< Interface for ingame (skip, auto, menu)
				MENU_BUTTON_INTRFC,					///< Interface for ingame menu button (save, load, options)
				MAIN_MENU_INTRFC,					///< Interface for main menu (start, exit, options)
				OPTIONS_MENU_INTRFC,				///< Interface for options menu (exit to title, exit game, etc.)
				NUM_OF_INTERFACES
			};
		
			/// Data structure for a button on an interface
			struct Button
			{
				std::string name;						///< Name of button
				BUTTON_STATES state;					///< State of button
				SDL_Texture* sheet;						///< Sprite sheet for button
				SDL_Rect rectSrc[NUM_OF_BUTTON_STATES];	///< Where the current sprite is on the spritesheet
				SDL_Rect rectDst[NUM_OF_BUTTON_STATES];	///< Where to draw the sprite
				Button()
				{
					state = RESTING;
					sheet = NULL;
				};
			};

			/// Data structure for an interface
			struct Interface
			{
				std::vector<M22Interface::Button> buttons;		///< Array of buttons in the interface
				SDL_Texture* spriteSheet;						///< Background of interface (optional)
				float alpha;									///< Current alpha
				M22Interface::INTERFACES type;					///< Type of interface
			
				Interface()
				{
					alpha = 0.0f;
				};
			
				/// Fades in all buttons using \a Lerp and \a alpha, usually used for main menu
				void Interface::FadeInAllButtons(void)
				{
					this->alpha = M22Graphics::Lerp( this->alpha, 255.0f, DEFAULT_LERP_SPEED/4 );
					for(size_t i = 0; i < this->buttons.size(); i++)
					{
						SDL_SetTextureAlphaMod( this->buttons[i].sheet, Uint8(this->alpha) );
					};
				};
			
			};
		
			static std::vector<Interface> storedInterfaces;			///< Array of loaded interfaces
			static std::vector<Interface*> activeInterfaces;		///< Array of pointers to interfaces to draw/update
			static M22Interface::BUTTON_STATES* skipButtonState;	///< Current state of skip button
			static M22Interface::BUTTON_STATES* menuButtonState;	///< Current state of menu button
			static SDL_Texture* ChatBoxRenderer;					///< Render target for drawing speech box off-screen

			static bool menuOpen;									///< Is the menu open?

			/// Draws all interfaces in \a activeInterfaces
			static void DrawActiveInterfaces(void);
			
			/// Draws the buttons of the active interfaces
			static void DrawActiveInterfacesButtons(void);
			
			static bool DRAW_TEXT_AREA;								///< Draw the text area?
			
			/// Draws the text box, name of person talking, current line; off-screen then into the main renderer
			/// \param _ScrSizeX Screen width
			/// \param _ScrSizeY Screen height
			static void DrawTextArea(int, int);
			
			/// Updates active interfaces, checking if they've been clicked, etc.
			/// \param _ScrSizeX Screen width
			/// \param _ScrSizeY Screen height
			static void UpdateActiveInterfaces(int _ScrSizeX, int _ScrSizeY);
			
			/// Checks if a single point is within a box
			/// \param _pos1 Single point
			/// \param _pos2 Box position
			/// \param _size Size of box
			/// \return True if overlap, false if not
			static bool CheckOverlap(Vec2 _pos1, Vec2 _pos2, Vec2 _size);
			
			/// Resets all stored (and by extension, active) interfaces to default settings
			static void ResetStoredInterfaces(void);

			/// Initializes the text box and sprites
			static void InitTextBox(void);

			/// Initializes an interface from buttons file + constants
			///
			/// \param _interface Pointer to interface to initialize
			/// \param _num_of_buttons Number of buttons in interface
			/// \param _startline Line to start on in buttons file
			/// \param _filename File path/name of buttons file
			/// \param _opaque Is the interface opaque to begin with?
			/// \param _type Type of interface from \a M22Interface::INTERFACES
			/// \return Error code if problem encountered, 0 if fine
			static short int InitializeInterface(M22Interface::Interface* _interface, int _num_of_buttons, int _startline = 0, const std::string _filename = "graphics/interface/GAME_BUTTONS.txt", bool _opaque = true, M22Interface::INTERFACES _type = M22Interface::INTERFACES::INGAME_INTRFC);
	};

	/// \class 		M22Renderer M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for SDL wrapper
	///
	/// \details 	This class is responsible for wrapping SDL functions, so that we can change to another renderer easier
	///
	class M22Renderer
	{
		private:
		public:
			/// Renders current buffer
			static void RenderPresent(void);
		
			/// Clears the current buffer
			static void RenderClear(void);

			static SDL_Window* SDL_SCREEN;							///< Context for the SDL Window
			static SDL_Renderer *SDL_RENDERER;						///< Context for the SDL Renderer
		
			/// Copys the entire background texture to the current frame
			///
			/// \param _bg The M22Engine::Background object to render
			static void RenderCopy(M22Engine::Background _bg);
		
			/// Gets texture width/height
			///
			/// \param _txtr The SDL_Texture to check
			/// \param _w Reference to variable to store width in
			/// \param _h Reference to variable to store height in
			static void GetTextureInfo(SDL_Texture* _txtr, int& _w, int& _h);
		
			/// Loads the texture from the file and returns the loaded texture
			///
			/// \param _txtr The SDL_Texture to check
			static SDL_Texture* LoadTexture(std::string _filepath);
		
			/// Sets the active draw color (values between 0 and 255)
			///
			/// \param _r Red
			/// \param _g Green
			/// \param _b Blue
			/// \param _a Alpha
			static void M22Renderer::SetDrawColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a);

			/// Delays the thread for the specified time
			///
			/// \param _delay Time in milliseconds to delay by
			static void M22Renderer::Delay(unsigned int _delay);
	};

	/// \class 		M22Lua M22Engine.h "include/M22Engine.h"
	/// \brief 		Class for Lua engine
	///
	/// \details 	This class is responsible for wrapping Lua around the engine
	///
	class M22Lua
	{
		private:
		public:
			static lua_State *STATE;
			static int M22Lua::Initialize();
			static void M22Lua::Shutdown();
			static int M22Lua::ExecuteM22ScriptCommand(lua_State*);
	};

};
#endif