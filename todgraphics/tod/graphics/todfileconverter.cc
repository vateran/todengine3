#include "tod/precompiled.h"
#include "tod/file.h"
#include "tod/buffer.h"
#include "tod/filesystem.h"
#include "tod/binarywriter.h"
#include "tod/binarystreamwriter.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/todfileconverter.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/skeleton.h"
#include "tod/graphics/boneanimator.h"
#include "tod/graphics/boneanimtrack.h"
#include "tod/graphics/boneanim.h"
#include "tod/graphics/bone.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
#include "tod/graphics/fbxloader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace tod::graphics
{


//-----------------------------------------------------------------------------
TodFileConverter::TodFileConverter()
: boneIndexCounter(0)
{
}


//-----------------------------------------------------------------------------
int32 TodFileConverter::allocBoneIndex(const String& bone_name)
{
    BoneNameHash hash = bone_name.hash();
    auto i = this->boneIndexer.find(hash);
    if (this->boneIndexer.end() == i)
    {
        BoneIndex new_bone_index = boneIndexCounter++;
        this->boneIndexer.emplace(hash, new_bone_index);
        return new_bone_index;
    }
    else
    {
        return i->second;
    }
}


//-----------------------------------------------------------------------------
void TodFileConverter::setTextureRootPath(const String& path)
{
    this->textureRootPath = path;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::exportMesh(const String& uri, const String& output_uri)
{
    Buffer raw_data;
    if (false == FileSystem::instance()->load(uri, &raw_data))
    {
        TOD_RETURN_TRACE(nullptr);
    }


    String ext = uri.extractFileExtension();
    if (S("fbx") == ext)
    {



        return true;
    }




    Assimp::Importer importer;
    auto ai_scene = importer.ReadFileFromMemory(
        &raw_data[0]
        , raw_data.size()
        , ( aiProcess_Triangulate
          | aiProcess_MakeLeftHanded
          | aiProcess_ValidateDataStructure
          | aiProcess_ImproveCacheLocality
          | aiProcess_LimitBoneWeights
          | aiProcess_GenNormals
          | aiProcess_FlipUVs
          )
        , uri.extractFileExtension().c_str());
    if ((nullptr == ai_scene)
        || (ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
        || (nullptr == ai_scene->mRootNode))
    {
        TOD_LOG("error", "TodFileConverter::exportMesh(%s):\n    %s\n",
            uri.c_str(), importer.GetErrorString());
        TOD_RETURN_TRACE(nullptr);
    }


    //모든 Bone 을 돌면서 unique 한 aiBone 을 알아낸다.
    std::unordered_map<String, aiBone*> bones;
    for (uint32 i = 0; i < ai_scene->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = ai_scene->mMeshes[i];

        for (uint32 j = 0; j < ai_mesh->mNumBones; ++j)
        {
            aiBone* bone = ai_mesh->mBones[j];
            bones.emplace(String(bone->mName.C_Str()), ai_mesh->mBones[j]);
        }
    }

    //Bone Index 를 할당한다
    this->boneIndexer.clear();
    this->boneIndexCounter = 0;
    std::function<void(aiNode*)> fn_alloc_bone = nullptr;
    int indent = 0;
    fn_alloc_bone = [this, &fn_alloc_bone, &bones, &indent](aiNode* parent)
    {
        ++indent;

        int32 bone_index = -1;
        if (bones.end() != bones.find(parent->mName.C_Str()))
        {
            bone_index = this->allocBoneIndex(parent->mName.C_Str());
        }

        for (int i = 0; i < indent; ++i)
            printf("    ");
        printf("exportMesh[%d %s]\n", bone_index, parent->mName.C_Str());

        for (uint32 i = 0; i < parent->mNumChildren; ++i)
        {
            fn_alloc_bone(parent->mChildren[i]);
        }
        --indent;
    };
    fn_alloc_bone(ai_scene->mRootNode);

    
    BinaryStreamWriter writer;
    writer.reserve(1024 * 1024 * 4);

    //mesh 갯수
    writer.write<uint32>(ai_scene->mNumMeshes);

    //mesh 를 돌면서
    for (uint32 i = 0; i < ai_scene->mNumMeshes; ++i)
    {
        auto ai_mesh = ai_scene->mMeshes[i];

        //SubMesh 이름
        writer.write(String(ai_mesh->mName.C_Str()));

        //Material index
        writer.write<uint8>(ai_mesh->mMaterialIndex);

        //Vertices 갯수
        writer.write<uint32>(ai_mesh->mNumVertices);

        //VB 크기 쓰기
        writer.write<uint32>(sizeof(SkinnedVertexLayout) * ai_mesh->mNumVertices);

        //VertexBuffer 를 쓸 BinaryStreamWriter 새로 생성
        BinaryStreamWriter vbwriter;
        vbwriter.reserve(sizeof(SkinnedVertexLayout) * ai_mesh->mNumVertices);

        for (uint32 j = 0; j < ai_mesh->mNumVertices; ++j)
        {
            //POSITON
            vbwriter.write(ai_mesh->mVertices[j]);

            //NORMAL
            vbwriter.write(ai_mesh->mNormals[j]);

            //TEXCOORD
            Vector2 texcoord;
            if (nullptr != ai_mesh->mTextureCoords[0])
            {
                auto& tc = ai_mesh->mTextureCoords[0][j];
                texcoord.x = tc.x;
                texcoord.y = tc.y;
            }
            vbwriter.write(texcoord);

            //BLENDINDICES
            vbwriter.write(std::array<uint8, TOD_BONE_INDEX_COUNT> {});

            //BLENDWEIGHTS
            vbwriter.write(std::array<uint8, TOD_BONE_INDEX_COUNT> {});
        }

        //BoneWeights
        BinaryWriter bone_writer(vbwriter.ptr(), vbwriter.capacity());
        std::vector<uint8> vertex_load_datas;
        vertex_load_datas.resize(ai_mesh->mNumVertices);
        uint32 num_bones = ai_mesh->mNumBones;
        for (uint32 k = 0; k < num_bones; ++k)
        {
            aiBone* ai_bone = ai_mesh->mBones[k];
            BoneIndex bone_index = this->allocBoneIndex(ai_bone->mName.C_Str());
            printf("VertexBuffer[%d %s]\n", bone_index, ai_bone->mName.C_Str());
            for (uint32 t = 0; t < ai_bone->mNumWeights; ++t)
            {
                aiVertexWeight& ai_vw = ai_bone->mWeights[t];
                bone_writer.seek(ai_vw.mVertexId * sizeof(SkinnedVertexLayout), BinaryWriter::START);
                SkinnedVertexLayout* vdata = (SkinnedVertexLayout*)bone_writer.ptr();
                uint8& vld = vertex_load_datas[ai_vw.mVertexId];
                vdata->boneIndex[vld] = bone_index;
                //vdata->weight[vld] = static_cast<uint8>(ai_vw.mWeight * 255 + 0.5f);
                vdata->weight[vld] = ai_vw.mWeight;
                ++vld;
            }
        }

        //실제 VB 데이터를 쓴다 (vbwriter -> writer)
        writer.write(vbwriter.ptr(), vbwriter.size());

        //IndexBuffer
        //IB 크기 체크
        uint32 ib_size = 0;
        uint32 index_count = 0;
        for (uint32 j = 0; j < ai_mesh->mNumFaces; ++j)
        {
            auto& face = ai_mesh->mFaces[j];
            index_count += face.mNumIndices;
        }

        //Indices 갯수
        writer.write<uint32>(index_count);

        for (uint32 j = 0; j < ai_mesh->mNumFaces; ++j)
        {
            auto& face = ai_mesh->mFaces[j];
            for (uint32 k = 0; k < face.mNumIndices; k+=3)
            {
                writer.write<uint32>(face.mIndices[k]);
                writer.write<uint32>(face.mIndices[k+2]);
                writer.write<uint32>(face.mIndices[k+1]);
            }
        }
    }

    File file(output_uri, File::OpenMode().binary().write());

    //헤더 쓰기
    TodMeshFileHeader header;
    header.version = 1;
    file.write(header);

    //Material
    file.write<uint32>(ai_scene->mNumMaterials);
    for (uint32 i = 0; i < ai_scene->mNumMaterials; ++i)
    {
        aiMaterial* ai_material = ai_scene->mMaterials[i];

        for (uint32 j = 0; j < ai_material->mNumProperties; ++j)
        {
            switch (ai_material->mProperties[j]->mType)
            {
            case aiPTI_Float:
            {
                float value[20];
                memset(&value, 0, sizeof(value));
                memcpy(&value[0]
                    , ai_material->mProperties[j]->mData
                    , ai_material->mProperties[j]->mDataLength);
                printf("%s : [%f,%f,%f]\n"
                    , ai_material->mProperties[j]->mKey.C_Str()
                    , value[0], value[1], value[2]);
                break;
            }
            case aiPTI_Double:
            {
                double value[20];
                memset(&value, 0, sizeof(value));
                memcpy(&value
                    , ai_material->mProperties[j]->mData
                    , ai_material->mProperties[j]->mDataLength);
                printf("%s : [%f,%f,%f]\n"
                    , ai_material->mProperties[j]->mKey.C_Str()
                    , value[0], value[1], value[2]);
                break;
            }
            case aiPTI_String:
            {
                String value;
                value.resize(ai_material->mProperties[j]->mDataLength - 4);
                memcpy(&value[0]
                    , ai_material->mProperties[j]->mData + 4
                    , ai_material->mProperties[j]->mDataLength - 4);
                printf("%s : [%s]\n"
                    , ai_material->mProperties[j]->mKey.C_Str()
                    , value.c_str());
                break;
            }
            case aiPTI_Integer:
            {
                int32 value[20];
                memset(&value, 0, sizeof(value));
                memcpy(&value
                    , ai_material->mProperties[j]->mData
                    , ai_material->mProperties[j]->mDataLength);
                printf("%s : [%d,%d,%d]\n"
                    , ai_material->mProperties[j]->mKey.C_Str()
                    , value[0], value[1], value[2]);
                break;
            }
            case aiPTI_Buffer:
            {
                String value;
                value.resize(ai_material->mProperties[j]->mDataLength - 4);
                memcpy(&value[0]
                    , ai_material->mProperties[j]->mData + 4
                    , ai_material->mProperties[j]->mDataLength - 4);
                printf("%s : [%s]\n"
                    , ai_material->mProperties[j]->mKey.C_Str()
                    , value.c_str());
                break;
            }
            }
        }

                       
        static aiTextureType ai_texture_type[] =
        {
              aiTextureType_DIFFUSE
            , aiTextureType_NORMALS
            , aiTextureType_SPECULAR
        };

        for (int32 i = 0; i < Mesh::Material::MAX_USE_TEXTURE; ++i)
        {
            aiString aipath;
            ai_material->GetTexture(ai_texture_type[i], 0, &aipath);
            String final_path;
            if (0 < aipath.length)
            {
                String path(aipath.C_Str());
                path = path.normalizePath();
                String file_name = path.extractFileName();
                final_path = this->textureRootPath + S("/") + file_name;
            }
            file.write(final_path);
        }
    }

    //vertex/index 데이터 쓰기
    file.write(writer.ptr(), writer.size());

    return true;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::importMesh(const String& uri, Mesh** mesh_out)
{
    File file(uri, File::OpenMode().binary().read());
    if (false == file.valid())
    {
        TOD_RETURN_TRACE(false);
    }

    Mesh* mesh = new Mesh();

    //TodFileHeader 읽기
    TodMeshFileHeader header;
    file.read(header);

    //Material
    uint32 num_materials;
    file.read<uint32>(num_materials);
    mesh->resizeMaterialSlot(num_materials);
    for (uint32 i = 0; i < num_materials; ++i)
    {
        Mesh::Material* material = mesh->getMaterial(i);
        for (int32 i = 0; i < 3; ++i)
        {
            String path;
            file.read(path);
            if (false == path.empty())
            {
                material->textures[i] = path;
            }
        }
    }

    uint32 num_mesh = 0;
    file.read(num_mesh);
    
    for (uint32 i = 0; i < num_mesh; ++i)
    {
        //SubMesh 이름
        String sub_mesh_name;
        file.read(sub_mesh_name);

        //material index
        uint8 material_index;
        file.read(material_index);

        //SubMesh 생성
        Mesh::SubMesh* sub_mesh = mesh->newSubMesh(sub_mesh_name.c_str());
        sub_mesh->materialIndex = material_index;
        sub_mesh->aabb.set({ -10, -10, -10 }, { 10, 10, 10 });

        //VertexBuffer
        {
            //Vertices 갯수
            uint32 num_vertices = 0;
            file.read(num_vertices);

            //VB 크기
            uint32 vb_size = 0;
            file.read(vb_size);

            sub_mesh->vb->create(num_vertices, sizeof(SkinnedVertexLayout));
            int8* vb_ptr = nullptr;
            sub_mesh->vb->lock(&vb_ptr);
            file.read(vb_ptr, vb_size);
            sub_mesh->vb->unlock();
        }

        //IndexBuffer
        {
            //Indices 갯수
            uint32 num_indices = 0;
            file.read(num_indices);

            sub_mesh->ib->create(num_indices, sizeof(uint32));
            int8* ib_ptr = nullptr;
            sub_mesh->ib->lock(&ib_ptr);
            file.read(ib_ptr, num_indices * sizeof(uint32));
            sub_mesh->ib->unlock();
        }
    }

    *mesh_out = mesh;
    mesh->updateAABB();

    return true;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::exportAnim(const String& uri, const String& output_uri)
{
    Buffer raw_data;
    if (false == FileSystem::instance()->load(uri, &raw_data))
    {
        TOD_RETURN_TRACE(nullptr);
    }

    Assimp::Importer importer;
    auto ai_scene = importer.ReadFileFromMemory(
        &raw_data[0]
        , raw_data.size()
        , (
              aiProcess_MakeLeftHanded
            | aiProcess_LimitBoneWeights
            | aiProcess_ValidateDataStructure
            | aiProcess_ImproveCacheLocality
            )
        , uri.extractFileExtension().c_str());
    if ((nullptr == ai_scene)
     || (nullptr == ai_scene->mRootNode))
    {
        TOD_LOG("error", "TodFileConverter::exportAnim(%s):\n    %s\n",
            uri.c_str(), importer.GetErrorString());
        TOD_RETURN_TRACE(nullptr);
    }

    this->boneIndexer.clear();
    this->boneIndexCounter = 0;


    File file(output_uri, File::OpenMode().binary().write());


    //헤더 쓰기
    TodAnimFileHeader header;
    header.version = 1;
    file.write(header);


    //Global InvTransform
    Matrix44 global_inv_transform;
    memcpy(
          &global_inv_transform
        , &ai_scene->mRootNode->mTransformation
        , sizeof(Matrix44));
    global_inv_transform.inverse();
    file.write(global_inv_transform);


    //Animation Count
    file.write<uint32>(ai_scene->mNumAnimations);


    for (uint32 i = 0; i < ai_scene->mNumAnimations; ++i)
    {
        auto ai_anim = ai_scene->mAnimations[i];

        //Animation name
        String anim_name(ai_anim->mName.C_Str());
        file.write(anim_name);

        //Duration
        float tick_per_sec = static_cast<float>
            (ai_anim->mTicksPerSecond != 0 ? ai_anim->mTicksPerSecond : 4000.0f);
        float duration = static_cast<float>(ai_anim->mDuration / tick_per_sec);
        file.write(duration);

        //Bone Count
        file.write<uint32>(ai_anim->mNumChannels);


        for (uint32 j = 0; j < ai_anim->mNumChannels; ++j)
        {
            auto track = ai_anim->mChannels[j];

            //Bone Name
            String bone_name(track->mNodeName.C_Str());
            file.write(bone_name);


            file.write<uint32>(track->mNumPositionKeys);
            for (uint32 k = 0; k < track->mNumPositionKeys; ++k)
            {
                auto& key = track->mPositionKeys[k];

                //Time
                file.write<float>(static_cast<float>(key.mTime / tick_per_sec));

                //Position
                Vector3 position(key.mValue.x, key.mValue.y, key.mValue.z);
                file.write(position);
            }

            file.write<uint32>(track->mNumRotationKeys);
            for (uint32 k = 0; k < track->mNumRotationKeys; ++k)
            {
                auto& key = track->mRotationKeys[k];

                //Time
                file.write<float>(static_cast<float>(key.mTime / tick_per_sec));

                //Rotation
                Quaternion rotation(key.mValue.x, key.mValue.y, key.mValue.z, key.mValue.w);
                file.write(rotation);
            }

            file.write<uint32>(track->mNumScalingKeys);
            for (uint32 k = 0; k < track->mNumScalingKeys; ++k)
            {
                auto& key = track->mScalingKeys[k];

                //Time
                file.write<float>(static_cast<float>(key.mTime / tick_per_sec));

                //Scaling
                Vector3 scaling(key.mValue.x, key.mValue.y, key.mValue.z);
                file.write(scaling);
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::importAnim(const String& uri, BoneAnimator** anim_out)
{
    File file(uri, File::OpenMode().binary().read());
    if (false == file.valid())
    {
        TOD_RETURN_TRACE(false);
    }

    BoneAnimator* bone_animator = new BoneAnimator();
    *anim_out = bone_animator;
    int32 bone_index = 0;


    //헤더 읽기
    TodAnimFileHeader header;
    header.version = 1;
    file.read(header);


    //Global InvTransform
    file.read(bone_animator->globalInvTransform);


    //Animation Count
    uint32 anim_count = 0;
    file.read(anim_count);


    for (uint32 i = 0; i < anim_count; ++i)
    {
        //Animation name
        String anim_name;
        file.read(anim_name);

        //Duration
        float duration;
        file.read(duration);

        //Bone Count
        uint32 bone_count = 0;
        file.read<uint32>(bone_count);


        auto anim = bone_animator->newBoneAnim(anim_name, duration);

        for (uint32 j = 0; j < bone_count; ++j)
        {
            //Bone Name
            String bone_name;
            file.read(bone_name);

            printf("Anim Bone[%s]\n", bone_name.c_str());

            auto boneAnimTrack = anim->createBoneAnimTrack(bone_name);

            uint32 num_position_keys = 0;
            file.read(num_position_keys);
            for (uint32 k = 0; k < num_position_keys; ++k)
            {
                //Time
                float time = 0;
                file.read(time);

                //Position
                Vector3 position;
                file.read(position);

                boneAnimTrack->positionKeys.emplace(time, position);
            }

            uint32 num_rotation_keys = 0;
            file.read(num_rotation_keys);
            for (uint32 k = 0; k < num_rotation_keys; ++k)
            {
                //Time
                float time = 0;
                file.read(time);

                //Quaternion
                Quaternion rotation;
                file.read(rotation);

                boneAnimTrack->rotateKeys.emplace(time, rotation);
            }

            uint32 num_scaling_keys = 0;
            file.read(num_scaling_keys);
            for (uint32 k = 0; k < num_scaling_keys; ++k)
            {
                //Time
                float time = 0;
                file.read(time);

                //Scaling
                Vector3 scaling;
                file.read(scaling);

                boneAnimTrack->scaleKeys.emplace(time, scaling);
            }
        }
    }

    return true;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::exportSkeleton
(const String& uri, const String& output_uri, const String& root_name)
{
    Buffer raw_data;
    if (false == FileSystem::instance()->load(uri, &raw_data))
    {
        TOD_RETURN_TRACE(nullptr);
    }


    Assimp::Importer importer;
    auto ai_scene = importer.ReadFileFromMemory(
        &raw_data[0]
        , raw_data.size()
        , (
              aiProcess_MakeLeftHanded
            | aiProcess_LimitBoneWeights
            | aiProcess_ValidateDataStructure
            | aiProcess_ImproveCacheLocality
            )
        , uri.extractFileExtension().c_str());
    if ((nullptr == ai_scene)
        || (nullptr == ai_scene->mRootNode))
    {
        TOD_LOG("error", "TodFileConverter::exportAnim(%s):\n    %s\n",
            uri.c_str(), importer.GetErrorString());
        TOD_RETURN_TRACE(nullptr);
    }

        
    File file(output_uri, File::OpenMode().write().binary());
    if (false == file.valid())
    {
        TOD_RETURN_TRACE(false);
    }


    {
        //걍 출력용
        int indent = 0;
        std::function<void(aiNode*)> f = nullptr;
        f = [&indent, &f](aiNode* node)
        {
            ++indent;

            for (int i = 0; i < indent; ++i)
                printf("    ");
            printf("Skeleton[%s]\n", node->mName.C_Str());

            for (uint32 i = 0; i < node->mNumChildren; ++i)
            {
                f(node->mChildren[i]);
            }
            --indent;
        };
        f(ai_scene->mRootNode);
    }


    //모든 Bone 을 돌면서 unique 한 aiBone 을 알아낸다.
    std::unordered_map<String, aiBone*> bones;
    for (uint32 i = 0; i < ai_scene->mNumMeshes; ++i)
    {
        aiMesh* ai_mesh = ai_scene->mMeshes[i];

        for (uint32 j = 0; j < ai_mesh->mNumBones; ++j)
        {
            aiBone* bone = ai_mesh->mBones[j];
            bones.emplace(String(bone->mName.C_Str()), ai_mesh->mBones[j]);
        }
    }

    int indent = 0;
    int bone_index = 0;

    std::function<bool(aiNode*)> fn_save_bone = nullptr;
    fn_save_bone = [&fn_save_bone, &file, &bones, &indent, &bone_index](aiNode* cur_node)
    {
        ++indent;

        //Bone 이름
        file.write(String(cur_node->mName.C_Str()));

        //Bone 인지 여부
        auto bone_iter = bones.find(cur_node->mName.C_Str());
        bool is_bone = (bones.end() != bone_iter);
        file.write(is_bone);

        //기본 Transform
        file.write(cur_node->mTransformation);


        for (int i = 0; i < indent; ++i)
            printf("    ");
        printf("exportSkeleton[%d %s]\n", bone_index, cur_node->mName.C_Str());


        if (is_bone)
        {
            //Bone Offset Matrix
            aiBone* bone = bone_iter->second;
            file.write(bone->mOffsetMatrix);
            ++bone_index;
        }
        
        //children 갯수
        file.write<uint32>(cur_node->mNumChildren);
        
        //Children 돌면서 저장
        for (uint32 i = 0; i < cur_node->mNumChildren; ++i)
        {   
            aiNode* child = cur_node->mChildren[i];
            if (false == fn_save_bone(child))
            {
                TOD_LOG("error"
                    , "Node[%s] Child 저장하다가 에러"
                    , child->mName.C_Str());
                TOD_RETURN_TRACE(false);
            }
        }

        --indent;

        return true;
    };

    if (false == fn_save_bone(ai_scene->mRootNode))
    {
        TOD_RETURN_TRACE(false);
    }

    return true;
}


//-----------------------------------------------------------------------------
bool TodFileConverter::importSkeleton(const String& uri, Skeleton** skeleton_out)
{
    File file(uri, File::OpenMode().read().binary());
    if (false == file.valid())
    {
        TOD_RETURN_TRACE(false);
    }

    Skeleton* skeleton = new Skeleton();

    int indent = 0;

    std::function<bool(Bone*)> fn_load_bone = nullptr;
    fn_load_bone = [&fn_load_bone, &file , &skeleton, &indent](Bone* parent)
    {
        ++indent;

        //Root 를 읽는다
        String bone_name;
        file.read(bone_name);

        //Bone 인지 판별
        bool is_bone = false;
        file.read(is_bone);

        Bone* bone = skeleton->newBone(bone_name, is_bone);
        if (nullptr != parent)
        {
            parent->addChild(bone);
        }

        //BindTransform
        file.read(bone->getBindTransform());

        if (is_bone)
        {
            //Bone Offset Matrix
            file.read(bone->getBoneOffset());
        }

        //children 갯수
        uint32 child_count = 0;
        file.read(child_count);

        for (int i = 0; i < indent; ++i)
            printf("    ");
        printf("importSkeleton[%d %s]\n", bone->getBoneTransformIndex(), bone_name.c_str());

        for (uint32 i = 0; i < child_count; ++i)
        {   
            if (false == fn_load_bone(bone))
            {
                TOD_SAFE_DELETE(skeleton);
                TOD_RETURN_TRACE(false);
            }
        }

        --indent;

        return true;
    };


    if (false == fn_load_bone(nullptr))
    {
        TOD_SAFE_DELETE(skeleton);
        TOD_RETURN_TRACE(false);
    }

    *skeleton_out = skeleton;

    return true;
}

}