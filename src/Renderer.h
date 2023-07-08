#pragma once
#include <cassert>
#include <thread>
#include <mutex>

#include "Eigen/Eigen"
#include "SDL2/SDL.h"

#include "Transform.h"
#include "MathTypes.h"
#include "Mesh.h"

#include "Types.h"
#include "Definitions.h"
#include "Log.h"

namespace fpgfx
{
    // Color buffer width
    static constexpr u32 COLORBUFFER_WIDTH = 1600UL;
    // For fast typing
    static constexpr u32 CBF_W = COLORBUFFER_WIDTH;

    // Color buffer height
    static constexpr u32 COLORBUFFER_HEIGHT = 900UL;
    // For fast typing
    static constexpr u32 CBF_H = COLORBUFFER_HEIGHT;

    // Color buffer size(pixels)
    static constexpr sz_t CBF_SIZE_PX = COLORBUFFER_WIDTH * COLORBUFFER_HEIGHT;

    // Color buffer size(bytes)
    static constexpr sz_t CBF_SIZE = COLORBUFFER_WIDTH * COLORBUFFER_HEIGHT * 4ULL;

    static constexpr float ASPECT_RATIO = 16.0f / 9.0f;

    constexpr static u32 RGBA(u8 R, u8 G, u8 B, u8 Alpha = 0xFF)
    {
        return (B) | (G << 8u) | (R << 16u) | (Alpha << 24u);
    }

    struct RenderContext
    {
        SDL_Renderer *Renderer = nullptr;
        SDL_Texture *RenderTexture = nullptr;
        u32 *ColorBuffer;
        f32 *DepthBuffer;
        std::mutex DrawingMutex;
    };

    struct Vertex
    {
        // We don't do depth for now
        Vec2f Position;
        Color3f Color;
    };

    /// @brief Initializes the render context
    /// @param pCtx Render Context
    /// @param pWindow Associated Window
    void InitRenderContext(RenderContext *pCtx, SDL_Window *pWindow);

    void CleanupRenderContext(RenderContext *pCtx);

    /// @brief Clears the color buffer. Not the color buffur texture.
    void Clear(RenderContext *pCtx);

    /// @brief Copies the color buffer to the color buffer texture, updates the render target texture of the
    /// renderer with the color buffer texture.
    void Display(RenderContext *pCtx);

    float EdgeFunction(Vec2f const &V0, Vec2f const &V1, Vec2f const &Point);

    bool IsInside(Vec2f const &V0, Vec2f const &V1, Vec2f const &V2, Vec2f const &Point);

    BoundingBox ComputeMinBoundingBox(Vec2f const &V0, Vec2f const &V1, Vec2f const &V2);

    /// @brief Puts a pixel with the given color at (x,y). Thread-safe.
    void PutPixel(RenderContext *pCtx, Vec2i const &Coord, u32 Color = RGBA(255, 255, 255));

    /// @brief Draw a filled triangle given three coordinates.
    /// @note The triangle winding order needs to be CCW or the triangle won't be visible.
    void DrawTriangle(RenderContext *pCtx, Vec2f const &V0, Vec2f const &V1, Vec2f const &V2, u32 Color = RGBA(255, 255, 255));
    /// @brief Draw a filled triangle given three coordinates.
    /// @note The triangle winding order needs to be CCW or the triangle won't be visible.
    void DrawTriangle(RenderContext *pCtx, Vertex const &V0, Vertex const &V1, Vertex const &V2);

    /// @brief Draws a line from start to the end with the given color.
    /// @param Start Start point of the line
    /// @param End End point of the line
    void DrawLine(RenderContext *pCtx, Vec2i const &Start, Vec2i const &End, u32 Color = RGBA(255, 255, 255));

    /// @brief Draw a filled rectangle.
    void DrawRect(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u32 Color = RGBA(255, 255, 255));

    /// @brief Draw a rectangle.
    void DrawRectOutline(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u32 Color = RGBA(255, 255, 255));

    /// @brief Draws a filled grayscale rectangle.
    /// @param Color Grayscale Value (0-255)
    void DrawRectGS(RenderContext *pCtx, Vec2i const &Origin, u32 Width, u32 Height, u8 Color = 255u);
}