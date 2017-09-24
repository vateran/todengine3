#pragma once
#include "tod/object.h"
#include "tod/objref.h"
namespace tod
{

class Node;
  
//!@ingroup Core
//!@brief TodEngine에서 Node에 기능을 부여하는 기능을 관리하는 인터페이스
class Component : public Derive<Component, Object>
{
public:
    Component():node(nullptr) {}

    inline Node* getNode() { return this->node; }

private:
    Node* node;
    friend class Node;
};

}
