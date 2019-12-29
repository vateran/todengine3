#pragma once
#include <bitset>
#include "tod/object.h"
#include "tod/objref.h"
#include "tod/component.h"
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
    
    int32 release() override;
    
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
    Node* findNodeByName(const String& name, int32 depth=-1);
    //!@brief 지정된 상대경로의 Node를 반환
    Node* getRelativeNode(const String& path);
    //!@brief 자식 노드의 갯수를 반환
    int32 getChildCount() const
    { return static_cast<int>(this->children.size()); }
    //!@brief 자식 노드 전체를 반환
    Nodes& getChildren() { return this->children; }
    Node* getChildAt(int32 index);
    //!@brief root Node 로 부터 절대 경로를 반환
    String getAbsolutePath() const;
    Node* getParent() { return this->parent; }
    int32 indexOf(Node* child_node);
    int32 getSelfIndex();
    //@}
    
    //!@brief Component System
    //@{
    void addComponent(Component* component);
    void removeComponentByName(const String& name);
    void resetComponentByName(const String& name);
    void moveComponent(int32 index);
    template <typename T>
    T* findComponent();
    template <typename T>
    bool hasComponent();
    Component* findComponentByName(const String& name);
    Components& getComponents() { return this->components; }
    //@}

    void setName(const String& name);
    const String& getName() { return this->name; }
    void setVisible(bool value) { this->flags[VISIBLE] = value; }
    bool isVisible() { return this->flags[VISIBLE]; }
    void setHidden(bool value) { this->flags[HIDDEN] = value; }
    bool isHidden() { return this->flags[HIDDEN]; }
    void setSerializable(bool value) { this->flags[SERIALIZABLE] = value; }
    bool isSerializable() { return this->flags[SERIALIZABLE]; }
    
    //!@brief internal events
    //@{
    virtual void onBeginSerialize() {}
    virtual void onEndSerialize() {}
    virtual void onBeginDeserialize() {}
    virtual void onEndDeserialize() {}
    //@}
    
    static void bindProperty();
    static void bindMethod();
    
private:
    Node* parent;
    String name;
    Nodes children;
    Components components;
    int32 nameHash;
    std::bitset<FLAG_MAX> flags;
    
private:
    static Node* find_node_by_name(Node* parent, int32 name_hash,
                                   int32 depth, int32 limit_depth);
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

template <typename T>
bool Node::hasComponent()
{
    //일단은 임시로 findComponent 로 구현
    return (this->findComponent<T>() != nullptr);
}
    
}
