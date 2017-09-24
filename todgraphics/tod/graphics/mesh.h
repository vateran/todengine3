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


template <typename T>
class TypeId
{
public:
    typedef TypeId<T> type;
    static int64 getId()
    {
        static std::decay<T> s_instance;
        return (int64)&s_instance;
    }
    template <typename S>
    static bool equal()
    {
        return type::getId() == TypeId<S>::getId();
    }
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
        int writePos;
        bool enable;
        uint16 startOffset;
    };

public:
    Mesh();
    virtual~Mesh();
    
    void setupVertexStruct(VertexDataOption& option);
    virtual void setDrawType(DrawType value);
    
    virtual void* lock(int vertex_count, int offset=0)=0;
    virtual void unlock()=0;
    
    template <typename T>
    void addVertexData(VertexDataType type, const T& data);
    template <typename T>
    void setVertexData(VertexDataType type, int index, const T& data);
    virtual void setVertexDataPtr(VertexStructInfo& type, int index, void* data)=0;
    void addIndex(uint32 index) { this->indices.push_back(index); }
    uint32 getIndex(uint32 index) { return this->indices[index]; }
    uint32 getIndexSize() { return this->indices.size(); }
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
    int stride;
    int vertexCount;

};


template <typename T>
void Mesh::addVertexData(VertexDataType type, const T& data)
{
    auto& info = this->vertexStructInfo[static_cast<int>(type)];
    this->setVertexDataPtr(info, info.writePos, (void*)(&data));
    ++info.writePos;
}


template <typename T>
void Mesh::setVertexData(VertexDataType type, int index, const T& data)
{
    auto& info = this->vertexStructInfo[static_cast<int>(type)];
    this->setVertexDataPtr(info, index, (void*)(&data));
}

}
