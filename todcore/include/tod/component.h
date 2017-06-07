#pragma once
#include "tod/object.h"
namespace tod
{
  
//!@ingroup Core
//!@brief TodEngine에서 Node에 기능을 부여하는 기능을 관리하는 인터페이스
class Component : public Derive<Component, Object>
{
public:
};

}
