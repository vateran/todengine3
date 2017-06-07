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
    
    this->write_to_json(node, doc, doc.GetAllocator());
    
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);
    json_str_out = buffer.GetString();
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
        return nullptr;
    }
    
    auto node = this->read_from_json(doc);
    return node;
}
    

//-----------------------------------------------------------------------------
Node* Serializer::deserializeFromJsonFile(const String& uri)
{
    Node* ret = nullptr;
    
    if (!FileSystem::instance()->load(uri, [this, &ret](FileSystem::Data& data)
    { ret = this->deserializeFromJson(std::move(&data[0])); return true; },
    FileSystem::LoadOption().string())) return nullptr;
    
    return ret;
}
    

//-----------------------------------------------------------------------------
void Serializer::write_to_json
(Node* node, rapidjson::Value& json,
 rapidjson::MemoryPoolAllocator<>& allocator)
{
    if (!node->isSerializable()) return;
    
    node->onBeginSerialize();
    
    json.AddMember(
        "type",
        rapidjson::Value(node->getType()->getName().c_str(), allocator),
        allocator);
    
    this->write_to_json_prop(node, json, allocator);
    
    auto& children = node->getChildren();
    if (children.size())
    {
        unsigned int count = 0;
        rapidjson::Value json_children(rapidjson::kArrayType);
        for (auto i=children.begin();i!=children.end();++i, ++count)
        {
            rapidjson::Value json_child(rapidjson::kObjectType);
            this->write_to_json(*i, json_child, allocator);
            json_children.PushBack(json_child, allocator);
        }
        json.AddMember("child", json_children, allocator);
    }
}

    
//-----------------------------------------------------------------------------
void Serializer::write_to_json_prop
(Node* node, rapidjson::Value& json,
 rapidjson::MemoryPoolAllocator<>& allocator)
{
    //static properties
    auto type = node->getType();
    
    rapidjson::Value json_prop(rapidjson::kObjectType);
    
    bool exist_prop = false;
    for (auto& i : type->getAllPropertyOrder())
    {
        if (!i->isFlagOn(static_cast<int>
                        (PropertyAttr::WRITE))) continue;
        if (!i->isFlagOn(static_cast<int>
                        (PropertyAttr::SERIALIZABLE))) continue;
        if (i->isDefaultValue(node)) continue;
        
        String out;
        i->toString(node, out);
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
Node* Serializer::read_from_json(rapidjson::Value& jval)
{
    if (jval.IsNull()) return nullptr;
    
    const auto& type_iter = jval.FindMember("type");
    if (type_iter == jval.MemberEnd()) return nullptr;
    
    auto type = Kernel::instance()->findType(type_iter->value.GetString());
    if (nullptr == type) return nullptr;
    
    auto node = static_cast<Node*>(type->createObject());
    if (nullptr == node) return nullptr;
    node->onBeginDeserialize();
    
    this->read_prop_from_json(node, jval);
    
    const auto& children_iter = jval.FindMember("child");
    if (jval.MemberEnd() != children_iter)
    {
        auto& children = children_iter->value;
        if (children.IsArray())
        {
            for (auto i=children.Begin();i!=children.End();++i)
            {
                Node* child_node = this->read_from_json(*i);
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
void Serializer::read_prop_from_json(Node* node, rapidjson::Value& jval)
{
    if (jval.IsNull()) return;
    
    //static properties
    const auto& prop_jval_iter = jval.FindMember("prop");
    if (jval.MemberEnd() != prop_jval_iter)
    {
        auto& prop_jval = prop_jval_iter->value;
        auto type = node->getType();
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
            f->first->fromString(node, (f->second->value).GetString());
        }
    }
}
    
}
