#include "tod/node.h"
namespace tod
{

//-----------------------------------------------------------------------------
const char* Node::EVENT_ADDCHILD = "EVENT_ADDCHILD";
const char* Node::EVENT_REMOVECHILD = "EVENT_ADDCHILD";
    

//-----------------------------------------------------------------------------
Node::Node():
parent(nullptr),
nameHash(0)
{
    this->setVisible(true);
    this->setSerializable(true);
}
    

//-----------------------------------------------------------------------------
Node::~Node()
{
    printf("%p\n", this);
}


//-----------------------------------------------------------------------------
int Node::release()
{
    TOD_ASSERT(this->refCount >= 0);

    if (this->refCount <= 1)
    {
        if (this->parent)
        {
            this->removeFromParent();
            return 0;
        }
        else
        {
            for (auto& child : this->children)
                child->parent = nullptr;
            this->children.clear();
        }
    }
    
    return BaseType::release();
}

    
//-----------------------------------------------------------------------------
bool Node::addChild(Node* node)
{
    if (node == this) TOD_THROW_EXCEPTION("can't add self as a child");
    node->parent = this;
    this->children.emplace_back(node);
    this->dispatchEvent(EVENT_ADDCHILD, Params::newParam(node));
    return true;
}
    
    
//-----------------------------------------------------------------------------
void Node::removeChild(Node* node)
{
    //EVENT dispatch 까지는 지정된 node 가 살아 있어야 한다
    node->retain();
    
    node->parent = nullptr;
    this->children.erase(std::remove_if(
        this->children.begin(),
        this->children.end(),
        [node](const ObjRef<Node>& child)
        { return child == node; }),
        this->children.end());
    this->dispatchEvent(EVENT_REMOVECHILD, Params::newParam(node));
    
    node->release();
}
    

//-----------------------------------------------------------------------------
void Node::removeChildByName(const String& name)
{
    auto child = this->findNodeByName(name, 0);
    if (nullptr == child) return;
    child->removeChild(child);
}


//-----------------------------------------------------------------------------
void Node::removeAllChildren()
{
    for (auto& n : this->children)
    {
        n->parent = nullptr;
        this->dispatchEvent(EVENT_REMOVECHILD, Params::newParam(n.get()));
    }
    this->children.clear();
}
    

//-----------------------------------------------------------------------------
void Node::removeFromParent()
{
    if (nullptr == this->parent) return;
    this->parent->removeChild(this);
}
    

//-----------------------------------------------------------------------------
Node* Node::find_node_by_name
(Node* parent, int name_hash, int depth, int limit_depth)
{
    if (parent->nameHash == name_hash) return const_cast<Node*>(parent);
    if (depth == limit_depth) return nullptr;
    
    for (auto& child : parent->children)
    {
        if (child->isHidden()) continue;
        auto find_it = Node::find_node_by_name(
            child.get(), name_hash, depth + 1, limit_depth);
        if (nullptr != find_it) return find_it;
    }
    return nullptr;
};


//-----------------------------------------------------------------------------
Node* Node::findNodeByName(const String& name, int depth)
{
    auto name_hash = name.hash();
    for (auto& child : this->children)
    {
        if (child->isHidden()) continue;
        auto find_it = Node::find_node_by_name(child.get(), name_hash, 0, depth);
        if (nullptr != find_it) return find_it;
    }
    return nullptr;
}


//-----------------------------------------------------------------------------
Node* Node::getChildAt(int index)
{
    if (index < 0 || index >= this->children.size())
        TOD_RETURN_TRACE(nullptr);
    return this->children[index];
}

    
//-----------------------------------------------------------------------------
String Node::getAbsolutePath() const
{
    String path { S("/") };
    
    const Node* cur = this;
    std::vector<const Node*> upward;
    upward.reserve(32);
    size_t len = 0;
    while (cur && cur->parent != nullptr)
    {
        len += cur->name.size() + 1;
        upward.push_back(cur);
        cur = cur->parent;
    }
    
    path.reserve(len);
    int s = static_cast<int>(upward.size()-1);
    for (int i=s;i>=0;--i)
    {
        if (i < s) path += S("/");
        path += upward[i]->name;
    }
    
    return path;
}


//-----------------------------------------------------------------------------
int Node::indexOf(Node* child_node)
{
    int count = static_cast<int>(this->children.size());
    for (int i=0;i<count;++i)
    {
        if (this->children.at(i).equal(child_node))
            return i;
    }
    return -1;
}


//-----------------------------------------------------------------------------
int Node::getSelfIndex()
{
    if (nullptr == this->parent) return -1;
    return this->parent->indexOf(this);
}
    

//-----------------------------------------------------------------------------
void Node::addComponent(Component* component)
{
    this->components.push_back(component);
    component->node = this;
}


//-----------------------------------------------------------------------------
void Node::removeComponentByName(const String& name)
{
    auto name_hash = name.hash();
    this->components.erase(std::remove_if(
        this->components.begin(),
        this->components.end(),
        [name_hash](const ObjRef<Component>& component)
        { return component->getType()->getName().hash() == name_hash; }));
}


//-----------------------------------------------------------------------------
void Node::resetComponentByName(const tod::String &name)
{
    auto name_hash = name.hash();
    for (auto& c : this->components)
    {
        if (c->getType()->getName().hash() == name_hash)
            c->resetAllProperties();
    }
}


//-----------------------------------------------------------------------------
void Node::moveComponent(int index)
{

}


//-----------------------------------------------------------------------------
Component* Node::findComponentByName(const String& name)
{
    auto name_hash = name.hash();
    for (auto& c : this->components)
    {
        if (c->getType()->getName().hash() == name_hash)
            return c;
    }
    TOD_RETURN_TRACE(nullptr);
}

    
//-----------------------------------------------------------------------------
Node* Node::getRelativeNode(const String& path)
{
    std::vector<String> path_list;
    path.split("/", path_list);
    
    Node* cur = this;
    for (size_t i=0;i<path_list.size();++i)
    {
        const String& cur_name = path_list[i];
        if (cur_name == S(".")) {}
        else if (cur_name == S("..")) { cur = cur->parent; }
        else { cur = cur->findNodeByName(cur_name, 0); }
        if (nullptr == cur) return nullptr;
    }
    
    return cur;
}
    

//-----------------------------------------------------------------------------
void Node::setName(const String& name)
{
    this->name = name;
    this->nameHash = name.hash();
}
    

//-----------------------------------------------------------------------------
void Node::bindProperty()
{
    BIND_PROPERTY(const String&, "name", "",
                  setName, getName, String(), PropertyAttr::DEFAULT);
    BIND_PROPERTY(bool, "visible", "",
                  setVisible, isVisible, true, PropertyAttr::DEFAULT);
    BIND_PROPERTY(bool, "hidden", "",
                  setHidden, isHidden, false, PropertyAttr::DEFAULT);
    BIND_PROPERTY(bool, "serializable", "",
                  setSerializable, isSerializable, true, PropertyAttr::DEFAULT);
}


//-----------------------------------------------------------------------------
void Node::bindMethod()
{
}
    
}
