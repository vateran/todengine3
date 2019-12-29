#pragma once
#include "tod/stringconv.h"
namespace tod
{
    
class Object;

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Property 의 속성
enum class PropertyAttr
{
    READ = 1<<0,                                //! < 읽기 가능
    WRITE = 1<<1,                               //! < 쓰기 가능
    HIDDEN = 1<<2,                              //! < 숨겨짐
    SERIALIZABLE = 1<<3,                        //! < Serialization 가능
    DEFAULT = READ | WRITE | SERIALIZABLE,
    
    MAX
};
    

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Object의 Property정보를 처리
class Property
{
public:
    enum EditorType
    {
        EDITORTYPE_SINGLELINE,
        EDITORTYPE_MULTILINE,
        EDITORTYPE_SLIDER,
        EDITORTYPE_FILEDIALOG,
    };
    
public:
    Property(const String& name);
    virtual~Property();
    
    virtual void fromString(Object* object, const String& value)=0;
    virtual void toString(Object* object, String& out_value)=0;
    virtual void setDefaultValue(Object* object)=0;
    virtual bool isDefaultValue(Object* object)=0;
    virtual bool isEnum() const = 0;
    virtual bool isReadOnly()=0;
    virtual bool isEqualType(const std::type_info& type_info) const;
    virtual bool isNumeric() const=0;
    virtual const std::type_info& getType() const=0;
    
    inline void setFlags(int32 value);
    inline bool isFlagOn(int32 flag) const;
    inline const String& getName() const;
    inline void setDesc(const String& value);
    inline const String& getDesc() const;
    inline void setEditorType(const EditorType& value);
    inline const EditorType& getEditorType() const;
    
private:
    String name;
    String desc;
    int32 flags;
    EditorType editorType;
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief 간단한 C/C++ native 타입을 Type에서 처리하기 위한 템플릿
template <typename PROPERTY_TYPE>
class SimpleProperty : public Property
{
public:
    SimpleProperty(const String& name);
    
    virtual void set(Object* object, const PROPERTY_TYPE& value)=0;
    virtual PROPERTY_TYPE get(Object* object)=0;
    
    const std::type_info& getType() const override;
    bool isEnum() const override;
    bool isNumeric() const override;
};
    

//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief 간단한 C/C++ native 타입을 Type에서 등록하기 위한 Holder 템플릿
template <typename TYPE, typename PROPERTY_TYPE>
class SimplePropertyBind : public SimpleProperty<PROPERTY_TYPE>
{
public:
    typedef void (TYPE::*Setter)(PROPERTY_TYPE);
    typedef PROPERTY_TYPE (TYPE::*Getter)();
    
public:
    SimplePropertyBind(const String& name);
    
    void bind(Setter setter, Getter getter);
    void set(Object* object, const PROPERTY_TYPE& value) override;
    PROPERTY_TYPE get(Object* object) override;
    void setDefaultValue(const PROPERTY_TYPE& value);
    const PROPERTY_TYPE& getDefaultValue() const;
    void fromString(Object* object, const String& value) override;
    void toString(Object* object, String& out_value) override;
    void setDefaultValue(Object* object) override;
    bool isDefaultValue(Object* object) override;
    bool isReadOnly() override;
    
private:
    Setter setter;
    Getter getter;
    typename std::decay<PROPERTY_TYPE>::type defaultValue;
};
    
    
//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief enum 정보를 저장
template <typename PROPERTY_TYPE>
class EnumList
{
public:
    typedef std::tuple<int, PROPERTY_TYPE> StringToValue;
    typedef std::tuple<PROPERTY_TYPE, String> ValueToString;
    typedef std::vector<StringToValue> StringToValueList;
    typedef std::vector<ValueToString> ValueToStringList;
    typedef typename ValueToStringList::iterator iterator;
    
public:
    template <typename ... ARGS>
    EnumList(const ARGS& ... args);
    bool empty() const;
    void add(const String& string, PROPERTY_TYPE value);
    PROPERTY_TYPE toValue(const String& string) const;
    bool toString(PROPERTY_TYPE value, String& out);
    void setInvalidValue(PROPERTY_TYPE value);
    PROPERTY_TYPE getInvalidValue();
    iterator begin();
    iterator end();
    
private:
    template <typename ARG, typename ... ARGS>
    void add_tuple(const ARG& value, const ARGS& ... args);
    void add_tuple();
    
private:
    StringToValueList stringToValue;
    ValueToStringList valueToString;
    PROPERTY_TYPE invalidValue;
};
    
    
//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief enum 을 나타내는 Property
template <typename PROPERTY_TYPE>
class EnumProperty : public SimpleProperty<PROPERTY_TYPE>
{
public:
    typedef EnumList<PROPERTY_TYPE> EnumListType;
    
public:
    EnumProperty(const String& name);
    
    virtual EnumListType& getEnumList() = 0;
    
    bool isEnum() const;
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Enum 타입을 Type에서 등록하기 위한 Holder 템플릿
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
class EnumPropertyBind : public EnumProperty<PROPERTY_TYPE>
{
public:
    typedef void (TYPE::*Setter)(PROPERTY_TYPE);
    typedef PROPERTY_TYPE (TYPE::*Getter)();
    typedef typename EnumProperty<PROPERTY_TYPE>::EnumListType EnumListType;
    typedef EnumListType&(*Enumerator)();
    
public:
    EnumPropertyBind(const String& name);
    
    void bind(Setter setter, Getter getter, Enumerator enumerator);
    void set(Object* object, const PROPERTY_TYPE& value) override;
    PROPERTY_TYPE get(Object* object) override;
    void setDefaultValue(const PROPERTY_TYPE& value);
    const PROPERTY_TYPE& getDefaultValue() const;
    void fromString(Object* object, const String& value) override;
    void toString(Object* object, String& out_value) override;
    void setDefaultValue(Object* object) override;
    bool isDefaultValue(Object* object) override;
    bool isReadOnly() override;
    EnumListType& getEnumList() override;
    
private:
    Setter setter;
    Getter getter;
    Enumerator enumerator;
    typename std::decay<PROPERTY_TYPE>::type defaultValue;
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Runtime에 실제로 값을 저장할 수 있는 Property.
//!이 Proprety는 instance 가 될 것이며, Meta정보로 사용되는 것이 아님
template <typename TYPE, typename PROPERTY_TYPE>
class DynamicPropertyBind : public SimpleProperty<PROPERTY_TYPE>
{   
public:
    DynamicPropertyBind(const String& name);
    
    void set(Object* object, const PROPERTY_TYPE& value) override;
    PROPERTY_TYPE get(Object* object) override;
    void setDefaultValue(const PROPERTY_TYPE& value);
    const PROPERTY_TYPE& getDefaultValue() const;
    void fromString(Object* object, const String& value) override;
    void toString(Object* object, String& out_value) override;
    void setDefaultValue(Object* object) override;
    bool isDefaultValue(Object* object) override;
    bool isReadOnly() override;
    
private:
    typename std::decay<PROPERTY_TYPE>::type value;
    typename std::decay<PROPERTY_TYPE>::type defaultValue;
    
};

#define BIND_PROPERTY(type, name, desc, setter, getter, default_value, flags) \
do { typedef tod::SimplePropertyBind<SelfType, type> PropType;\
auto prop = new PropType(name);\
prop->bind(&SelfType::setter, &SelfType::getter);\
prop->setDefaultValue(default_value);\
prop->setFlags(static_cast<int>(flags));\
prop->setDesc(desc);\
SelfType::get_type()->addProperty(prop); } while(false)

    
#define BIND_PROPERTY_READONLY(type, name, desc, getter, default_value, flags) \
do { typedef tod::SimplePropertyBind<SelfType, type> PropType;\
auto prop = new PropType(name);\
prop->bind(nullptr, &SelfType::getter);\
prop->setDefaultValue(default_value);\
prop->setFlags(static_cast<int>(flags));\
prop->setDesc(desc);\
SelfType::get_type()->addProperty(prop); } while(false)


#define BIND_ENUM_PROPERTY(type, name, desc, setter, getter, enumerator, default_value, flags) \
do { typedef tod::EnumPropertyBind<SelfType, type, 0> PropType;\
auto prop = new PropType(name);\
prop->bind(&SelfType::setter, &SelfType::getter, &SelfType::enumerator);\
prop->setDefaultValue(default_value);\
prop->setFlags(static_cast<int>(flags));\
prop->setDesc(desc);\
SelfType::get_type()->addProperty(prop); } while(false)
    
}

#include "tod/property.inl"
