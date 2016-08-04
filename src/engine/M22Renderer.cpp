/// \file 		M22Renderer.cpp
/// \author 	Sam Lynch
/// \brief 		SDL wrapper for the March22 engine
/// \version 	0.6.0
/// \date 		June 2016

#include <engine/M22Engine.h>

using namespace March22;


SDL_Window* M22Renderer::SDL_SCREEN = NULL;
SDL_Renderer *M22Renderer::SDL_RENDERER = NULL;

void M22Renderer::RenderPresent(void)
{
	SDL_RenderPresent(M22Renderer::SDL_RENDERER);
	return;
};

void M22Renderer::RenderCopy(M22Engine::Background _bg)
{
	SDL_RenderCopy(M22Renderer::SDL_RENDERER, _bg.sprite			, NULL, NULL);
	return;
};

void M22Renderer::RenderClear(void)
{
	SDL_RenderClear(M22Renderer::SDL_RENDERER);
	return;
};

void M22Renderer::GetTextureInfo(SDL_Texture* _txtr, int& _w, int& _h)
{
	SDL_QueryTexture(_txtr, NULL, NULL, &_w, &_h);
	return;
};

SDL_Texture* M22Renderer::LoadTexture(std::string _filepath)
{
	return IMG_LoadTexture(M22Renderer::SDL_RENDERER, _filepath.c_str());
};

void M22Renderer::SetDrawColor(unsigned char _r, unsigned char _g, unsigned char _b, unsigned char _a)
{
	SDL_SetRenderDrawColor(M22Renderer::SDL_RENDERER, _r, _g, _b, _a);
	return;
};

void M22Renderer::Delay(unsigned int _delay)
{
	SDL_Delay(_delay);
	return;
};