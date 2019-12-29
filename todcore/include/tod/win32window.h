#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include "tod/window.h"
namespace tod::windows
{

class Win32Window : public Window
{
public:
    struct KeyInputDesc
    {
        std::vector<uint8> keyCombinations;
        String desc;
        float period;
        float executedSec;
        KeyInputHandleFunc handleFunc;
        bool excuted;
        bool shift;
        bool alt;
        bool ctrl;
    };

    typedef std::shared_ptr<KeyInputDesc> KeyInputDescPtr;
    typedef std::list<KeyInputDescPtr> KeyInputDescs;
    KeyInputDescs keyInputDescs;

public:
    Win32Window();
    virtual~Win32Window();

    bool create(const String& title, int32 width, int32 height) override;
    bool create(void* windowHandler) override;
    void destroy() override;

    void show(bool fullscreen) override;
    void move(int32 x, int32 y) override;
    int32 mainLoop(const HandleFunc& handle_func) override;
    void* getWindowHandle() const override;
    void getSize(OUT uint32& width, OUT uint32& height) const override;
    uint32 getWidth() const override;
    uint32 getHeight() const override;

    void addKeyInputHandler(
        const String& key
        , const String& desc
        , float period
        , const KeyInputHandleFunc& handle_func) override;
    void handleKeyInput();
    void setMouseInputHandler(const MouseInputHandleFunc& handle_func) override;

private:
    LRESULT handleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    HINSTANCE instanceHandle;
    HWND windowHandle;
    tod::String wndClsName;
    MouseInputHandleFunc mouseInputHandleFunc;
    MouseInputState mouseInputState;
};

}