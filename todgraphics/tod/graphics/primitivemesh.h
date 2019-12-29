#pragma once
#include "tod/singleton.h"
namespace tod::graphics
{

///@brief Primitive Mesh 들을 초기화 해준다
///초기화 이름
///* "quad"
class PrimitiveMesh : public Singleton<PrimitiveMesh>
{
public:
    PrimitiveMesh();
};

}
