namespace tod
{


//-----------------------------------------------------------------------------
void Property::setFlags(int32 value)
{
    this->flags |= value;
}


//-----------------------------------------------------------------------------
bool Property::isFlagOn(int32 flag) const
{
    return (this->flags & flag) > 0;
}


//-----------------------------------------------------------------------------
const String& Property::getName() const
{
    return this->name;
}


//-----------------------------------------------------------------------------
void Property::setDesc(const String& value)
{
    this->desc = value;
}


//-----------------------------------------------------------------------------
const String& Property::getDesc() const
{
    return this->desc;
}


//-----------------------------------------------------------------------------
void Property::setEditorType(const EditorType& value)
{
    this->editorType = value;
}


//-----------------------------------------------------------------------------
const Property::EditorType& Property::getEditorType() const
{
    return this->editorType;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
SimpleProperty<PROPERTY_TYPE>::SimpleProperty(const String& name)
: Property(name)
{
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
const std::type_info& SimpleProperty<PROPERTY_TYPE>::getType() const
{
    return typeid(PROPERTY_TYPE);
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
bool SimpleProperty<PROPERTY_TYPE>::isEnum() const
{
    return false;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
bool SimpleProperty<PROPERTY_TYPE>::isNumeric() const
{
    return std::is_arithmetic<PROPERTY_TYPE>::value;
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
SimplePropertyBind<TYPE, PROPERTY_TYPE>::SimplePropertyBind(const String& name)
: SimpleProperty<PROPERTY_TYPE>(name)
, setter(nullptr)
, getter(nullptr)
{
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::bind(Setter setter, Getter getter)
{
    this->setter = setter;
    this->getter = getter;
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::set(Object* object, const PROPERTY_TYPE& value)
{
    TYPE* self = static_cast<TYPE*>(object);
    (self->*setter)(value);
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
PROPERTY_TYPE SimplePropertyBind<TYPE, PROPERTY_TYPE>
::get(Object* object)
{
    TYPE* self = static_cast<TYPE*>(object);
    return (self->*getter)();
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::setDefaultValue(const PROPERTY_TYPE& value)
{
    this->defaultValue = value;
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
const PROPERTY_TYPE& SimplePropertyBind<TYPE, PROPERTY_TYPE>
::getDefaultValue() const
{
    return this->defaultValue;
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::fromString(Object* object, const String& value)
{
    typedef StringConv<PROPERTY_TYPE> SC;
    this->set(static_cast<TYPE*>(object), SC::fromString(value));
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::toString(Object* object, String& out_value)
{
    typedef StringConv<PROPERTY_TYPE> SC;
    SC::toString(this->get(static_cast<TYPE*>(object)), out_value);
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
void SimplePropertyBind<TYPE, PROPERTY_TYPE>
::setDefaultValue(Object* object)
{
    this->set(object, this->defaultValue);
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
bool SimplePropertyBind<TYPE, PROPERTY_TYPE>
::isDefaultValue(Object* object)
{
    return this->get(object) == this->defaultValue;
}


//-----------------------------------------------------------------------------
template <typename TYPE, typename PROPERTY_TYPE>
bool SimplePropertyBind<TYPE, PROPERTY_TYPE>
::isReadOnly()
{
    return nullptr == this->setter;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
template <typename ... ARGS>
EnumList<PROPERTY_TYPE>::EnumList(const ARGS& ... args)
{
    this->add_tuple(args ...);
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
bool EnumList<PROPERTY_TYPE>::empty() const
{
    return this->stringToValue.empty();
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
void EnumList<PROPERTY_TYPE>::add(const String& string, PROPERTY_TYPE value)
{
    this->stringToValue.push_back(std::make_tuple(string.hash(), value));
    this->valueToString.push_back(std::make_tuple(value, string));
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
PROPERTY_TYPE EnumList<PROPERTY_TYPE>::toValue(const String& string) const
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


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
bool EnumList<PROPERTY_TYPE>::toString(PROPERTY_TYPE value, String& out)
{
    auto i = std::find_if(this->valueToString.begin(),
        this->valueToString.end(),
        [value](const ValueToString& s)
    { return std::get<0>(s) == value; });
    if (this->valueToString.end() == i) TOD_RETURN_TRACE(false);
    out = std::get<1>(*i);
    return true;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
void EnumList<PROPERTY_TYPE>::setInvalidValue(PROPERTY_TYPE value)
{
    this->invalidValue = value;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
PROPERTY_TYPE EnumList<PROPERTY_TYPE>::getInvalidValue()
{
    return this->invalidValue;
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
typename EnumList<PROPERTY_TYPE>::iterator EnumList<PROPERTY_TYPE>::begin()
{
    return this->valueToString.begin();
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
typename EnumList<PROPERTY_TYPE>::iterator EnumList<PROPERTY_TYPE>::end()
{
    return this->valueToString.end();
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
template <typename ARG, typename ... ARGS>
void EnumList<PROPERTY_TYPE>::add_tuple(const ARG& value, const ARGS& ... args)
{
    this->add(std::get<0>(value), std::get<1>(value));
    this->add_tuple(args ...);
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
void EnumList<PROPERTY_TYPE>::add_tuple()
{
    //variadic arg ³¡
}
        

//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
EnumProperty<PROPERTY_TYPE>::EnumProperty(const String& name)
: SimpleProperty<PROPERTY_TYPE>(name)
{
}


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
bool EnumProperty<PROPERTY_TYPE>::isEnum() const
{
    return true;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>::EnumPropertyBind(const String& name)
: EnumProperty<PROPERTY_TYPE>(name)
, setter(nullptr)
, getter(nullptr)
, enumerator(nullptr)
{
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::bind(Setter setter, Getter getter, Enumerator enumerator)
{
    this->setter = setter;
    this->getter = getter;
    this->enumerator = enumerator;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::set(Object* object, const PROPERTY_TYPE& value)
{
    TYPE* self = static_cast<TYPE*>(object);
    (self->*setter)(value);
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
PROPERTY_TYPE EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::get(Object* object)
{
    TYPE* self = static_cast<TYPE*>(object);
    return (self->*getter)();
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::setDefaultValue(const PROPERTY_TYPE& value)
{
    this->defaultValue = value;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
const PROPERTY_TYPE& EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::getDefaultValue() const
{
    return this->defaultValue;
}

                                                                             
//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::fromString(Object* object, const String& value)
{
    static auto& e = this->enumerator();
    this->set(static_cast<TYPE*>(object), e.toValue(value));
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::toString(Object* object, String& out_value)
{
    static auto& e = this->enumerator();
    e.toString(this->get(object), out_value);
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
void EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::setDefaultValue(Object* object)
{
    this->set(object, this->defaultValue);
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
bool EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::isDefaultValue(Object* object)
{
    return this->get(object) == this->defaultValue;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
bool EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::isReadOnly()
{
    return nullptr == this->setter;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE, PROPERTY_TYPE INVALID_VALUE>
typename EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>::EnumListType&
EnumPropertyBind<TYPE, PROPERTY_TYPE, INVALID_VALUE>
::getEnumList()
{
    return this->enumerator();
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>    
DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::DynamicPropertyBind(const String& name)
: SimpleProperty<PROPERTY_TYPE>(name)
{
}

//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
void DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::set(Object* object, const PROPERTY_TYPE& value)
{
    this->value = value;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
PROPERTY_TYPE DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::get(Object* object)
{
    return this->value;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
void DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::setDefaultValue(const PROPERTY_TYPE& value)
{
    this->defaultValue = value;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
const PROPERTY_TYPE& DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::getDefaultValue() const
{
    return this->defaultValue;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
void DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::fromString(Object* object, const String& value)
{
    typedef StringConv<PROPERTY_TYPE> SC;
    this->set(static_cast<TYPE*>(object), SC::fromString(value));
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
void DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::toString(Object* object, String& out_value)
{
    typedef StringConv<PROPERTY_TYPE> SC;
    SC::toString(this->get(static_cast<TYPE*>(object)), out_value);
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
void DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::setDefaultValue(Object* object)
{
    this->set(object, this->defaultValue);
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
bool DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::isDefaultValue(Object* object)
{
    return this->get(object) == this->defaultValue;
}


//-----------------------------------------------------------------------------    
template <typename TYPE, typename PROPERTY_TYPE>
bool DynamicPropertyBind<TYPE, PROPERTY_TYPE>
::isReadOnly()
{
    return false;
}

}
