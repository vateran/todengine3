#include "tod/kernel.h"
#include <functional>
#include "tod/type.h"
#include "tod/node.h"
namespace tod
{

//-----------------------------------------------------------------------------
Kernel::Kernel():
root(nullptr)
{
    this->setRootNode(new Node());
}
    
    
//-----------------------------------------------------------------------------
Kernel::~Kernel()
{
    for (auto& t : this->types) delete t.second;
    this->setRootNode(nullptr);
}
    

//-----------------------------------------------------------------------------
void Kernel::init()
{
    this->register_buildtins();
}


//-----------------------------------------------------------------------------
Object* Kernel::create(const String& type_name)
{
    auto i = this->types.find(type_name.hash());
    if (this->types.end() == i) TOD_RETURN_TRACE(nullptr);
    return i->second->createObject();
}
    

//-----------------------------------------------------------------------------
Node* Kernel::create(const String& type_name, const String& name)
{
    std::vector<String> path_list;
    name.split("/", path_list);
    
    Node* cur_parent = this->root;
    Node* child_node = nullptr;
    int path_size = static_cast<int>(path_list.size());
    for (int path_pos=0;path_pos<path_size;++path_pos)
    {
        auto& cur_path_name = path_list[path_pos];
        child_node = cur_parent->findNodeByName(cur_path_name);
        if (nullptr == child_node)
        {
            if (path_pos == path_size-1)
            {
                child_node = static_cast<Node*>(this->create(type_name));
                if (nullptr == child_node) return nullptr;
            }
            else
            {
                child_node = newInstance<Node>();
            }
            
            child_node->setName(cur_path_name);
            cur_parent->addChild(child_node);
        }
        
        cur_parent = child_node;
    }
    
    return child_node;
}


//-----------------------------------------------------------------------------
void Kernel::addType(Type* type)
{
    auto i = this->types.find(type->getNameHash());
    if (this->types.end() != i)
        TOD_THROW_EXCEPTION("already exist type name(%s)",
                            type->getName().c_str());
    this->types.insert(std::make_pair(type->getNameHash(), type));
}
    

//-----------------------------------------------------------------------------
Type* Kernel::findType(const String& type_name)
{
    auto i = this->types.find(type_name.hash());
    if (this->types.end() == i) TOD_RETURN_TRACE(nullptr);
    return i->second;
}
    

//-----------------------------------------------------------------------------
void Kernel::setRootNode(tod::Node *root_node)
{
    if (nullptr != this->root) this->root->release();
    this->root = root_node;
    if (nullptr != this->root) this->root->retain();
}
    

//-----------------------------------------------------------------------------
void Kernel::register_buildtins()
{
    REGISTER_TYPE(Node);
}
    
}
