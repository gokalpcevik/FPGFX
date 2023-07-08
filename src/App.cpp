#include "App.h"

namespace fpgfx
{
    RenderContext ctx;
    TimeStep Ts;

    MeshRef sphere;

    // Declare
    void OnRender(RenderContext *pCtx);

    f32 TimeStep::GetFPS() const
    {
        return 1.0f / ((f32)FrameTime / (f32)SDL_GetPerformanceFrequency());
    }

    f32 TimeStep::GetFrameTime() const
    {
        return (f32)FrameTime / (f32)SDL_GetPerformanceFrequency() * 1000.0f;
    }

    f32 TimeStep::GetFrameTimeSeconds() const
    {
        return (f32)FrameTime / (f32)SDL_GetPerformanceCounter();
    }

    int Start()
    {
        Log::Init();

        // Create window
        GetApp().pWindow = SDL_CreateWindow(
            "FPGX",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            COLORBUFFER_WIDTH,
            COLORBUFFER_HEIGHT,
            SDL_WINDOW_ALLOW_HIGHDPI);

        // Check window creation
        if (App::GetWindow() == nullptr)
        {
            fpgfx_error("Window cannot be created.");
            return EXIT_FAILURE;
        }

        fpgfx_info("Window has been created.");

        InitRenderContext(&ctx, App::GetWindow());
        InitImGui(App::GetWindow(), ctx.Renderer);

        GetApp().pScene = new Scene();

        sphere = ImportMesh("untitled.fbx");

        Entity e = GetApp().pScene->CreateEntity();

        e.AddComponent<MeshComponent>(sphere);
        auto& tc = e.GetComponent<TransformComponent>();
     
        // Update
        while (GetApp().IsRunning)
        {
            tc.Angle += 0.01f;
            fpgfx::Update();

            auto const counter = SDL_GetPerformanceCounter();
            Ts.FrameTime = counter - Ts.LastTickCount;
            Ts.LastTickCount = counter;
            ++Ts.UpdateCounter;
        }

        return EXIT_SUCCESS;
    }

    void Update()
    {
        SDL_Event e;

        // Handle events
        while (SDL_PollEvent(&e) != 0)
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
            HandleQuitEvents(&e);
        }

        ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        // Clear the color buffer
        Clear(&ctx);

        OnRender(&ctx);
        ImGui::Render();

        // Copy RT and Display
        Display(&ctx);
    }

    void Shutdown()
    {
        CleanupRenderContext(&ctx);
        delete GetApp().pScene;
        GetApp().IsRunning = false;
    }

    void HandleQuitEvents(SDL_Event *e)
    {
        // Quit Event
        if (e->type == SDL_QUIT)
            fpgfx::Shutdown();
    }

    Affine3f ViewMatrix = CameraLookAtRH({+0.0f,+5.0f,+5.0f},Vec3f::Zero()).inverse();
    Projective3f ProjectionMatrix = Perspective(0.1f,100.0f,(f32)M_PI / 4.0f, ASPECT_RATIO);

    void OnRender(RenderContext *pCtx)
    {
        Scene* pScene = GetApp().pScene;

        entt::registry& Reg = pScene->m_Registry;
        
        auto view = Reg.view<TransformComponent, MeshComponent>();

        for(auto e : view)
        {
            auto&&[TC, MC]= view.get<TransformComponent, MeshComponent>(e);
            MeshRef const& pMesh = MC.MeshPtr;

            for(sz_t i = 0; i < pMesh->IndexCnt-2; i+=3)
            {
                Vertex3& V0 = pMesh->Vertices[pMesh->Indices[i+0]];
                Vertex3& V1 = pMesh->Vertices[pMesh->Indices[i+1]];
                Vertex3& V2 = pMesh->Vertices[pMesh->Indices[i+2]];
            
                // Vertex Positions in World Frame
                Vec3f V0_Pw = TC.GetTransform() * V0.Position;
                Vec3f V1_Pw = TC.GetTransform() * V1.Position;
                Vec3f V2_Pw = TC.GetTransform() * V2.Position;

                // Transform to camera space
                V0_Pw = ViewMatrix * V0_Pw;
                V1_Pw = ViewMatrix * V1_Pw;
                V2_Pw = ViewMatrix * V2_Pw;

                // Transform to Homogenous Clip Space
                Vec4f V0_Pc = ProjectionMatrix * V0_Pw.homogeneous();
                Vec4f V1_Pc = ProjectionMatrix * V1_Pw.homogeneous();
                Vec4f V2_Pc = ProjectionMatrix * V2_Pw.homogeneous();

                // TODO: A clipping stage should be added here. ( Against the cube with side lengths w = -z)

                // NDC Space ( Perspective division )
                Vec3f P0 = V0_Pc.head<3>() / V0_Pc[3];
                Vec3f P1 = V1_Pc.head<3>() / V1_Pc[3];
                Vec3f P2 = V2_Pc.head<3>() / V2_Pc[3];

                // Viewport space, ignore depth for now
                P0 = P0 / 2.0f + Vec3f{0.5f,0.5f,0.0f};
                P1 = P1 / 2.0f + Vec3f{0.5f,0.5f,0.0f};
                P2 = P2 / 2.0f + Vec3f{0.5f,0.5f,0.0f};
                
                // Raster space, TODO: change DrawTriangle to take in raster space coordinates
                Vec2f P0_raster = { (P0[0] * (f32)COLORBUFFER_WIDTH) , (P0[1] * (f32)COLORBUFFER_HEIGHT) };
                Vec2f P1_raster = { (P1[0] * (f32)COLORBUFFER_WIDTH) , (P1[1] * (f32)COLORBUFFER_HEIGHT) };
                Vec2f P2_raster = { (P2[0] * (f32)COLORBUFFER_WIDTH) , (P2[1] * (f32)COLORBUFFER_HEIGHT) };
                // All three vertices of the same triangle has the same color
                Vec3i RGB = (pMesh->Vertices[pMesh->Indices[i]].Color * 255.0f).cast<s32>();
                u32 Color = RGBA(RGB[0],RGB[1],RGB[2]); // A=0xFF

                DrawTriangle(pCtx,P0_raster,P1_raster,P2_raster,Color);
                //DrawLine(pCtx,P0_raster,P1_raster);
                //DrawLine(pCtx,P1_raster,P2_raster);
                //DrawLine(pCtx,P2_raster,P0_raster);
            }
        }
    }

    
}