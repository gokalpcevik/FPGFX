#include "Renderer.h"
#include <sstream>
#include "imgui_impl_sdlrenderer.h"

namespace fpgfx
{
    void InitRenderContext(RenderContext *pCtx, SDL_Window *pWindow)
    {
        assert(pCtx != nullptr && pWindow != nullptr);

        // Allocate new buffer
        pCtx->ColorBuffer = new u32[CBF_SIZE_PX];

        // Zero mem the buffer
        memset(pCtx->ColorBuffer, 0x0, CBF_SIZE);

        // Create renderer
        pCtx->Renderer = SDL_CreateRenderer(pWindow, -1, SDL_RENDERER_PRESENTVSYNC);

        // Check renderer creation
        if (pCtx->Renderer == nullptr)
        {
            fpgfx_error("Renderer cannot be created.");
            throw;
        }

        fpgfx_info("Renderer has been created.");

        // Create the render target texture
        pCtx->RenderTexture = SDL_CreateTexture(pCtx->Renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, CBF_W, CBF_H);

        assert(pCtx->RenderTexture != nullptr);

        // Update the RT texture with the color buffer data
        SDL_UpdateTexture(pCtx->RenderTexture, nullptr, pCtx->ColorBuffer, CBF_W * 4U);
    }

    void CleanupRenderContext(RenderContext *pCtx)
    {
        // assert(pCtx != nullptr);

        {
            std::lock_guard guard(pCtx->DrawingMutex);
            // Delete and nullify the color buffer
            delete[] pCtx->ColorBuffer;
            pCtx->ColorBuffer = nullptr;

            delete[] pCtx->DepthBuffer;
            pCtx->DepthBuffer = nullptr;
        }

        SDL_DestroyRenderer(pCtx->Renderer);
        SDL_DestroyTexture(pCtx->RenderTexture);
    }

    void PutPixel(RenderContext *pCtx, Vec2i const &Coord, u32 Color)
    {
        // assert(pCtx != nullptr);

        // Make sure the coordinates are within bounds
        // assert(Coord[0] < CBF_W && Coord[1] < CBF_H);

        pCtx->ColorBuffer[Coord[1] * CBF_W + Coord[0]] = Color;
    }

    void DrawRect(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u32 Color)
    {
        // assert(pCtx != nullptr);

        u32 X_End = Origin[0] + Width;
        u32 Y_End = Origin[1] + Height;

        // assert((X_End < CBF_W) && (Y_End < CBF_H));

        for (u32 y = Origin[1]; y < Y_End; ++y)
        {
            for (u32 x = Origin[0]; x < X_End; ++x)
            {
                PutPixel(pCtx, {x, y}, Color);
            }
        }
    }

    void DrawRectOutline(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u32 Color)
    {
        // assert(pCtx != nullptr);

        u32 X_End = Origin[0] + Width;
        u32 Y_End = Origin[1] + Height;

        // assert((X_End < CBF_W) && (Y_End < CBF_H));

        for (u32 y = Origin[1]; y < Y_End; ++y)
        {
            if (y == Origin[1] || y == Y_End - 1)
            {
                for (u32 x = Origin[0]; x < X_End; ++x)
                    PutPixel(pCtx, {x, y}, Color);
            }
            else
            {
                PutPixel(pCtx, {Origin[0], y}, Color);
                PutPixel(pCtx, {X_End, y}, Color);
            }
        }
    }

    void DrawRectGS(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u8 Color)
    {
        // assert(pCtx != nullptr);

        u32 X_End = Origin[0] + Width;
        u32 Y_End = Origin[1] + Height;

        // assert((X_End < CBF_W) && (Y_End < CBF_H));

        // Need to lock guard
        for (u32 y = Origin[1]; y < Y_End; ++y)
        {
            memset(&pCtx->ColorBuffer[y * CBF_W], Color, Width * 4U);
        }
    }

    bool IsInside(Vec2f const &V0, Vec2f const &V1, Vec2f const &V2, Vec2f const &Point)
    {
        return (EdgeFunction(V0, V1, Point) <= 0.0f) && (EdgeFunction(V1, V2, Point) <= 0.0f) && (EdgeFunction(V2, V0, Point) <= 0.0f);
    }

    float EdgeFunction(Vec2f const &V0, Vec2f const &V1, Vec2f const &Point)
    {
        Vec2f A = Point - V0;
        Vec2f B = V1 - V0;
        return (A[0] * B[1] - A[1] * B[0]);
    }

    BoundingBox ComputeMinBoundingBox(Vec2f const &V0, Vec2f const &V1, Vec2f const &V2)
    {
        float X_Min = std::min(V0[0], V1[0]);
        float X_Max = std::max(V1[0], V2[0]);

        // Check if the 3rd point's X coord is smaller
        if (V2[0] < X_Min)
            X_Min = V2[0];

        // Check if the first point's X coord is larger
        if (V0[0] > X_Max)
            X_Max = V0[0];

        float Y_Min = std::min(V0[1], V1[1]);
        float Y_Max = std::max(V0[1], V2[1]);

        // Check if the 3rd point's Y coord is smaller
        if (V2[1] < Y_Min)
            Y_Min = V2[1];

        // Check if the first point's Y coord is larger
        if (V1[1] > Y_Max)
            Y_Max = V1[1];

        return BoundingBox{Vec2f{X_Min, Y_Min}, Vec2f{X_Max - X_Min, Y_Max - Y_Min}};
    }

    void DrawTriangle(RenderContext *pCtx, Vertex const &V0, Vertex const &V1, Vertex const &V2)
    {
        BoundingBox BB = ComputeMinBoundingBox(V0.Position, V1.Position, V2.Position);

        Vec2f End = BB.Origin + BB.Size;

        Color3f C10 = V1.Color - V0.Color;
        Color3f C20 = V2.Color - V0.Color;

        for (u32 y = BB.Origin[1]; y < End[1]; ++y)
        {
            for (u32 x = BB.Origin[0]; x < End[0]; ++x)
            {
                float E01 = EdgeFunction(V0.Position, V1.Position, {x, y});
                float E12 = EdgeFunction(V1.Position, V2.Position, {x, y});
                float E20 = EdgeFunction(V2.Position, V0.Position, {x, y});

                if (E01 <= 0.0f && E12 <= 0.0f && E20 <= 0.0f)
                {
                    float Area = EdgeFunction(V0.Position, V1.Position, V2.Position);
                    float Lambda0 = E12 / Area;
                    float Lambda1 = E20 / Area;
                    float Lambda2 = E01 / Area;

                    Color3i Color = ((V0.Color + Lambda1 * C10 + Lambda2 * C20) * 255.0f).cast<s32>();
                    PutPixel(pCtx, {x, COLORBUFFER_HEIGHT - y}, RGBA(Color[0], Color[1], Color[2]));
                }
            }
        }
    }

    void DrawTriangleInterp(RenderContext *pCtx, Vec4f const &V0, Vec4f const &V1, Vec4f const &V2, Vec3f const &C0, Vec3f const &C1, Vec3f const &C2)
    {
        BoundingBox BB = ComputeMinBoundingBox(V0.head<2>(), V1.head<2>(), V2.head<2>());

        Vec2f End = BB.Origin + BB.Size;

        float D10 = C1[2] - C0[2];
        float D20 = C2[2] - C0[2];

        float W10 = V1[3] - V0[3];
        float W20 = V2[3] - V0[3];

        for (u32 y = BB.Origin[1]; y < End[1]; ++y)
        {
            for (u32 x = BB.Origin[0]; x < End[0]; ++x)
            {
                float E01 = EdgeFunction(V0.head<2>(), V1.head<2>(), {x, y});
                float E12 = EdgeFunction(V1.head<2>(), V2.head<2>(), {x, y});
                float E20 = EdgeFunction(V2.head<2>(), V0.head<2>(), {x, y});

                if (E01 <= 0.0f && E12 <= 0.0f && E20 <= 0.0f)
                {
                    float Area = EdgeFunction(V0.head<2>(), V1.head<2>(), V2.head<2>());
                    float Lambda0 = E12 / Area;
                    float Lambda1 = E20 / Area;
                    float Lambda2 = E01 / Area;

                    float OneOverPwInterp = V0[3] + Lambda1 * W10 + Lambda2 * W20;

                    float Depth = C0[2] + Lambda1 * D10 + Lambda2 * D20;
                    Depth = Depth / OneOverPwInterp;

                    PutPixel(pCtx, {x, COLORBUFFER_HEIGHT - y}, RGBA(Depth, Depth, Depth));
                }
            }
        }
    }

    void DrawTriangle();

    void DrawTriangle(RenderContext *pCtx, Vec2f const &V0, Vec2f const &V1, Vec2f const &V2, u32 Color)
    {
        BoundingBox BB = ComputeMinBoundingBox(V0, V1, V2);

        Vec2f End = BB.Origin + BB.Size;
        for (u32 y = BB.Origin[1]; y < End[1]; ++y)
        {
            for (u32 x = BB.Origin[0]; x < End[0]; ++x)
            {
                if (IsInside(V0, V1, V2, {x, y}))
                {
                    PutPixel(pCtx, {x, y}, Color);
                }
            }
        }
    }

    void DrawLine(RenderContext *pCtx, Vec2i const &Start, Vec2i const &End, u32 Color)
    {
        int x0 = Start[0];
        int y0 = Start[1];
        int x1 = End[0];
        int y1 = End[1];

        int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
        int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
        int err = dx + dy, e2; /* error value e_xy */

        for (;;)
        { /* loop */
            PutPixel(pCtx, Vec2i{x0, y0}, Color);
            e2 = 2 * err;
            if (e2 >= dy)
            { /* e_xy+e_x > 0 */
                if (x0 == x1)
                    break;
                err += dy;
                x0 += sx;
            }
            if (e2 <= dx)
            { /* e_xy+e_y < 0 */
                if (y0 == y1)
                    break;
                err += dx;
                y0 += sy;
            }
        }
    }

    void Clear(RenderContext *pCtx)
    {
        std::memset(pCtx->ColorBuffer, 0x0, CBF_SIZE);
    }

    void Display(RenderContext *pCtx)
    {
        // std::lock_guard guard(pCtx->DrawingMutex);

        SDL_UpdateTexture(pCtx->RenderTexture, nullptr, pCtx->ColorBuffer, CBF_W * 4ULL);

        // Copy the color buffer texture
        SDL_RenderCopy(pCtx->Renderer, pCtx->RenderTexture, nullptr, nullptr);

        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        // Present
        SDL_RenderPresent(pCtx->Renderer);
    }

}