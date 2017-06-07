#include "tod/objref.h"
#include "tod/kernel.h"
namespace tod
{

//-----------------------------------------------------------------------------
void ObjRefHelper::setByPath(Object** ptr, const String& path)
{
    *ptr = Kernel::instance()->lookup(path);
}

}
