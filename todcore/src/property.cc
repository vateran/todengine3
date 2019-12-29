#include "tod/precompiled.h"
#include "tod/property.h"
namespace tod
{

//-----------------------------------------------------------------------------
Property::Property(const String& name)
: name(name)
{
}


//-----------------------------------------------------------------------------
Property::~Property()
{
}

//-----------------------------------------------------------------------------
bool Property::isEqualType(const std::type_info& type_info) const
{
    return this->getType().hash_code() == type_info.hash_code();
}
    
}
