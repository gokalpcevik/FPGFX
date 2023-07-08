#pragma once
#include <vector>
#include <memory>

#include "Log.h"
#include "MathTypes.h"
#include "Types.h"

// Assimp
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

namespace fpgfx
{
    struct Vertex3
    {
        Vec3f Position;
        Vec3f Normal;
        Vec2f UV;
        Vec3f Color;
    };

    using VertexArray = std::vector<Vertex3>;
    using IndexArray = std::vector<u32>;

    struct Mesh
    {
        Mesh() = default;

        VertexArray Vertices{};
        IndexArray Indices{};
        sz_t VertexCnt;
        sz_t IndexCnt;

        static constexpr u32 Stride = (u32)sizeof(Vertex3);
    };

    using MeshRef = std::shared_ptr<Mesh>;

    MeshRef ImportMesh(char const* path);
}