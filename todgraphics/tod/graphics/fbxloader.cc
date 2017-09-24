#include "tod/graphics/fbxloader.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/mesh.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/meshcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include <fbxsdk.h>
#include <fbxsdk/utils/FbxGeometryConverter.h>
namespace tod::graphics
{


class FbxLoaderPrivate
{
public:
    FbxLoaderPrivate();
    ~FbxLoaderPrivate();
    
    Node* load(const String& uri);
    
private:
    void load_fbx_node(FbxNode* cur_fbx_node, Node* cur_node);

private:
    FbxManager* fbxmgr;
    FbxIOSettings* ios;
    FbxGeometryConverter* conv;
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
    FileSystem::Data data;
    FbxInt64 pos;
    int readerID;
};


//-----------------------------------------------------------------------------
TodFbxStream::TodFbxStream(FbxManager* fbxmgr, const String& uri):
  pos(0)
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
        if (!FileSystem::instance()->load(this->uri, this->data))
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
int TodFbxStream::Read (void* pData, int pSize) const
{
    if (this->data.empty()) return 0;
    if (this->data.size() <= this->pos) return 0;
    auto read_size = std::min(pSize,
        static_cast<int>(this->data.size() - this->pos));
    std::memcpy(pData, &this->data[this->pos], read_size);
    const_cast<TodFbxStream*>(this)->pos += read_size;
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
    return this->pos;
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
    SAFE_DELETE(this->inner);
}
    
    
//-----------------------------------------------------------------------------
Node* FbxLoader::load(const String& uri)
{
    return this->inner->load(uri);
}


//-----------------------------------------------------------------------------
FbxLoaderPrivate::FbxLoaderPrivate():
  fbxmgr(nullptr)
, ios(nullptr)
, conv(nullptr)
{
    this->fbxmgr = FbxManager::Create();
    this->ios = FbxIOSettings::Create(this->fbxmgr, IOSROOT);
    this->fbxmgr->SetIOSettings(this->ios);
    this->conv = new FbxGeometryConverter(this->fbxmgr);
}


//-----------------------------------------------------------------------------
FbxLoaderPrivate::~FbxLoaderPrivate()
{
    SAFE_DELETE(this->conv);
}


//-----------------------------------------------------------------------------
Node* FbxLoaderPrivate::load(const String& uri)
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
    importer->Import(root_scene);
    importer->Destroy();
    
    this->conv->Triangulate(root_scene, true);
    
    
    //root_scene 을 순회하면서 Node 를 구축
    auto root_node = newInstance<Node>();
    this->load_fbx_node(root_scene->GetRootNode(), root_node);
    
    
    return root_node;
}


//-----------------------------------------------------------------------------
void FbxLoaderPrivate::load_fbx_node(FbxNode* cur_fbx_node, Node* cur_node)
{
    cur_node->setVisible(cur_fbx_node->GetVisibility());
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
    }
}

}
