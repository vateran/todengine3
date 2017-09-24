#include "tod/node.h"
#include "tod/filesystem.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/texturecache.h"
#include "tod/graphics/modelloader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
namespace tod::graphics
{

class ModelLoaderPrivate
{
public:
    struct LoadInfo
    {
        String uriPath;
        const aiScene* aiScene;
    };

public:
    Node* load(const String& uri);
    
private:
    void load_data(aiNode* ai_node, Node* cur_node, LoadInfo* load_info);
    void apply_texture(
        LoadInfo* load_info, MeshComponent* mesh_compo,
        aiMaterial* ai_material, aiTextureType type,
        const String& texture_map_name);
};


//-----------------------------------------------------------------------------
ModelLoader::ModelLoader():
inner(new ModelLoaderPrivate)
{
}


//-----------------------------------------------------------------------------
ModelLoader::~ModelLoader()
{
    SAFE_DELETE(this->inner);
}


//-----------------------------------------------------------------------------
Node* ModelLoader::load(const String& uri)
{
    return this->inner->load(uri);
}


//-----------------------------------------------------------------------------
Node* ModelLoaderPrivate::load(const String& uri)
{
    Assimp::Importer importer;
    
    FileSystem::Data raw_data;
    if (!FileSystem::instance()->load(uri, raw_data))
        TOD_RETURN_TRACE(nullptr);
    
    auto ai_scene = importer.ReadFileFromMemory(
        &raw_data[0], raw_data.size(),
        aiProcess_Triangulate | aiProcess_CalcTangentSpace,
        uri.extractFileExtension().c_str());
    if (nullptr == ai_scene
        || ai_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE
        || nullptr == ai_scene->mRootNode)
    {
        TOD_LOG("error", "ModelLoader::load(%s):\n    %s\n",
            uri.c_str(), importer.GetErrorString());
        TOD_RETURN_TRACE(nullptr);
    }
    
    LoadInfo load_info;
    load_info.uriPath = uri.extractPath();
    load_info.aiScene = ai_scene;
    
    auto root_node = newInstance<Node>();
    this->load_data(ai_scene->mRootNode, root_node, &load_info);

    return root_node;
}


//-----------------------------------------------------------------------------
void ModelLoaderPrivate::load_data
(aiNode* ai_node, Node* cur_node, LoadInfo* load_info)
{
    //이름 설정
    cur_node->setName(ai_node->mName.C_Str());
    

    //Transform Component
    auto transform_component = newInstance<TransformComponent>();
    cur_node->addComponent(transform_component);
    
    aiVector3t<ai_real> scaling;
    aiQuaterniont<ai_real> rotation;
    aiVector3t<ai_real> position;
    ai_node->mTransformation.Decompose(scaling, rotation, position);
    Quaternion rot_quat(rotation.x, rotation.y, rotation.z, rotation.w);
    auto rot_euler = rot_quat.toEuler();
    transform_component->setTranslation(
        Vector3(position.x, position.y, position.z));
    transform_component->setRotation(
        Vector3(rot_euler.x, rot_euler.y, rot_euler.z));
    transform_component->setScaling(
        Vector3(scaling.x, scaling.y, scaling.z));
    
    
    for (uint32 i=0;i<ai_node->mNumMeshes;++i)
    {
        aiMesh* ai_mesh = load_info->aiScene->mMeshes[ai_node->mMeshes[i]];
        
        
        //Mesh Component 추가
        auto mesh_component = newInstance<MeshComponent>();
        auto mesh = mesh_component->getMesh();
        cur_node->addComponent(mesh_component);
        
        mesh->setupVertexStruct(VertexDataOption()
            .add(VertexDataType::POSITION)
            .add(VertexDataType::NORMAL)
            .add(VertexDataType::TANGENT)
            .add(VertexDataType::TEXCOORD));
        mesh->lock(ai_mesh->mNumVertices);
    
    
        //Vertices
        for (uint32 j=0;j<ai_mesh->mNumVertices;++j)
        {
            auto& v = ai_mesh->mVertices[j];
            auto& n = ai_mesh->mNormals[j];
            mesh->setVertexData(
                VertexDataType::POSITION, j, Vector3(v.x, v.y, v.z));
            mesh->setVertexData(
                VertexDataType::NORMAL, j, Vector3(n.x, n.y, n.z));
            
            if (nullptr != ai_mesh->mTangents)
            {
                auto& t = ai_mesh->mTangents[j];
                mesh->setVertexData(
                    VertexDataType::TANGENT, j, Vector3(t.x, t.y, t.z));
            }
            else
            {
                mesh->setVertexData(
                    VertexDataType::TANGENT, j, Vector3(1, 0, 0));
            }
            
            Vector2 texcoord;
            if (ai_mesh->mTextureCoords[0])
            {
                auto& t = ai_mesh->mTextureCoords[0][j];
                texcoord.x = t.x;
                texcoord.y = t.y;
            }
            mesh->setVertexData(VertexDataType::TEXCOORD, j, texcoord);
        }
        
        
        //Material
        if (0 <= ai_mesh->mMaterialIndex)
        {
            aiMaterial* ai_material = load_info->aiScene->mMaterials[ai_mesh->mMaterialIndex];
            this->apply_texture(load_info, mesh_component, ai_material, aiTextureType_DIFFUSE, "DiffuseTex");
            this->apply_texture(load_info, mesh_component, ai_material, aiTextureType_SPECULAR, "SpecularTex");
            this->apply_texture(load_info, mesh_component, ai_material, aiTextureType_NORMALS, "NormalTex");
        }
        else
        {
        }
        
        
        //Indices
        for (uint32 j=0;j<ai_mesh->mNumFaces;++j)
        {
            auto& face = ai_mesh->mFaces[j];
            for (uint32 k=0;k<face.mNumIndices;++k)
                mesh->addIndex(face.mIndices[k]);
        }
        
        
        mesh->unlock();
    }
    
    
    for (uint32 i=0;i<ai_node->mNumChildren;++i)
    {
        auto child_node = newInstance<Node>();
        cur_node->addChild(child_node);
        this->load_data(ai_node->mChildren[i], child_node, load_info);
    }
}


//-----------------------------------------------------------------------------
void ModelLoaderPrivate::apply_texture
(LoadInfo* load_info, MeshComponent* mesh_compo,
 aiMaterial* ai_material, aiTextureType type, const String& texture_map_name)
{
    for (int j=0;j<ai_material->GetTextureCount(type);++j)
    {
        aiString str;
        ai_material->GetTexture(type, j, &str);
        mesh_compo->setTexture(texture_map_name,
            load_info->uriPath + S("/") + str.C_Str());
    }
}

}

