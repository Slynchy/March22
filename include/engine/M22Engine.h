#ifndef M22ENGINE_H
#define M22ENGINE_H

/*
	RENDERER == SDL_RENDERER
	Shorthand
*/
#define RENDERER SDL_RENDERER

#define DEFAULT_MUSIC_VOLUME_MULT 0.25f
#define DEFAULT_SFX_VOLUME_MULT 0.05f
#define DEFAULT_LERP_SPEED 0.15f

#include "SDL.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "engine/Vectors.h"
#include <vector>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <string>

class M22Engine
{
	private:
	public:
		struct Character
		{
			std::string name;
			std::vector<std::vector<SDL_Texture*>> sprites;
			std::vector<std::string> emotions;
			std::vector<std::string> outfits;
		};
		struct CharacterReference
		{
			SDL_Texture* sprite;
			SDL_Rect rect;
			float alpha;
			bool clearing;
			CharacterReference()
			{
				alpha = 0.0f;
				clearing = false;
				sprite = NULL;
			};
		};
		struct Background
		{
			SDL_Texture* sprite;
			float alpha;
		};
		enum GAMESTATES
		{
			MAIN_MENU,
			INGAME
		};
		enum CHARACTERS
		{
			NONE,
			YUUJI,
			GIRL,
			TOTAL_CHARACTERS
		};
		
		static Vec2 MousePos;
		static bool LMB_Pressed;

		static int GetCharacterIndexFromName(std::string);
		static int GetOutfitIndexFromName(std::string, int);
		static int GetEmotionIndexFromName(std::string, int _charIndex);

		static std::vector<std::string> CHARACTER_NAMES;
		static unsigned short int ACTIVE_BACKGROUND_INDEX;
		static std::vector<Background> ACTIVE_BACKGROUNDS;

		static std::vector<Character> CHARACTERS_ARRAY;
		static std::vector<std::string> CHARACTER_EMOTIONS;

		static M22Engine::GAMESTATES GAMESTATE;
		static SDL_Window* SDL_SCREEN;
		static SDL_Renderer *SDL_RENDERER;
		static SDL_Event SDL_EVENTS;
		static const Uint8 *SDL_KEYBOARDSTATE;

		static short int InitializeM22(int ScrW, int ScrH);
};

class M22Graphics
{
	private:
	public:
		struct ArrowObj
		{
			SDL_Texture* sprite;
			float frame;
			ArrowObj()
			{
				sprite = NULL;
				frame = 0.0f;
			};
		};
		static SDL_Texture* textFrame;
		static ArrowObj arrow;
		static std::vector<SDL_Texture*> characterFrameHeaders;
		static std::vector<M22Engine::CharacterReference> activeCharacters;
		static std::vector<SDL_Texture*> mainMenuBackgrounds;
		static M22Engine::Background activeMenuBackground;
		static M22Engine::Background menuLogo;

		static std::vector<SDL_Texture*> BACKGROUNDS;
		static std::vector<std::string> backgroundIndex;
		static short int LoadBackgroundsFromIndex(const char* _filename);
		static void DrawBackground(SDL_Texture* _target);
		static void UpdateBackgrounds(void);
		static void UpdateCharacters(void);

		static void DrawArrow(int ScrW, int ScrH);

		static float Lerp(float _var1, float _var2, float _t); 

		static TTF_Font *textFont;
};

class M22Sound
{
	private:
	public:
		enum MIXERS
		{
			BGM,
			SFX,
			VOICE
		};
		static std::vector<Mix_Chunk*> SOUND_FX;
		static std::vector<Mix_Music*> MUSIC;
		static std::vector<std::string> MUSIC_NAMES;
		static std::vector<std::string> SFX_NAMES;
		static float MUSIC_VOLUME;
		static float SFX_VOLUME;
		static int currentTrack;

		static short int PlaySting(short int);
		static short int PlaySting(short int, bool);
		static short int PlaySting(std::string, bool);

		static short int ChangeMusicTrack(short int _position);
		static short int ChangeMusicTrack(std::string _name);
		static void StopMusic();
		static void PauseMusic();
		static void ResumeMusic();
		static short int StartMusic(short int _position);
};

class M22Script
{
	private:
	public:
		enum LINETYPE
		{
			NEW_BACKGROUND,
			FADE_TO_BLACK,
			NEW_MUSIC,
			STOP_MUSIC,
			PLAY_STING,
			DRAW_CHARACTER,
			CLEAR_CHARACTERS,
			LOAD_SCRIPT,
			SPEECH,
			NARRATIVE
		};

		static std::string currentLine;
		static int currentLineIndex;
		static std::vector<std::string> currentScript;
		static int activeSpeakerIndex;
		static SDL_Surface *currentLineSurface;
		static SDL_Surface *currentLineSurfaceShadow;

		static float fontSize;

		static short int LoadScriptToCurrent(const char* _filename);
		static void DrawCurrentLine(int ScrW, int ScrH);
		static void ChangeLine(int _newLine);
		static unsigned int SplitString(const std::string&, std::vector<std::string>&, char);
		static M22Script::LINETYPE CheckLineType(std::string);
};

class M22Interface
{
	private:
	public:
		struct Button
		{
			bool mouseOver;
			SDL_Texture* sheet;
			SDL_Rect rectSrc;
			SDL_Rect rectDst;
			Button()
			{
				mouseOver = false;
				sheet = NULL;
			};
		};

		struct Interface
		{
			std::vector<M22Interface::Interface> buttons;
			SDL_Texture* spriteSheet;
		};

		static std::vector<Interface> activeInterfaces;
};

#endif