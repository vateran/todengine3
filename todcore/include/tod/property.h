#pragma once
#include "tod/stringconv.h"
#include "tod/exception.h"
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
    Property(const String& name):name(name) {}
    virtual~Property() {}
    
    virtual void fromString(Object* object, const String& value)=0;
    virtual void toString(Object* object, String& out_value)=0;
    virtual void setDefaultValue(Object* object)=0;
    virtual bool isDefaultValue(Object* object)=0;
    virtual bool isEnum() const = 0;
    virtual bool isReadOnly()=0;
    virtual bool isEqualType(const std::type_info& type_info) const
    { return this->getType().hash_code() == type_info.hash_code(); }
    virtual bool isNumeric() const=0;
    virtual const std::type_info& getType() const=0;
    
    void setFlags(int value);
    bool isFlagOn(int flag) { return (this->flags & flag) > 0; }
    const String& getName() { return this->name; }
    void setDesc(const String& value) { this->desc = value; }
    const String& getDesc() { return this->desc; }
    void setEditorType(const EditorType& value) { this->editorType = value; }
    const EditorType& getEditorType() { return this->editorType; }
    
private:
    String name;
    String desc;
    int flags;
    EditorType editorType;
};


//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief 간단한 C/C++ native 타입을 Type에서 처리하기 위한 템플릿
template <typename PROPERTY_TYPE>
class SimpleProperty : public Property
{
public:
    SimpleProperty(const String& name):Property(name) {}
    
    virtual void set(Object* object, const PROPERTY_TYPE& value)=0;
    virtual PROPERTY_TYPE get(Object* object)=0;
    
    const std::type_info& getType() const override { return typeid(PROPERTY_TYPE); }
    bool isEnum() const override { return false; }
    bool isNumeric() const override
    { return std::is_arithmetic<PROPERTY_TYPE>::value; }
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
    SimplePropertyBind(const String& name):
    SimpleProperty<PROPERTY_TYPE>(name)
    , setter(nullptr)
    , getter(nullptr)
    {
    }
    
    void bind(Setter setter, Getter getter)
    {
        this->setter = setter;
        this->getter = getter;
    }
    void set(Object* object, const PROPERTY_TYPE& value) override
    {
        TYPE* self = static_cast<TYPE*>(object);
        (self->*setter)(value);
    }
    PROPERTY_TYPE get(Object* object) override
    {
        TYPE* self = static_cast<TYPE*>(object);
        return (self->*getter)();
    }
    void setDefaultValue(const PROPERTY_TYPE& value)
    {
        this->defaultValue = value;
    }
    const PROPERTY_TYPE& getDefaultValue() const
    {
        return this->defaultValue;
    }
    
    void fromString(Object* object, const String& value) override
    {
        typedef StringConv<PROPERTY_TYPE> SC;
        this->set(static_cast<TYPE*>(object), SC::fromString(value));
    }
    void toString(Object* object, String& out_value) override
    {
        typedef StringConv<PROPERTY_TYPE> SC;
        SC::toString(this->get(static_cast<TYPE*>(object)), out_value);
    }
    void setDefaultValue(Object* object) override
    {
        this->set(object, this->defaultValue);
    }
    bool isDefaultValue(Object* object) override
    {
        return this->get(object) == this->defaultValue;
    }
    bool isReadOnly() override
    {
        return nullptr == this->setter;
    }
    
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
    EnumList(const ARGS& ... args)
    {
        this->add_tuple(args ...);
    }
    bool empty() const
    {
        return this->stringToValue.empty();
    }
    void add(const String& string, PROPERTY_TYPE value)
    {
        this->stringToValue.push_back(std::make_tuple(string.hash(), value));
        this->valueToString.push_back(std::make_tuple(value, string));
    }
    PROPERTY_TYPE toValue(const String& string) const
    {
        auto hash = string.hash();
        auto i = std::find_if(this->stringToValue.begin(),
                              this->stringToValue.end(),
                              [hash](const StringToValue& s)
                              { return std::get<0>(s) == hash; });
        if (this->stringToValue.end() == i)
            i = this->stringToValue.begin();
        return std::get<1>(*i);
    }
    bool toString(PROPERTY_TYPE value, String& out)
    {
        auto i = std::find_if(this->valueToString.begin(),
                              this->valueToString.end(),
                              [value](const ValueToString& s)
                              { return std::get<0>(s) == value; });
        if (this->valueToString.end() == i) TOD_RETURN_TRACE(false);
        out = std::get<1>(*i);
        return true;
    }
    void setInvalidValue(PROPERTY_TYPE value)
    {
        this->invalidValue = value;
    }
    PROPERTY_TYPE getInvalidValue()
    {
        return this->invalidValue;
    }
    iterator begin() { return this->valueToString.begin(); }
    iterator end() { return this->valueToString.end(); }
    
private:
    template <typename ARG, typename ... ARGS>
    void add_tuple(const ARG& value, const ARGS& ... args)
    {
        this->add(std::get<0>(value), std::get<1>(value));
        this->add_tuple(args ...);
    }
    void add_tuple() {}
    
private:
    StringToValueList stringToValue;
    ValueToStringList valueToString;
    PROPERTY_TYPE invalidValue;
};
    
    
//-----------------------------------------------------------------------------
//!@ingroup Reflection
//!@brief Enum 을 나타내는 Property
template <typename PROPERTY_TYPE>
class EnumProperty : public SimpleProperty<PROPERTY_TYPE>
{
public:
    typedef EnumList<PROPERTY_TYPE> EnumListType;
    
public:
    EnumProperty(const String& name):
    SimpleProperty<PROPERTY_TYPE>(name) {}
    
    virtual EnumListType& getEnumList() = 0;
    
    bool isEnum() const { return true; }
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
    EnumPropertyBind(const String& name):
    EnumProperty<PROPERTY_TYPE>(name)
    , setter(nullptr)
    , getter(nullptr)
    , enumerator(nullptr)
    {
    }
    
    void bind(Setter setter, Getter getter, Enumerator enumerator)
    {
        this->setter = setter;
        this->getter = getter;
        this->enumerator = enumerator;
    }
    void set(Object* object, const PROPERTY_TYPE& value) override
    {
        TYPE* self = static_cast<TYPE*>(object);
        (self->*setter)(value);
    }
    PROPERTY_TYPE get(Object* object) override
    {
        TYPE* self = static_cast<TYPE*>(object);
        return (self->*getter)();
    }
    void setDefaultValue(const PROPERTY_TYPE& value)
    {
        this->defaultValue = value;
    }
    const PROPERTY_TYPE& getDefaultValue() const
    {
        return this->defaultValue;
    }
    
    void fromString(Object* object, const String& value) override
    {
        static auto& e = this->enumerator();
        this->set(static_cast<TYPE*>(object), e.toValue(value));
    }
    void toString(Object* object, String& out_value) override
    {
        static auto& e = this->enumerator();
        e.toString(this->get(object), out_value);
    }
    void setDefaultValue(Object* object) override
    {
        this->set(object, this->defaultValue);
    }
    bool isDefaultValue(Object* object) override
    {
        return this->get(object) == this->defaultValue;
    }
    bool isReadOnly() override
    {
        return nullptr == this->setter;
    }
    EnumListType& getEnumList() override
    {
        return this->enumerator();
    }
    
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
    DynamicPropertyBind(const String& name):
    SimpleProperty<PROPERTY_TYPE>(name)
    {
    }
    
    void set(Object* object, const PROPERTY_TYPE& value) override
    {
        this->value = value;
    }
    PROPERTY_TYPE get(Object* object) override
    {
        return this->value;
    }
    void setDefaultValue(const PROPERTY_TYPE& value)
    {
        this->defaultValue = value;
    }
    const PROPERTY_TYPE& getDefaultValue() const
    {
        return this->defaultValue;
    }
    
    void fromString(Object* object, const String& value) override
    {
        typedef StringConv<PROPERTY_TYPE> SC;
        this->set(static_cast<TYPE*>(object), SC::fromString(value));
    }
    void toString(Object* object, String& out_value) override
    {
        typedef StringConv<PROPERTY_TYPE> SC;
        SC::toString(this->get(static_cast<TYPE*>(object)), out_value);
    }
    void setDefaultValue(Object* object) override
    {
        this->set(object, this->defaultValue);
    }
    bool isDefaultValue(Object* object) override
    {
        return this->get(object) == this->defaultValue;
    }
    bool isReadOnly() override
    {
        return false;
    }
    
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
