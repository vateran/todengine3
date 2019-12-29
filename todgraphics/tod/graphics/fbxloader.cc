#include "tod/graphics/fbxloader.h"
#include "tod/file.h"
#include "tod/filesystem.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/renderinterface.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/inputlayout.h"
#include "tod/graphics/vertexbuffer.h"
#include "tod/graphics/indexbuffer.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/material.h"
#include <fbxsdk.h>
#include <fbxsdk/utils/FbxGeometryConverter.h>

namespace tod::graphics
{


class FbxLoaderPrivate
{
public:
    FbxLoaderPrivate();
    ~FbxLoaderPrivate();
    
    bool importMesh(const String& uri, Mesh** mesh);
    
private:
    void load_fbx_node(FbxNode* cur_fbx_node, Mesh* mesh);

private:
    FbxManager* fbxmgr;
    FbxIOSettings* ios;
    FbxGeometryConverter* geometryConverter;
    int32 vertexIndex;
};

    
class TodFbxStream : public FbxStream
{
public:
    TodFbxStream(FbxManager* fbxmgr, const String& uri);
    virtual~TodFbxStream();
    EState GetState() override;
    bool Open(void*) override;
    bool Close() override;
    bool Flush() override;
    int Write(const void* pData, int pSize) override;
    int Read (void* pData, int pSize) const override;
    int GetReaderID() const override;
    int GetWriterID() const override;
    void Seek(const FbxInt64& pOffset,
              const FbxFile::ESeekPos& pSeekPos) override;
    long GetPosition() const override;
    void SetPosition(long pPosition) override;
    int GetError() const override;
    void ClearError() override;
    
private:
    String uri;
    Buffer data;
    mutable FbxInt64 pos;
    int readerID;
};


//-----------------------------------------------------------------------------
TodFbxStream::TodFbxStream(FbxManager* fbxmgr, const String& uri)
: pos(0)
, readerID(-1)
{
    this->uri = uri;
    auto ext = uri.extractFileExtension();
    ext.lower();
    this->readerID = fbxmgr->
        GetIOPluginRegistry()->
            FindReaderIDByExtension(ext.c_str());
}


//-----------------------------------------------------------------------------
TodFbxStream::~TodFbxStream()
{
    Close();
}


//-----------------------------------------------------------------------------
TodFbxStream::EState TodFbxStream::GetState()
{
    return !this->data.empty() ? FbxStream::eOpen : eClosed;
}


//-----------------------------------------------------------------------------
bool TodFbxStream::Open(void*)
{
    if (this->data.empty())
    {
        if (!FileSystem::instance()->load(this->uri, &this->data))
            TOD_RETURN_TRACE(false);
    }
    
    this->pos = 0;
    
    return true;
}


//-----------------------------------------------------------------------------
bool TodFbxStream::Close()
{
    return true;
}


//-----------------------------------------------------------------------------
bool TodFbxStream::Flush()
{
    return true;
}


//-----------------------------------------------------------------------------
int TodFbxStream::Write(const void* pData, int pSize)
{
    return 0;
}


//-----------------------------------------------------------------------------
int TodFbxStream::Read(void* pData, int pSize) const
{
    if (this->data.empty()) return 0;
    if (this->data.size() <= static_cast<size_t>(this->pos)) return 0;
    auto read_size = std::min(pSize,
        static_cast<int>(this->data.size() - this->pos));
    std::memcpy(pData, &this->data[this->pos], read_size);
    this->pos += read_size;
    return read_size;
}


//-----------------------------------------------------------------------------
int TodFbxStream::GetReaderID() const
{
    return this->readerID;
}


//-----------------------------------------------------------------------------
int TodFbxStream::GetWriterID() const
{
    return -1;
}


//-----------------------------------------------------------------------------
void TodFbxStream::Seek
(const FbxInt64& pOffset, const FbxFile::ESeekPos& pSeekPos)
{
    switch (pSeekPos)
    {
        case FbxFile::eBegin:
            this->pos = (long)pOffset;
            break;
        case FbxFile::eCurrent:
            this->pos += (long)pOffset;
            break;
        case FbxFile::eEnd:
            this->pos = this->data.size() - (long)pOffset;
            break;
    }
}


//-----------------------------------------------------------------------------
long TodFbxStream::GetPosition() const
{
    return static_cast<long>(this->pos);
}


//-----------------------------------------------------------------------------
void TodFbxStream::SetPosition(long pPosition)
{
    this->pos = pPosition;
}


//-----------------------------------------------------------------------------
int TodFbxStream::GetError() const
{
    return 0;
}


//-----------------------------------------------------------------------------
void TodFbxStream::ClearError()
{
}


//-----------------------------------------------------------------------------
FbxLoader::FbxLoader():
inner(new FbxLoaderPrivate)
{
}


//-----------------------------------------------------------------------------
FbxLoader::~FbxLoader()
{
    TOD_SAFE_DELETE(this->inner);
}
    
    
//-----------------------------------------------------------------------------
bool FbxLoader::importMesh(const String& uri, Mesh** mesh)
{
    return this->inner->importMesh(uri, mesh);
}


//-----------------------------------------------------------------------------
FbxLoaderPrivate::FbxLoaderPrivate():
  fbxmgr(nullptr)
, ios(nullptr)
, geometryConverter(nullptr)
, vertexIndex(0)
{
    this->fbxmgr = FbxManager::Create();
    this->ios = FbxIOSettings::Create(this->fbxmgr, IOSROOT);
    this->fbxmgr->SetIOSettings(this->ios);
    this->geometryConverter = new FbxGeometryConverter(this->fbxmgr);
}


//-----------------------------------------------------------------------------
FbxLoaderPrivate::~FbxLoaderPrivate()
{
    TOD_SAFE_DELETE(this->geometryConverter);
}


//-----------------------------------------------------------------------------
bool FbxLoaderPrivate::importMesh(const String& uri, Mesh** mesh)
{
    TodFbxStream stream(this->fbxmgr, uri);
    FbxImporter* importer = FbxImporter::Create(this->fbxmgr, "");
    if (!importer->Initialize(&stream, nullptr, -1, this->ios))
    {
        TOD_THROW_EXCEPTION("FbxLoader error:(%s)\n\nuri(%s)",
                            importer->GetStatus().GetErrorString(),
                            uri.c_str());
        TOD_RETURN_TRACE(nullptr);
    }
    

    FbxScene* root_scene = FbxScene::Create(this->fbxmgr, "");
    bool ret = importer->Import(root_scene);
    importer->Destroy();
    if (false == ret) TOD_RETURN_TRACE(false);

    
    //Axis 변경
    //FbxAxisSystem SceneAxisSystem = root_scene->GetGlobalSettings().GetAxisSystem();
    //FbxAxisSystem OurAxisSystem(FbxAxisSystem::eYAxis, FbxAxisSystem::eParityOdd, FbxAxisSystem::eRightHanded);
    //if (SceneAxisSystem != OurAxisSystem)
    //{
        //OurAxisSystem.ConvertScene(root_scene);
    //}


    //단위 변경
    //FbxSystemUnit SceneSystemUnit = root_scene->GetGlobalSettings().GetSystemUnit();
    //if (SceneSystemUnit.GetScaleFactor() != 1.0)
    //{
        //The unit in this example is centimeter.
        //FbxSystemUnit::cm.ConvertScene(root_scene);
    //}


    //NUBS, patch 를 Triangle 로 변경
    this->geometryConverter->Triangulate(root_scene, true);


    Mesh* out_mesh = new Mesh();

    int32 material_count = root_scene->GetMaterialCount();
    for (int32 i = 0; i < material_count; ++i)
    {
        FbxSurfaceMaterial* fbx_material = root_scene->GetMaterial(i);

        FbxProperty lProperty = fbx_material->GetFirstProperty();
        while (lProperty.IsValid())
        {
            lProperty = fbx_material->GetNextProperty(lProperty);
            printf("%s : ", lProperty.GetName().Buffer());
            switch (lProperty.GetPropertyDataType().GetType())
            {
            case eFbxBool:
                printf("%s", lProperty.Get<FbxBool>()?"true":"false");
                break;
            case eFbxDouble:
                printf("%f", lProperty.Get<FbxDouble>());
                break;
            case eFbxFloat:
                printf("%f", lProperty.Get<FbxFloat>());
                break;
            case eFbxDouble4:
                printf("%f %f %f %f", lProperty.Get<FbxColor>().mRed, lProperty.Get<FbxColor>().mGreen, lProperty.Get<FbxColor>().mBlue, lProperty.Get<FbxColor>().mAlpha);
                break;
            case eFbxDouble3:
                printf("%f %f %f", lProperty.Get<FbxDouble3>().mData[0], lProperty.Get<FbxDouble3>().mData[1], lProperty.Get<FbxDouble3>().mData[2]);
                break;
            case eFbxInt:
                printf("%d", lProperty.Get<FbxInt>());
                break;
            case eFbxString:
                printf("%s", lProperty.Get<FbxString>().Buffer());
                break;
            }
            printf("\n");
        }


        Material material;
        material.name = fbx_material->GetName();
        MaterialCache::instance()->addMaterial(material);
    }

    //Texture 들 읽기
    int32 texture_count = root_scene->GetTextureCount();
    out_mesh->resizeMaterialSlot(texture_count);
    for (int32 i = 0; i < texture_count; ++i)
    {
        FbxTexture* fbx_texture = root_scene->GetTexture(i);
        FbxFileTexture* fbx_file_texture = FbxCast<FbxFileTexture>(fbx_texture);
        if (nullptr == fbx_file_texture) TOD_RETURN_TRACE(false);
        if (nullptr != fbx_file_texture->GetUserDataPtr()) TOD_RETURN_TRACE(false);
        out_mesh->getMaterial(i)->textures[0] = fbx_file_texture->GetFileName();
    }
    
    
    //root_scene 을 순회하면서 Node 를 구축
    this->vertexIndex = 0;
    this->load_fbx_node(root_scene->GetRootNode(), out_mesh);
    out_mesh->updateAABB();
    *mesh = out_mesh;
    
    return true;
}


//-----------------------------------------------------------------------------
void FbxLoaderPrivate::load_fbx_node(FbxNode* cur_fbx_node, Mesh* mesh)
{
    auto read_normal = [](FbxMesh * fbx_mesh, int32 ctrl_point_index, int32 vertex_index, Vector3 & out_normal)
    {
        if (fbx_mesh->GetElementNormalCount() < 1)
        {
            return false;
        }

        FbxGeometryElementNormal* fbx_normal = fbx_mesh->GetElementNormal(0);
        int32 ref_index = -1;
        switch (fbx_normal->GetMappingMode())
        {
        case FbxGeometryElement::eByPolygonVertex:
            switch (fbx_normal->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                ref_index = vertex_index;
                break;
            case FbxGeometryElement::eIndexToDirect:
                ref_index = fbx_normal->GetIndexArray().GetAt(vertex_index);                
                break;
            default:
                TOD_RETURN_TRACE(false);
            }
            break;
        }

        const auto& ref_normal = fbx_normal->GetDirectArray().GetAt(ref_index).mData;
        out_normal.set(
              static_cast<float>(ref_normal[0])
            , static_cast<float>(ref_normal[1])
            , static_cast<float>(ref_normal[2]));

        return true;
    };

    auto read_uv = [](FbxMesh * fbx_mesh, int32 ctrl_point_index, int32 texture_uv_index, Vector2& out_uv)
    {
        if (fbx_mesh->GetElementUVCount() < 1)
        {
            return false;
        }
        
        FbxGeometryElementUV* fbx_uv = fbx_mesh->GetElementUV(0);
        int32 ref_index = -1;
        switch (fbx_uv->GetMappingMode())
        {
        case FbxGeometryElement::eByControlPoint:
            switch (fbx_uv->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
                ref_index = ctrl_point_index;
                break;
            case FbxGeometryElement::eIndexToDirect:
                ref_index = fbx_uv->GetIndexArray().GetAt(ctrl_point_index);
                break;
            default:
                TOD_RETURN_TRACE(false);
            }
            break;
        case FbxGeometryElement::eByPolygonVertex:
            switch (fbx_uv->GetReferenceMode())
            {
            case FbxGeometryElement::eDirect:
            case FbxGeometryElement::eIndexToDirect:
                ref_index = texture_uv_index;
                break;
            default:
                TOD_RETURN_TRACE(false);
            }
            break;
        }

        const auto& ref_uv = fbx_uv->GetDirectArray().GetAt(ref_index).mData;
        out_uv.set(
              static_cast<float>(ref_uv[0])
            , static_cast<float>(ref_uv[1]));

        return true;
    };




    auto cur_mesh = cur_fbx_node->GetMesh();
    Mesh::SubMesh* sub_mesh = nullptr;
    if (nullptr != cur_mesh)
    {
        cur_mesh->RemoveBadPolygons();
        cur_mesh->GenerateNormals(false, false, true);


        sub_mesh = mesh->newSubMesh(cur_fbx_node->GetName());
        sub_mesh->materialIndex = cur_fbx_node->GetMaterialCount() > 0 ? 0 : -1;


        //Vertices
        Vector3 aabb_min(std::numeric_limits<float>::max());
        Vector3 aabb_max(std::numeric_limits<float>::min());
        uint32 num_ctrl_points = cur_mesh->GetControlPointsCount();
        sub_mesh->vb->create(num_ctrl_points, sizeof(SkinnedVertexLayout));
        int8* vb_ptr = nullptr;
        sub_mesh->vb->lock(&vb_ptr);
        SkinnedVertexLayout* vb_layout_original
            = reinterpret_cast<SkinnedVertexLayout*>(vb_ptr);
        for (uint32 i = 0; i < num_ctrl_points; ++i)
        {
            SkinnedVertexLayout* vb_layout_ptr = &vb_layout_original[i];
            const auto& p = cur_mesh->GetControlPointAt(i).mData;
            vb_layout_ptr->position.set(
                  static_cast<float>(p[0])
                , static_cast<float>(p[2])
                , static_cast<float>(p[1])
            );

            for (uint32 i = 0; i < 3; ++i)
            {
                if (vb_layout_ptr->position.array[i] < aabb_min.array[i])
                {
                    aabb_min.array[i] = vb_layout_ptr->position.array[i];
                }
                if (aabb_max.array[i] < vb_layout_ptr->position.array[i])
                {
                    aabb_max.array[i] = vb_layout_ptr->position.array[i];
                }
            }
        }
        sub_mesh->aabb.set(aabb_min, aabb_max);



        int32 num_vertices = cur_mesh->GetPolygonCount();
        for (int32 i = 0; i < num_vertices; ++i)
        {
            for (int32 j = 0; j < 3; ++j)
            {
                int32 ctrl_point_index = cur_mesh->GetPolygonVertex(i, j);
                SkinnedVertexLayout* vb_layout_ptr
                    = &vb_layout_original[ctrl_point_index];

                //Normal
                for (int32 l = 0; l < cur_mesh->GetElementNormalCount(); ++l)
                {
                    FbxGeometryElementNormal* fbx_normal = cur_mesh->GetElementNormal(l);
                    switch (fbx_normal->GetMappingMode())
                    {
                    case FbxGeometryElement::eByControlPoint:
                        switch (fbx_normal->GetReferenceMode())
                        {
                        case FbxGeometryElement::eDirect:
                        {
                            const auto& fbx_vec = fbx_normal->GetDirectArray().GetAt(ctrl_point_index);
                            vb_layout_ptr->normal.set(
                                  static_cast<float>(fbx_vec[0])
                                , static_cast<float>(fbx_vec[1])
                                , static_cast<float>(fbx_vec[2]));
                        }
                        break;
                        case FbxGeometryElement::eIndexToDirect:
                        {
                            int id = fbx_normal->GetIndexArray().GetAt(ctrl_point_index);
                            const auto& fbx_vec = fbx_normal->GetDirectArray().GetAt(id);
                            vb_layout_ptr->normal.set(
                                  static_cast<float>(fbx_vec[0])
                                , static_cast<float>(fbx_vec[1])
                                , static_cast<float>(fbx_vec[2]));
                        }
                        break;
                        default:
                            break;
                        }
                        break;
                    case FbxGeometryElement::eByPolygonVertex:
                        switch (fbx_normal->GetReferenceMode())
                        {
                        case FbxGeometryElement::eDirect:
                        {
                            const auto& fbx_vec = fbx_normal->GetDirectArray().GetAt(this->vertexIndex);
                            vb_layout_ptr->normal.set(
                                  static_cast<float>(fbx_vec[0])
                                , static_cast<float>(fbx_vec[1])
                                , static_cast<float>(fbx_vec[2]));
                        }
                        break;
                        case FbxGeometryElement::eIndexToDirect:
                        {
                            int id = fbx_normal->GetIndexArray().GetAt(this->vertexIndex);
                            const auto& fbx_vec = fbx_normal->GetDirectArray().GetAt(id);
                            vb_layout_ptr->normal.set(
                                  static_cast<float>(fbx_vec[0])
                                , static_cast<float>(fbx_vec[1])
                                , static_cast<float>(fbx_vec[2]));
                        }
                        break;
                        default:
                            break;
                        }
                    }
                }
                
                //Textures
                for (int32 l = 0; l < cur_mesh->GetElementUVCount(); ++l)
                {
                    FbxGeometryElementUV* fbx_uv = cur_mesh->GetElementUV(l);
                    switch (fbx_uv->GetMappingMode())
                    {
                    case FbxGeometryElement::eByControlPoint:
                        switch (fbx_uv->GetReferenceMode())
                        {
                        case FbxGeometryElement::eDirect:
                            {
                                const auto& fbx_vec = fbx_uv->GetDirectArray().GetAt(ctrl_point_index);
                                vb_layout_ptr->tex.set(
                                      static_cast<float>(fbx_vec[0])
                                    , static_cast<float>(fbx_vec[1]));
                            }
                            break;
                        case FbxGeometryElement::eIndexToDirect:
                            {
                                int id = fbx_uv->GetIndexArray().GetAt(ctrl_point_index);
                                const auto& fbx_vec = fbx_uv->GetDirectArray().GetAt(id);
                                vb_layout_ptr->tex.set(
                                      static_cast<float>(fbx_vec[0])
                                    , static_cast<float>(fbx_vec[1]));
                            }
                        break;
                        default:
                            break;
                        }
                        break;
                    case FbxGeometryElement::eByPolygonVertex:
                        {
                            int uv_index = cur_mesh->GetTextureUVIndex(i, j);
                            switch (fbx_uv->GetReferenceMode())
                            {
                            case FbxGeometryElement::eDirect:
                            case FbxGeometryElement::eIndexToDirect:
                                {
                                    const FbxVector2& fbx_vec = fbx_uv->GetDirectArray().GetAt(uv_index);
                                    vb_layout_ptr->tex.set(
                                          static_cast<float>(fbx_vec[0])
                                        , static_cast<float>(fbx_vec[1]));
                                }
                            break;
                            default:
                                break;
                            }
                        }
                    break;
                    default:
                        break;
                    }
                }
                read_uv(
                      cur_mesh
                    , ctrl_point_index
                    , cur_mesh->GetTextureUVIndex(i, j)
                    , vb_layout_ptr->tex);

                ++this->vertexIndex;
            }
        }
        sub_mesh->vb->unlock();


        //Indices
        {
            auto num_indices = cur_mesh->GetPolygonVertexCount();
            sub_mesh->ib->create(num_indices, sizeof(uint32));
            int8* ptr = nullptr;
            sub_mesh->ib->lock(&ptr);
            auto indices = cur_mesh->GetPolygonVertices();
            uint32* ib_ptr = reinterpret_cast<uint32*>(ptr);
            if (true == cur_mesh->CheckIfVertexNormalsCCW())
            {
                for (auto i = 0; i < num_indices; ++i)
                {
                    ib_ptr[i] = indices[num_indices - i - 1];
                }
            }
            else
            {
                for (auto i = 0; i < num_indices; ++i)
                {
                    ib_ptr[i] = indices[num_indices - i - 1];
                }
            }
            sub_mesh->ib->unlock();
        }
    }


    //자식노드를 탐색
    auto child_count = cur_fbx_node->GetChildCount();
    for (int i = 0; i < child_count; ++i)
    {
        auto child_fbx_node = cur_fbx_node->GetChild(i);
        this->load_fbx_node(child_fbx_node, mesh);
    }

    /*cur_node->setVisible(cur_fbx_node->GetVisibility());
    cur_node->setName(cur_fbx_node->GetName());
    
    
    //Transform Component
    auto transform_component = newInstance<TransformComponent>();
    cur_node->addComponent(transform_component);
    const auto& translation = cur_fbx_node->LclTranslation.Get();
    const auto& rotation = cur_fbx_node->LclRotation.Get();
    const auto& scaling = cur_fbx_node->LclScaling.Get();
    transform_component->setTranslation(
        Vector3(translation.mData[0],
                translation.mData[1],
                translation.mData[2]));
    transform_component->setRotation(
        Vector3(rotation.mData[0],
                rotation.mData[1],
                rotation.mData[2]));
    transform_component->setScaling(
        Vector3(scaling.mData[0],
                scaling.mData[1],
                scaling.mData[2]));
    
    
    //Mesh 정보가 있다면 읽는다
    auto cur_mesh = cur_fbx_node->GetMesh();
    if (cur_mesh)
    {
        cur_mesh->RemoveBadPolygons();
        this->conv->EmulateNormalsByPolygonVertex(cur_mesh);
    
    
        //Mesh Component 추가
        auto mesh_component = newInstance<MeshComponent>();
        auto mesh = mesh_component->getMesh();
        cur_node->addComponent(mesh_component);
        
        
        auto ctrl_pt_count = cur_mesh->GetControlPointsCount();
        mesh->setupVertexStruct(VertexDataOption()
            .add(VertexDataType::POSITION)
            .add(VertexDataType::NORMAL));
        mesh->lock(ctrl_pt_count);
        
        
        //Control Point
        for (auto j=0;j<ctrl_pt_count;++j)
        {
            const auto& data = cur_mesh->GetControlPointAt(j).mData;
            mesh->addVertexData(
                VertexDataType::POSITION,
                Vector3(data[0], data[1], data[2]));
            
            for (auto l=0;l<cur_mesh->GetElementNormalCount();++l)
            {
                auto normal = cur_mesh->GetElementNormal(l);
                switch (normal->GetReferenceMode())
                {
                case FbxGeometryElement::eDirect:
                    {
                        auto normal_info = normal->GetDirectArray().GetAt(j);
                        const auto& data = normal_info.mData;
                        mesh->setVertexData(
                            VertexDataType::NORMAL,
                            j,
                            Vector3(data[0], data[1], data[2]));
                    }
                    break;
                case FbxGeometryElement::eIndexToDirect:
                    {
                        int id = normal->GetIndexArray().GetAt(j);
                        auto normal_info = normal->GetDirectArray().GetAt(id);
                        const auto& data = normal_info.mData;
                        mesh->setVertexData(
                            VertexDataType::NORMAL, id,
                            Vector3(data[0], data[1], data[2]));
                    }
                    break;
                default:
                    break;
                }
            }
        }
        
        
        //Indices
        auto indices = cur_mesh->GetPolygonVertices();
        for (int i=0;i<cur_mesh->GetPolygonVertexCount();++i)
        {
            auto index = indices[i];
            mesh->addIndex(index);
        }
        
        
        int polygon_count = cur_mesh->GetPolygonCount();
        for (auto i=0;i<polygon_count;++i)
        {
            int polygon_size = cur_mesh->GetPolygonSize(i);
            for (auto j=0;j<polygon_size;++j)
            {
                int ctrl_pt_index = cur_mesh->GetPolygonVertex(i, j);
                for (auto l=0;l<cur_mesh->GetElementVertexColorCount();++l)
                {
                    auto color = cur_mesh->GetElementVertexColor(l);
                    switch (color->GetReferenceMode())
                    {
                    case FbxGeometryElement::eDirect:
                        {
                            auto color_info = color->GetDirectArray().GetAt(ctrl_pt_index);
                            int a=0;
                        }
                        break;
                    case FbxGeometryElement::eIndexToDirect:
                        {
                            int id = color->GetIndexArray().GetAt(ctrl_pt_index);
                            auto color_info = color->GetDirectArray().GetAt(id);
                            int a=0;
                        }
                        break;
                    default:
                        break;
                    }
                }
            }
        }
        
        
        mesh->unlock();
    }
    
    
    //자식노드를 탐색
    auto child_count = cur_fbx_node->GetChildCount();
    for (int i=0;i<child_count;++i)
    {
        auto child_fbx_node = cur_fbx_node->GetChild(i);
        
        auto child_node = newInstance<Node>();
        cur_node->addChild(child_node);
        this->load_fbx_node(child_fbx_node, child_node);
    }*/
}

}
