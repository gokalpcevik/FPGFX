#pragma once
#include <Eigen/Eigen>

namespace fpgfx
{
    using Color3i = Eigen::Vector3i;
    using Color3f = Eigen::Vector3f;

    using Vec2i = Eigen::Vector2i;
    using Vec3i = Eigen::Vector3i;
    using Vec4i = Eigen::Vector4i;
    
    using Vec2f = Eigen::Vector2f;
    using Vec3f = Eigen::Vector3f;
    using Vec4f = Eigen::Vector4f;

    using Mat3f = Eigen::Matrix3f;
    using Mat4f = Eigen::Matrix4f;

    using Affine3f = Eigen::Affine3f;
    using Projective3f = Eigen::Projective3f;
    using AngleAxisf = Eigen::AngleAxisf;

    //
    //  + : Origin
    //  
    //  +-------------------
    //  |                   |
    //  |                   | 
    //  |       Size[1]<--- | 
    //  |                   | 
    //  |                   | 
    //   -------------------
    //  <------------------->
    //      Size[0]
    //
    struct BoundingBox
    {
        Vec2f Origin;
        Vec2f Size;
    };
}