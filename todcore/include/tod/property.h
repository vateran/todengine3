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
    Property(const String& name):name(name) {}
    virtual~Property() {}
    
    virtual void fromString(Object* object, const String& value)=0;
    virtual void toString(Object* object, String& out_value)=0;
    virtual void setDefaultValue(Object* object)=0;
    virtual bool isDefaultValue(Object* object)=0;
    virtual bool isDefaultValue(Object* object) const=0;
    virtual bool isReadOnly()=0;
    virtual bool isEqualType(const std::type_info& type_info) const
    { return this->getType().hash_code() == type_info.hash_code(); }
    virtual const std::type_info& getType() const=0;
    
    void setFlags(int value);
    bool isFlagOn(int flag) { return (this->flags & flag) > 0; }
    const String& getName() { return this->name; }
    void setDesc(const String& value) { this->desc = value; }
    const String& getDesc() { return this->desc; }
    
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
    virtual PROPERTY_TYPE get(Object* object) const=0;
    
    const std::type_info& getType() const { return typeid(PROPERTY_TYPE); }
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
    typedef PROPERTY_TYPE (TYPE::*ConstGetter)() const;
    
public:
    SimplePropertyBind(const String& name):
    SimpleProperty<PROPERTY_TYPE>(name)
    , setter(nullptr)
    , getter(nullptr)
    , constGetter(nullptr)
    {
    }
    
    void bind(Setter setter, Getter getter)
    {
        this->setter = setter;
        this->getter = getter;
    }
    
    void bind(Setter setter, ConstGetter getter)
    {
        this->setter = setter;
        this->constGetter = getter;
    }
    
    void set(Object* object, const PROPERTY_TYPE& value) override
    {
        TYPE* self = static_cast<TYPE*>(object);
        (self->*setter)(value);
    }
    PROPERTY_TYPE get(Object* object) override
    {
        if (this->getter)
        {
            TYPE* self = static_cast<TYPE*>(object);
            return (self->*getter)();
        }
        else
        {
            TYPE* self = static_cast<TYPE*>(object);
            return (self->*constGetter)();
        }
    }
    PROPERTY_TYPE get(Object* object) const override
    {
        if (this->getter)
        {
            TYPE* self = static_cast<TYPE*>(object);
            return (self->*getter)();
        }
        else
        {
            TYPE* self = static_cast<TYPE*>(object);
            return (self->*constGetter)();
        }
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
        if(nullptr == this->setter) return;
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
    bool isDefaultValue(Object* object) const override
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
    ConstGetter constGetter;
    
    struct DefaultValue
    {
        using remove_ref = typename std::conditional<
            std::is_reference<PROPERTY_TYPE>::value,
            typename std::remove_reference<PROPERTY_TYPE>::type,
            PROPERTY_TYPE>::type;
        using remove_const = typename std::conditional<
            std::is_const<remove_ref>::value,
            typename std::remove_const<remove_ref>::type,
            remove_ref>::type;
        typedef remove_const type;
    };
    
    typename DefaultValue::type defaultValue;
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

}
