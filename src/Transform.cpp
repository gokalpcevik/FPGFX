#include "Transform.h"

namespace fpgfx
{
    using namespace Eigen;

    Projective3f Perspective(f32 NearZ, f32 FarZ, f32 FOVY_Half, f32 AspectRatio)
    {
        CanvasDim dim = CalculateCanvasDim(FOVY_Half, AspectRatio, NearZ);

        // TODO: this can be optimized a lot more.
        Projective3f proj(Mat4f::Zero());
        Mat4f &mat = proj.matrix();
        mat(0, 0) = 2.0f * NearZ / (dim.Right - dim.Left);
        mat(1, 1) = 2.0f * NearZ / (dim.Top - dim.Bottom);
        mat(0, 2) = (dim.Right + dim.Left) / (dim.Right - dim.Left);
        mat(1, 2) = (dim.Top + dim.Bottom) / (dim.Top - dim.Bottom);
        mat(2, 2) = -(FarZ + NearZ) / (FarZ - NearZ);
        mat(3, 2) = -1.0f;
        mat(2, 3) = -2.0f * FarZ * NearZ / (FarZ - NearZ);

        return proj;
    }

    CanvasDim CalculateCanvasDim(f32 FOVY_Half, f32 AspectRatio, f32 NearZ)
    {
        CanvasDim dim;
        f32 CanvasHeight = 2.0f * std::tanf(FOVY_Half) * NearZ;
        f32 CanvasWidth = CanvasHeight * AspectRatio;

        dim.Top = CanvasHeight / 2.0f;
        dim.Bottom = -dim.Top;

        dim.Right = CanvasWidth / 2.0f;
        dim.Left = -dim.Right;

        return dim;
    }

    Affine3f CameraLookAtRH(Vec3f const &From, Vec3f const &To, Vec3f const &WorldUp)
    {
        Vec3f Forward = (From - To).normalized();
        Vec3f Right = WorldUp.cross(Forward).normalized();
        Vec3f Up = Forward.cross(Right);

        Affine3f CameraTF = Affine3f::Identity();
        Mat4f &CameraMatrix = CameraTF.matrix();
        CameraMatrix.col(0).head<3>() << Right;
        CameraMatrix.col(1).head<3>() << Up;
        CameraMatrix.col(2).head<3>() << Forward;
        CameraMatrix.col(3) << From.homogeneous();
        return (CameraTF);
    }
}