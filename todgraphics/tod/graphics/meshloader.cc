#include "tod/graphics/meshloader.h"

#include <array>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "tod/filesystem.h"
#include "tod/binarywriter.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/vector2.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/bone.h"
#include "tod/graphics/boneanim.h"
#include "tod/graphics/boneanimtrack.h"
#include "tod/graphics/boneanimator.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
#include "tod/graphics/todfileconverter.h"
#include "tod/graphics/fbxloader.h"
#include "tod/graphics/gltfloader.h"
namespace tod::graphics
{

class MeshLoader::MeshLoaderPrivate
{
public:
    struct LoadInfo
    {
        LoadInfo()
            : aiScene(nullptr)
            , boneAnimaController(nullptr) {}

        String uriPath;
        const aiScene* aiScene;
        BoneAnimator* boneAnimaController;
    };

public:
    bool loadMesh(const String& uri, Mesh** mesh)
    {
        String ext = uri.extractFileExtension();
        ext.lower();
        if (S("todmesh") == ext)
        {
            TodFileConverter file;
            return file.importMesh(uri, mesh);
        }
        else if (S("fbx") == ext)
        {
            FbxLoader file;
            return file.importMesh(uri, mesh);
        }
        else if (S("glb") == ext)
        {
            GLTFLoader file;
            return file.importMesh(uri, mesh);
        }

        Assimp::Importer importer;

        Buffer raw_data;
        if (false == FileSystem::instance()->load(uri, &raw_data))
        {
            TOD_RETURN_TRACE(false);
        }

        auto ai_scene = importer.ReadFileFromMemory(
              &raw_data[0]
            , raw_data.size()
            , aiProcess_Triangulate
            | aiProcess_MakeLeftHanded
            , uri.extractFileExtension().c_str());
        if (nullptr == ai_scene
            || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
            || nullptr == ai_scene->mRootNode)
        {
            TOD_LOG("error", "MeshLoader::load(%s):\n    %s\n",
                uri.c_str(), importer.GetErrorString());
            TOD_RETURN_TRACE(false);
        }

        LoadInfo load_info;
        load_info.uriPath = uri.extractPath();
        load_info.aiScene = ai_scene;
        if (true == load_info.uriPath.empty()) load_info.uriPath = ".";

        *mesh = this->load_skinned_mesh(load_info);
        if (nullptr == *mesh)
        {
            TOD_RETURN_TRACE(false);
        }

        return true;
    }

    bool loadAnim(const String& uri, BoneAnimator** bone_animator)
    {
        if (S("todanim") == uri.extractFileExtension())
        {
            TodFileConverter file;
            return file.importAnim(uri, bone_animator);
        }

        Assimp::Importer importer;

        Buffer raw_data;
        if (false == FileSystem::instance()->load(uri, &raw_data))
        {
            TOD_RETURN_TRACE(nullptr);
        }

        auto ai_scene = importer.ReadFileFromMemory(
            &raw_data[0]
            , raw_data.size()
            , aiProcess_MakeLeftHanded
            , uri.extractFileExtension().c_str());
        if (nullptr == ai_scene
            || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
            || nullptr == ai_scene->mRootNode)
        {
            TOD_LOG("error", "MeshLoader::load(%s):\n    %s\n",
                uri.c_str(), importer.GetErrorString());
            TOD_RETURN_TRACE(nullptr);
        }

        LoadInfo load_info;
        load_info.uriPath = uri.extractPath();
        load_info.aiScene = ai_scene;
        if (true == load_info.uriPath.empty()) load_info.uriPath = ".";

        this->load_anim(load_info);

        *bone_animator = load_info.boneAnimaController;

        return true;
    }

private:
    bool load_anim(LoadInfo& load_info)
    {
        /*BoneAnimator* bone_animator = new BoneAnimator();

        memcpy(
            bone_animator->globalInvTransform
            , &load_info.aiScene->mRootNode->mTransformation
            , sizeof(Matrix44));
        bone_animator->globalInvTransform.inverse();

        for (uint32 i = 0; i < load_info.aiScene->mNumAnimations; ++i)
        {
            auto ai_anim = load_info.aiScene->mAnimations[i];

            auto anim = bone_animator->newBoneAnim(ai_anim->mName.C_Str());
            float tick_per_sec = static_cast<float>
                (ai_anim->mTicksPerSecond != 0 ? ai_anim->mTicksPerSecond : 4000.0f);
            anim->create(
                ai_anim->mNumChannels
                , static_cast<float>(ai_anim->mDuration / tick_per_sec));

            for (uint32 j = 0; j < ai_anim->mNumChannels; ++j)
            {
                auto track = ai_anim->mChannels[j];
                int32 boneTransformIndex = bone_animator->
                    newBone(track->mNodeName.C_Str())->boneTransformIndex;
                auto boneAnimTrack = anim->getBoneAnimTrack(boneTransformIndex);
                boneAnimTrack->name = track->mNodeName.C_Str();

                for (uint32 k = 0; k < track->mNumPositionKeys; ++k)
                {
                    auto& key = track->mPositionKeys[k];
                    boneAnimTrack->positionKeys.emplace(
                        static_cast<float>(key.mTime / tick_per_sec)
                        , Vector3(
                            key.mValue.x
                            , key.mValue.y
                            , key.mValue.z));
                }
                for (uint32 k = 0; k < track->mNumRotationKeys; ++k)
                {
                    auto& key = track->mRotationKeys[k];
                    boneAnimTrack->rotateKeys.emplace(
                        static_cast<float>(key.mTime / tick_per_sec)
                        , Quaternion(
                            key.mValue.x
                            , key.mValue.y
                            , key.mValue.z
                            , key.mValue.w));
                }
                for (uint32 k = 0; k < track->mNumScalingKeys; ++k)
                {
                    auto& key = track->mScalingKeys[k];
                    boneAnimTrack->scaleKeys.emplace(
                        static_cast<float>(key.mTime / tick_per_sec)
                        , Vector3(
                            key.mValue.x
                            , key.mValue.y
                            , key.mValue.z));
                }
            }
        }

        load_info.boneAnimaController = bone_animator;

        auto root_bone = bone_animator->newBone(
            load_info.aiScene->mRootNode->mName.C_Str());
        this->load_bone(load_info, load_info.aiScene->mRootNode, root_bone);
        bone_animator->rootBone = root_bone;*/

        return true;
    }

    Mesh* load_skinned_mesh(LoadInfo& load_info)
    {
        auto mesh = new Mesh();
        auto ai_scene = load_info.aiScene;

        for (uint32 i = 0; i < ai_scene->mNumMeshes; ++i)
        {
            auto ai_mesh = ai_scene->mMeshes[i];

            //货肺款 SubMesh 积己
            auto sub_mesh = mesh->newSubMesh(ai_mesh->mName.C_Str());

            //VertexBuffer
            {
                sub_mesh->vb->create(
                      ai_mesh->mNumVertices
                    , sizeof(SkinnedVertexLayout));
                int8* ptr = nullptr;
                sub_mesh->vb->lock(&ptr);

                BinaryWriter writer(ptr, sub_mesh->vb->size());

                //Vertices
                for (uint32 j = 0; j < ai_mesh->mNumVertices; ++j)
                {
                    SkinnedVertexLayout* vdata
                        = (SkinnedVertexLayout*)writer.ptr();

                    //POSITON
                    writer.write(ai_mesh->mVertices[j]);

                    //NORMAL
                    writer.write(ai_mesh->mNormals[j]);

                    //TEXCOORD
                    Vector2 texcoord;
                    if (nullptr != ai_mesh->mTextureCoords[0])
                    {
                        auto& tc = ai_mesh->mTextureCoords[0][j];
                        texcoord.x = tc.x;
                        texcoord.y = 1 - tc.y;
                    }
                    writer.write(texcoord);

                    //BLENDINDICES
                    writer.write(std::array<uint8, TOD_BONE_INDEX_COUNT> {});

                    //BLENDWEIGHTS
                    writer.write(std::array<uint8, TOD_BONE_INDEX_COUNT> {});
                }

                //BoneWeights
                std::vector<uint8> vertex_load_datas;
                vertex_load_datas.resize(ai_mesh->mNumVertices);
                uint32 num_bones = std::min<uint32>(4, ai_mesh->mNumBones);
                for (uint32 k = 0; k < num_bones; ++k)
                {
                    aiBone* ai_bone = ai_mesh->mBones[k];
                    auto bone = load_info.boneAnimaController->newBone(ai_bone->mName.C_Str());
                    memcpy(&bone->boneOffset, &ai_bone->mOffsetMatrix, sizeof(Matrix44));
                    for (uint32 t = 0; t < ai_bone->mNumWeights; ++t)
                    {
                        aiVertexWeight& ai_vw = ai_bone->mWeights[t];
                        writer.seek(ai_vw.mVertexId * sizeof(SkinnedVertexLayout), BinaryWriter::START);
                        SkinnedVertexLayout* vdata = (SkinnedVertexLayout*)writer.ptr();
                        uint8& vld = vertex_load_datas[ai_vw.mVertexId];
                        vdata->boneIndex[vld] = bone->boneTransformIndex;
                        vdata->weight[vld] = static_cast<uint8>(ai_vw.mWeight * 255 + 0.5f);
                        ++vld;
                    }
                }

                sub_mesh->vb->unlock();
            }

            //IndexBuffer
            {
                sub_mesh->ib->create(ai_mesh->mNumFaces * 3, sizeof(uint32));
                int8* ptr = nullptr;
                sub_mesh->ib->lock(&ptr);

                BinaryWriter writer(ptr, sub_mesh->ib->size());

                for (uint32 j = 0; j < ai_mesh->mNumFaces; ++j)
                {
                    auto& face = ai_mesh->mFaces[j];
                    for (uint32 k = 0; k < face.mNumIndices; ++k)
                    {
                        writer.write(face.mIndices[k]);
                    }
                }
                sub_mesh->ib->unlock();
            }

            //Material
            if (0 <= ai_mesh->mMaterialIndex)
            {
                aiMaterial* ai_material = load_info.aiScene->mMaterials[ai_mesh->mMaterialIndex];
                this->apply_texture(load_info, sub_mesh, ai_material, aiTextureType_DIFFUSE, "texDiffuse");
                this->apply_texture(load_info, sub_mesh, ai_material, aiTextureType_SPECULAR, "texSpecular");
                this->apply_texture(load_info, sub_mesh, ai_material, aiTextureType_NORMALS, "texNormal");
            }
        }

        return mesh;
    }

    void load_bone(LoadInfo& load_info, aiNode* cur_node, Bone* cur_bone)
    {
        /*assert(nullptr != cur_node);

        memcpy(&cur_bone->transform, &cur_node->mTransformation, sizeof(Matrix44));

        for (uint32 i = 0; i < cur_node->mNumChildren; ++i)
        {
            auto child = cur_node->mChildren[i];
            auto bone = load_info.boneAnimaController->newBone(child->mName.C_Str());
            cur_bone->addChild(bone);
            this->load_bone(load_info, child, bone);
        }*/
    }

    void apply_texture(
        LoadInfo& load_info
        , Mesh::SubMesh* sub_mesh
        , aiMaterial* ai_material
        , aiTextureType type
        , const String& texture_map_name)
    {
        for (uint32 j = 0; j < ai_material->GetTextureCount(type); ++j)
        {
            aiString str;
            ai_material->GetTexture(type, j, &str);
            String path = load_info.uriPath + S("/") + str.C_Str();
            if (!FileSystem::instance()->isExists(path))
            {
                path = path.normalizePath().extractFileName();
            }
            if (!FileSystem::instance()->isExists(path))
            {
                path = load_info.uriPath + "/" + path;
            }
            if (!FileSystem::instance()->isExists(path))
            {
                path = load_info.uriPath + "/" + path;
            }

            //sub_mesh->setTexture(texture_map_name, path);
        }
    }
};


//-----------------------------------------------------------------------------
MeshLoader::MeshLoader()
: inner(new MeshLoaderPrivate)
{
    TOD_ASSERT(Renderer::instance()->isValid(), "Renderer 积己鞘夸");
}


//-----------------------------------------------------------------------------
MeshLoader::~MeshLoader()
{
    TOD_SAFE_DELETE(this->inner);
}


//-----------------------------------------------------------------------------
bool MeshLoader::load
(const String& uri, Mesh** mesh)
{
    return this->inner->loadMesh(uri, mesh);
}


//-----------------------------------------------------------------------------
AnimLoader::AnimLoader()
: inner(new MeshLoader::MeshLoaderPrivate)
{
}


//-----------------------------------------------------------------------------
AnimLoader::~AnimLoader()
{
    TOD_SAFE_DELETE(this->inner);
}


//-----------------------------------------------------------------------------
bool AnimLoader::load(const String& uri, BoneAnimator** bone_animator)
{
    return this->inner->loadAnim(uri, bone_animator);
}

}