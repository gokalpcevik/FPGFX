#include "Mesh.h"
#include <random>

namespace fpgfx
{
    std::random_device rd;

    float RandomFloat()
    {
		std::mt19937_64 mte(rd());
		std::uniform_real_distribution<float> dist(0.1f,1.0f);
		return dist(mte);
    }


    MeshRef ImportMesh(char const *path)
    {
        MeshRef meshPtr = std::make_shared<Mesh>();

        Assimp::Importer importer;

        aiScene const *pScene = importer.ReadFile(path, aiProcess_Triangulate);

        if (pScene == nullptr)
        {
            fpgfx_error("Mesh import '{0}' has failed.:{1}", path, importer.GetErrorString());
            return nullptr;
        }

        // imported mesh (index=0)
        aiMesh *iMesh = pScene->mMeshes[0];

        meshPtr->Vertices.reserve(iMesh->mNumVertices);
        meshPtr->Indices.reserve(iMesh->mNumFaces * 3ULL);
        meshPtr->Indices.resize(iMesh->mNumFaces * 3ULL);
        meshPtr->IndexCnt = iMesh->mNumFaces * 3ULL;    

        for (sz_t i = 0; i < iMesh->mNumVertices; ++i)
        {
            Vec3f pos = *(Vec3f *)&iMesh->mVertices[i];
            Vec3f normal = *(Vec3f *)&iMesh->mNormals[i];
            Vec2f UV = *(Vec2f *)&iMesh->mTextureCoords[0][i];
            f32 CR = RandomFloat();
            f32 CG = RandomFloat();
            f32 CB = RandomFloat();
            meshPtr->Vertices.emplace_back(Vertex3{pos, normal, UV, Vec3f{CR,CG,CB}});
        }

        for (sz_t i = 0; i < iMesh->mNumFaces; ++i)
        {
            aiFace const &face = iMesh->mFaces[i];
            meshPtr->Indices[3*i+0] = face.mIndices[0];
            meshPtr->Indices[3*i+1] = face.mIndices[1];
            meshPtr->Indices[3*i+2] = face.mIndices[2];
        }

        return std::move(meshPtr);
    }
}