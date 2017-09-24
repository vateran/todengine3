#include "tod/serializer.h"
#include "tod/node.h"
#include "tod/kernel.h"
#include "tod/filesystem.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>
namespace tod
{
 
//-----------------------------------------------------------------------------
bool Serializer::serializeToJson(Node* node, String& json_str_out)
{
    rapidjson::Document doc(rapidjson::kObjectType);
    
    this->write_node_to_json(node, doc, doc.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    json_str_out = buffer.GetString();
    return true;
}



//-----------------------------------------------------------------------------
bool Serializer::serializeToJsonFile(Node* node, const String& uri)
{
    String json_str;
    if (!this->serializeToJson(node, json_str)) TOD_RETURN_TRACE(false);
    
    if (!FileSystem::instance()->save(uri, &json_str[0], json_str.size()))
        TOD_RETURN_TRACE(false);

    return true;
}

    
//-----------------------------------------------------------------------------
Node* Serializer::deserializeFromJson(const tod::String &json_str)
{
    rapidjson::Document doc;
    if (doc.Parse(json_str.c_str(), json_str.size()).HasParseError())
    {
        TOD_THROW_EXCEPTION("deserializeFromJson(%d): %s",
                            doc.GetErrorOffset(),
                            rapidjson::GetParseError_En(doc.GetParseError()));
        TOD_RETURN_TRACE(nullptr);
    }
    
    auto node = this->read_node_from_json(doc);
    return node;
}
    

//-----------------------------------------------------------------------------
Node* Serializer::deserializeFromJsonFile(const String& uri)
{
    Node* ret = nullptr;
    
    if (!FileSystem::instance()->load(uri, [this, &ret](FileSystem::Data& data)
    { ret = this->deserializeFromJson(std::move(&data[0])); return true; },
    FileSystem::LoadOption().string())) TOD_RETURN_TRACE(nullptr);
    
    return ret;
}


//-----------------------------------------------------------------------------
void Serializer::write_object_to_json
(Object* object, rapidjson::Value& json,
 rapidjson::MemoryPoolAllocator<>& allocator)
{
    json.AddMember(
        "type",
        rapidjson::Value(object->getType()->getName().c_str(), allocator),
        allocator);
    
    this->write_prop_to_json(object, json, allocator);
}
    

//-----------------------------------------------------------------------------
void Serializer::write_node_to_json
(Node* node, rapidjson::Value& json,
 rapidjson::MemoryPoolAllocator<>& allocator)
{
    if (!node->isSerializable()) return;
    
    node->onBeginSerialize();
    
    json.AddMember(
        "type",
        rapidjson::Value(node->getType()->getName().c_str(), allocator),
        allocator);
    
    this->write_prop_to_json(node, json, allocator);
    
    auto& components = node->getComponents();
    if (!components.empty())
    {
        uint32 count = 0;
        rapidjson::Value json_components(rapidjson::kArrayType);
        for (auto i=components.begin();i!=components.end();++i, ++count)
        {
            rapidjson::Value json_comp(rapidjson::kObjectType);
            this->write_object_to_json(*i, json_comp, allocator);
            json_components.PushBack(json_comp, allocator);
        }
        json.AddMember("comp", json_components, allocator);
    }
    
    auto& children = node->getChildren();
    if (!children.empty())
    {
        uint32 count = 0;
        rapidjson::Value json_children(rapidjson::kArrayType);
        for (auto i=children.begin();i!=children.end();++i, ++count)
        {
            rapidjson::Value json_child(rapidjson::kObjectType);
            this->write_node_to_json(*i, json_child, allocator);
            json_children.PushBack(json_child, allocator);
        }
        json.AddMember("child", json_children, allocator);
    }
}

    
//-----------------------------------------------------------------------------
void Serializer::write_prop_to_json
(Object* object, rapidjson::Value& json,
 rapidjson::MemoryPoolAllocator<>& allocator)
{
    //static properties
    auto type = object->getType();
    
    rapidjson::Value json_prop(rapidjson::kObjectType);
    
    bool exist_prop = false;
    for (auto& i : type->getAllPropertyOrder())
    {
        if (!i->isFlagOn(static_cast<int>(PropertyAttr::WRITE)))
            continue;
        if (!i->isFlagOn(static_cast<int>(PropertyAttr::SERIALIZABLE)))
            continue;
        if (i->isDefaultValue(object))
            continue;
        
        String out;
        i->toString(object, out);
        json_prop.AddMember(
            rapidjson::Value(i->getName().c_str(), allocator),
            rapidjson::Value(out.c_str(), allocator),
            allocator);
        exist_prop = true;
    }
    
    if (exist_prop)
        json.AddMember("prop", json_prop, allocator);
}
    
    
//----------------------------------------------------------------------------
Node* Serializer::read_node_from_json(rapidjson::Value& jval)
{
    if (jval.IsNull()) TOD_RETURN_TRACE(nullptr);
    
    const auto& type_iter = jval.FindMember("type");
    if (type_iter == jval.MemberEnd()) TOD_RETURN_TRACE(nullptr);
    
    auto type = Kernel::instance()->findType(type_iter->value.GetString());
    if (nullptr == type) TOD_RETURN_TRACE(nullptr);
    
    auto node = static_cast<Node*>(type->createObject());
    if (nullptr == node) TOD_RETURN_TRACE(nullptr);
    node->onBeginDeserialize();
    
    this->read_prop_from_json(node, jval);
    
    const auto& component_iter = jval.FindMember("comp");
    if (jval.MemberEnd() != component_iter)
    {
        auto& components = component_iter->value;
        if (components.IsArray())
        {
            for (auto i=components.Begin();i!=components.End();++i)
            {
                const auto& comp_type_iter = (*i).FindMember("type");
                if (comp_type_iter == (*i).MemberEnd()) continue;
                
                auto comp = static_cast<Component*>(Kernel::instance()->
                    create(comp_type_iter->value.GetString()));
                node->addComponent(comp);
                
                this->read_prop_from_json(comp, (*i));
            }
        }
    }
    
    const auto& children_iter = jval.FindMember("child");
    if (jval.MemberEnd() != children_iter)
    {
        auto& children = children_iter->value;
        if (children.IsArray())
        {
            for (auto i=children.Begin();i!=children.End();++i)
            {
                Node* child_node = this->read_node_from_json(*i);
                if (child_node == 0) continue;
                child_node->retain();
                node->addChild(child_node);
                child_node->release();
            }
        }
    }
    
    return node;
}
    
    
//----------------------------------------------------------------------------
void Serializer::read_prop_from_json(Object* object, rapidjson::Value& jval)
{
    if (jval.IsNull()) return;
    
    //static properties
    const auto& prop_jval_iter = jval.FindMember("prop");
    if (jval.MemberEnd() != prop_jval_iter)
    {
        auto& prop_jval = prop_jval_iter->value;
        auto type = object->getType();
        std::unordered_map<Property*,
            rapidjson::Value::MemberIterator> prop_value_list;
        for (auto i=prop_jval.MemberBegin();i!=prop_jval.MemberEnd();++i)
        {
            auto prop = type->findProperty(i->name.GetString());
            if (prop == 0) continue;
            prop_value_list.insert(std::make_pair(prop, i));
        }
        
        for (auto& i : type->getAllPropertyOrder())
        {
            if (i->isReadOnly()) continue;
            auto f = prop_value_list.find(i);
            if (f == prop_value_list.end()) continue;
            f->first->fromString(object, (f->second->value).GetString());
        }
    }
}
    
}
