#pragma once
#include <rapidjson/document.h>
#include "tod/string.h"
namespace tod
{

class Object;
class Node;

//!@ingroup Relfection
//!@brief NOH를 serialize/deserialize 한다
//! - JSON형태의 string 또는 파일로 serialize
//! - JSON string 또는 파일로 부터 객체를 deserialize
class Serializer
{
public:
    bool serializeToJson(Node* node, String& json_str_out);
    bool serializeToJsonFile(Node* node, const String& uri);
    
    Node* deserializeFromJson(const String& json_str);
    Node* deserializeFromJsonFile(const String& uri);
    
private:
    void write_node_to_json(Node* node, rapidjson::Value& json,
                       rapidjson::MemoryPoolAllocator<>& allocator);
    void write_object_to_json(Object* object, rapidjson::Value& json,
                       rapidjson::MemoryPoolAllocator<>& allocator);
    void write_prop_to_json(Object* object, rapidjson::Value& json,
                            rapidjson::MemoryPoolAllocator<>& allocator);
    Node* read_node_from_json(rapidjson::Value& jval);
    void read_prop_from_json(Object* object, rapidjson::Value& jval);
};
    
}
