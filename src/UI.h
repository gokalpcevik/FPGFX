#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

namespace fpgfx
{
    void InitImGui(SDL_Window* pWindow, SDL_Renderer* pRenderer);   
    void OnRenderUI(SDL_Window* pWindow, SDL_Renderer* pRenderer);
}