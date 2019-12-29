#pragma once
#include <array>
#include "tod/object.h"
#include "tod/enumoption.h"
#include "tod/graphics/color.h"
#include "tod/graphics/colorf.h"
#include "tod/graphics/vector2.h"
#include "tod/graphics/vector3.h"
namespace tod::graphics
{

enum class VertexDataType : int
{
    POSITION,
    NORMAL,
    TANGENT,
    TEXCOORD,
    
    MAX

};
             

typedef EnumOption<VertexDataType> VertexDataOption;

    
class Mesh : public Derive<Mesh, Object>
{
public:
    enum DrawType
    {
        DRAWTYPE_TRIANGLES,
        DRAWTYPE_TRIANGLE_STRIP,
    };

    struct VertexStructMetaInfo
    {
        VertexStructMetaInfo(
            VertexDataType type, int64 factor_type,
            uint8 data_size, uint8 factor_count):
            type(type), factorType(factor_type),
            dataSize(data_size), factorCount(factor_count) {}
        
        VertexDataType type;        //VertexDataType
        int64 factorType;           //타입의 요소 타입 ex) TypeId<float>::getId()
        uint8 dataSize;             //Vertex구조에 들어가는 Composit타입의 sizeof
        uint8 factorCount;          //타입의 요소 count, ex) Vector3 = 3
    };
    struct VertexStructInfo
    {
        VertexStructInfo():
            metaInfo(nullptr), writePos(0),
            enable(false), startOffset(0)  {}
        
        VertexStructMetaInfo* metaInfo;
        int32 writePos;
        bool enable;
        uint16 startOffset;
    };

public:
    Mesh();
    virtual~Mesh();
    
    void setupVertexStruct(VertexDataOption& option);
    virtual void setDrawType(DrawType value);
    
    virtual void* lock(int32 vertex_count, int32 offset=0)=0;
    virtual void unlock()=0;
    
    template <typename T>
    void addVertexData(VertexDataType type, const T& data);
    template <typename T>
    void setVertexData(VertexDataType type, int32 index, const T& data);
    virtual void setVertexDataPtr(VertexStructInfo& type, int32 index, void* data)=0;
    void addIndex(uint32 index) { this->indices.push_back(index); }
    uint32 getIndex(uint32 index) { return this->indices[index]; }
    uint32 getIndexSize() { return static_cast<uint32>(this->indices.size()); }
    const String& getName() const { return this->name; }
    
    virtual void render()=0;
    virtual void load()=0;
    virtual void unload()=0;

protected:
    String name;
    DrawType drawType;
    VertexDataOption vertexStructOption;
    std::array<VertexStructInfo,
        static_cast<size_t>(VertexDataType::MAX)> vertexStructInfo;
    std::vector<uint32> indices;
    int32 stride;
    int32 vertexCount;

};


template <typename T>
void Mesh::addVertexData(VertexDataType type, const T& data)
{
    auto& info = this->vertexStructInfo[static_cast<int>(type)];
    this->setVertexDataPtr(info, info.writePos, (void*)(&data));
    ++info.writePos;
}


template <typename T>
void Mesh::setVertexData(VertexDataType type, int32 index, const T& data)
{
    auto& info = this->vertexStructInfo[static_cast<int>(type)];
    this->setVertexDataPtr(info, index, (void*)(&data));
}

}
