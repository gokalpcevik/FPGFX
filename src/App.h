#pragma once
#include <iostream>
#include <random>

#include "SDL2/SDL.h"
#include "Log.h"
#include "Types.h"
#include "Renderer.h"
#include "UI.h"
#include "Format.h"
#include "Mesh.h"
#include "ECS/Scene.h"
#include "ECS/Entity.h"

namespace fpgfx
{
    struct TimeStep
    {
        Uint64 LastTickCount = 0ULL;
        Uint64 FrameTime = 0ULL;
        uint64_t UpdateCounter = 0ULL;

        f32 GetFPS() const;
        f32 GetFrameTime() const;
        f32 GetFrameTimeSeconds() const;
    };

    struct App
    {
        static App &Get()
        {
            static App s;
            return s;
        }
        App(const App &) = delete;
        App &operator=(const App &) = delete;

    public:
        static SDL_Window *GetWindow() { return App::Get().pWindow; }
        
        Scene* pScene = nullptr;
        SDL_Window *pWindow = nullptr;
        bool IsRunning = true;

    private:
        App() {}
        ~App() {}
    };

    static App &GetApp() { return App::Get(); }

    int Start();
    void Update();
    void Shutdown();
    void HandleQuitEvents(SDL_Event *e);

}