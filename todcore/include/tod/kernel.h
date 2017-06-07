#pragma once
#include <unordered_map>
#include "tod/singleton.h"
#include "tod/string.h"
#include "tod/exception.h"
#include "tod/node.h"
namespace tod
{

class Type;

//!@brief Type 을 관리하고, Object를 생성하거나 NOH(Node of Hiearchy)에 Node를 생성
//!또한 NOH에 생성된 객체를 lookup
//!@ingroup Core
class Kernel : public Singleton<Kernel>
{
public:
    Kernel();
    virtual~Kernel();
    
    void init();
    
    //!@name Object Hierarchy Management
    //@{
    //!@brief 지정된 이름의 Object를 생성
    Object* create(const String& type_name);
    //!@brief 지정된 type_name의 Type으로 NOH에 Node객체를 생성
    Node* create(const String& type_name, const String& name);
    //!@brief 지정된 path에 위치한 Node객체를 반환
    template <typename T=Node>
    T* lookup(const String& path);
    //@}
    
    //!@name Type reigstry
    //@{
    //!@brief Type을 등록
    void addType(Type* type);
    //!@brief 지정된 이름의 Type을 반환
    Type* findType(const String& type_name);
    //@}
    
    void setRootNode(Node* root_node);
    Node* getRootNode() { return this->root; }
    
private:
    void register_buildtins();
    
private:
    typedef std::unordered_map<int, Type*> Types;
    Types types;
    Node* root;
    
};
    

//-----------------------------------------------------------------------------
template <typename T>
T* Kernel::lookup(const String& path)
{
    if (path.empty()) return nullptr;
    return static_cast<T*>(this->root->getRelativeNode(path));
}
    
}
