#pragma once
#include <QOpenGLExtraFunctions>
namespace tod::graphics
{

class OpenGl
{
public:
    typedef QOpenGLExtraFunctions FuncsType;
    static FuncsType* funcs;
    static void checkError();
};

#define TOD_OPENGL_CHECKERROR() OpenGl::checkError()

}
