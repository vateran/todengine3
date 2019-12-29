#pragma once
#include <functional>
#include "tod/basetype.h"
#include "tod/string.h"
namespace tod
{

class Window
{
public:
    struct MouseInputState
    {
        MouseInputState():x(0), y(0), lbutton(false), rbutton(false) {}
        int32 x;
        int32 y;
        bool lbutton;
        bool rbutton;
    };
    typedef std::function<void()> HandleFunc;
    typedef std::function<void(bool)> KeyInputHandleFunc;
    typedef std::function<void(const MouseInputState&)> MouseInputHandleFunc;

public:
    virtual~Window() {}

    virtual bool create(const String& title, int32 width, int32 height) = 0;
    virtual bool create(void* windowHandler) = 0;
    virtual void destroy() = 0;
    virtual void show(bool fullscreen) = 0;
    virtual void move(int32 x, int32 y) = 0;
    virtual int32 mainLoop(const HandleFunc& handle_func) = 0;
    virtual void* getWindowHandle() const = 0;
    virtual void getSize(OUT uint32& width, OUT uint32& height) const = 0;
    virtual uint32 getWidth() const = 0;
    virtual uint32 getHeight() const = 0;
    virtual void addKeyInputHandler(
        const String& key
        , const String& desc
        , float period
        , const KeyInputHandleFunc& handle_func) = 0;
    virtual void setMouseInputHandler(
        const MouseInputHandleFunc& handle_func) = 0;
};

}