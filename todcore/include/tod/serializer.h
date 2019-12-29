#pragma once
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
    
};
    
}
