#pragma once
#include "Types.h"
#include "MathTypes.h"
#include "Definitions.h"
#include "Eigen/Dense"

namespace fpgfx
{
    struct CanvasDim
    {
        f32 Bottom, Top, Left, Right;
    };

    Projective3f Perspective(f32 NearZ, f32 FarZ, f32 FOVY_Half, f32 AspectRatio);

    /// @param FOVY_Half In radians.
    /// @note It is assumed that distance from aperture to the canvas in z-axis is equal to NearZ.
    CanvasDim CalculateCanvasDim(f32 FOVY_Half, f32 AspectRatio, f32 NearZ);

    Affine3f CameraLookAtRH(Vec3f const &From, Vec3f const &To, Vec3f const &WorldUp = {0.0f, 1.0f, 0.0f});

    FP_FORCEINLINE Affine3f Translation(Vec3f const &T)
    {
        return Affine3f(Eigen::Translation3f(T));
    }

    FP_FORCEINLINE Affine3f Translation(f32 Tx, f32 Ty, f32 Tz)
    {
        return Affine3f(Eigen::Translation3f(Tx, Ty, Tz));
    }
}