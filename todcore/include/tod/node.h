#pragma once
#include <vector>
#include <bitset>
#include "tod/object.h"
#include "tod/component.h"
#include "tod/objref.h"
#include "tod/eventdispatcher.h"
namespace tod
{
    
//!@ingroup Core
//!@brief NOH(Node of Hierarchy)의 최상위 객체
//!TodEngine상의 객체 계층구조의 구성요소
//! - 이름 지정가능
//! - Event Handling
//! - Serialization 단위
//! - Component 조합
class Node : public EventDispatcher<Derive<Node, Object>>
{
public:
    static const char* EVENT_ADDCHILD;
    static const char* EVENT_REMOVECHILD;
    
public:
    enum Flag
    {
        VISIBLE,
        HIDDEN,
        SERIALIZABLE,
        
        FLAG_MAX,
    };
    
public:
    typedef std::vector<ObjRef<Node>> Nodes;
    typedef std::vector<ObjRef<Component>> Components;
    
public:
    Node();
    virtual~Node();
    
    //!@brief Node Hierarchy
    //@{
    bool addChild(Node* node);
    void removeChild(Node* node);
    void removeChildByName(const String& name);
    void removeAllChildren();
    void removeFromParent();
    //!@brief 지정된 이름의 Node 를 찾는다
    //!@param name 찾을 이름
    //!@param depth 지정된 depth 이상 tree 탐색을 하지 않는다
    Node* findNodeByName(const String& name, int depth=-1) const;
    //!@brief 지정된 상대경로의 Node를 반환
    Node* getRelativeNode(const String& path) const;
    //!@brief 자식 노드의 갯수를 반환
    int getChildCount() const
    { return static_cast<int>(this->children.size()); }
    //!@brief 자식 노드 전체를 반환
    Nodes& getChildren() { return this->children; }
    //!@brief root Node 로 부터 절대 경로를 반환
    String getAbsolutePath() const;
    Node* getParent() { return this->parent; }
    //@}
    
    //!@brief Component System
    //@{
    void addComponent(Component* component);
    void removeComponentByName(const String& name);
    void resetComponentByName(const String& name);
    void moveComponent(int index);
    template <typename T>
    T* findComponent();
    Component* findComponentByName(const String& name);
    //@}

    void setName(const String& name);
    const String& getName() const { return this->name; }
    void setVisible(bool value) { this->flags[VISIBLE] = value; }
    bool isVisible() const { return this->flags[VISIBLE]; }
    void setHidden(bool value) { this->flags[HIDDEN] = value; }
    bool isHidden() const { return this->flags[HIDDEN]; }
    void setSerializable(bool value) { this->flags[SERIALIZABLE] = value; }
    bool isSerializable() const { return this->flags[SERIALIZABLE]; }
    
    static void bindProperty();
    static void bindMethod();
    
    //!@brief internal events
    //@{
    virtual void onBeginSerialize() {}
    virtual void onEndSerialize() {}
    virtual void onBeginDeserialize() {}
    virtual void onEndDeserialize() {}
    //@}
    
private:
    Node* parent;
    String name;
    int nameHash;
    Nodes children;
    Components components;
    std::bitset<FLAG_MAX> flags;
    
private:
    static Node* find_node_by_name(const Node* parent, int name_hash,
                                   int depth, int limit_depth);
};
    
    
template <typename T>
T* Node::findComponent()
{
    for (auto& i : this->components)
    {
        auto component = dynamic_cast<T*>(i.get());
        if (nullptr != component) return component;
    }
    return nullptr;
}
    
}
