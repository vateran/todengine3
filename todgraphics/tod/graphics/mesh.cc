#include "tod/graphics/mesh.h"
namespace tod::graphics
{

//-----------------------------------------------------------------------------
Mesh::Mesh():
  drawType(DRAWTYPE_TRIANGLES)
, stride(0)
, vertexCount(0)
{
}


//-----------------------------------------------------------------------------
Mesh::~Mesh()
{
}


//-----------------------------------------------------------------------------
void Mesh::setupVertexStruct(VertexDataOption& option)
{
    typedef std::unordered_map<int, VertexStructMetaInfo> VertexStructMetaInfos;
    static VertexStructMetaInfos s_vertex_struct_meta_infos;
    if (s_vertex_struct_meta_infos.empty())
    {
        s_vertex_struct_meta_infos.insert(std::make_pair(
            static_cast<int>(VertexDataType::POSITION),
            VertexStructMetaInfo(
                VertexDataType::POSITION, TypeId<float>::getId(),
                sizeof(Vector3), 3)));
        s_vertex_struct_meta_infos.insert(std::make_pair(
            static_cast<int>(VertexDataType::NORMAL),
            VertexStructMetaInfo(
                VertexDataType::NORMAL, TypeId<float>::getId(),
                sizeof(Vector3), 3)));
        s_vertex_struct_meta_infos.insert(std::make_pair(
            static_cast<int>(VertexDataType::TANGENT),
            VertexStructMetaInfo(
                VertexDataType::TANGENT, TypeId<float>::getId(),
                sizeof(Vector3), 3)));
        s_vertex_struct_meta_infos.insert(std::make_pair(
            static_cast<int>(VertexDataType::TEXCOORD),
            VertexStructMetaInfo(
                VertexDataType::TEXCOORD, TypeId<float>::getId(),
                sizeof(Vector2), 2)));
    }
    
    this->vertexStructOption = option.getOption();

    int start_offset = 0;
    this->stride = 0;
    memset(&this->vertexStructInfo, 0, sizeof(this->vertexStructInfo));
    for (int i=0;i<static_cast<int>(VertexDataType::MAX);++i)
    {
        if (option.isOn(static_cast<VertexDataType>(i)))
        {
            auto& info = this->vertexStructInfo[i];
            auto meta_info_iter = s_vertex_struct_meta_infos.find(i);
            if (s_vertex_struct_meta_infos.end() == meta_info_iter) continue;
            info.metaInfo = &(meta_info_iter->second);
            info.enable = true;
            info.startOffset = start_offset;
            
            start_offset += info.metaInfo->dataSize;
            this->stride += info.metaInfo->dataSize;
        }
    }
}


//-----------------------------------------------------------------------------
void Mesh::setDrawType(DrawType value)
{
    this->drawType = value;
}

}
