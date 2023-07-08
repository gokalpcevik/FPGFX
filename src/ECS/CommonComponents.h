#pragma once
#include "Eigen/Eigen"
#include "SDL2/SDL.h"
#include <utility>
#include <string>

#include "../Types.h"
#include "../MathTypes.h"
#include "../Mesh.h"

namespace fpgfx
{
    struct TransformComponent
    {
        TransformComponent() = default;
        TransformComponent(Vec3f translation, Vec3f rotationAxis, Vec3f scale)
            : Translation(translation), RotationAxis(rotationAxis), Scale(scale) {}

        Affine3f GetTransform()
        {
            AngleAxisf aa(Angle, RotationAxis);
            Transform.setIdentity();
            return Transform.scale(Scale).rotate(aa).translate(Translation);
        }

        Vec3f Translation = Vec3f::Zero();
        Vec3f RotationAxis = Vec3f::UnitY();
        f32 Angle = 0.0f;
        Vec3f Scale{1.0f, 1.0f, 1.0f};

        Affine3f Transform = Affine3f::Identity();
    };

    struct MeshComponent
    {
        MeshComponent(MeshRef mesh) : MeshPtr(mesh) {}

        u32 Color = 0xFFFFFFFF;
        // SDL_Texture is not used since we'll put pixels by hand.
        SDL_Surface *pTexture = nullptr;
        MeshRef MeshPtr = nullptr;
    };
}
