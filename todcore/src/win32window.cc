#include "tod/precompiled.h"
#include "tod/win32window.h"
#include "tod/exception.h"
#include "tod/staticstring.h"
#include "tod/timemgr.h"
namespace tod::windows
{

//-----------------------------------------------------------------------------
Win32Window::Win32Window()
: instanceHandle(GetModuleHandle(nullptr))
, windowHandle(nullptr)
{
}



//-----------------------------------------------------------------------------
Win32Window::~Win32Window()
{
    destroy();
}


//-----------------------------------------------------------------------------
bool Win32Window::create(const String& title, int32 width, int32 height)
{
    this->wndClsName.format("TodWindowsWin32_%p", this);
    WNDCLASSEX wnd;
    memset(&wnd, 0, sizeof(WNDCLASSEX));
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.cbClsExtra = 0;
    wnd.lpfnWndProc = Win32Window::callback;
    wnd.lpszClassName = this->wndClsName.c_str();
    wnd.style = CS_HREDRAW | CS_VREDRAW;
    wnd.hbrBackground = static_cast<HBRUSH>(GetStockObject(COLOR_WINDOW + 1));
    if (0 == RegisterClassEx(&wnd))
    {
        TOD_RETURN_TRACE(false);
    }

    CREATESTRUCT cs;
    memset(&cs, 0, sizeof(CREATESTRUCT));
    cs.lpCreateParams = this;
    this->windowHandle = CreateWindowEx(
        WS_EX_APPWINDOW
        , this->wndClsName.c_str()
        , title.c_str()
        , WS_OVERLAPPEDWINDOW
        , 0, 0, width, height, 0, 0
        , this->instanceHandle, &cs);
    if (nullptr == this->windowHandle)
    {
        destroy();
        TOD_RETURN_TRACE(false);
    }

    return true;
}


//-----------------------------------------------------------------------------
bool Win32Window::create(void* windowHandler)
{
    this->windowHandle = reinterpret_cast<HWND>(windowHandler);
    return true;
}


//-----------------------------------------------------------------------------
void Win32Window::destroy()
{
    if (nullptr != this->windowHandle)
    {
        DestroyWindow(this->windowHandle);
        this->windowHandle = nullptr;
    }

    if (!this->wndClsName.empty())
    {
        UnregisterClass(this->wndClsName.c_str(), this->instanceHandle);
        this->wndClsName.clear();
    }
}


//-----------------------------------------------------------------------------
void Win32Window::show(bool fullscreen)
{
    ShowWindow(
        this->windowHandle
        , SW_SHOW | (fullscreen ? SW_MAXIMIZE : 0));
}


//-----------------------------------------------------------------------------
void Win32Window::move(int32 x, int32 y)
{
    SetWindowPos(
        this->windowHandle
        , 0
        , x, y
        , 0, 0
        , SWP_NOOWNERZORDER | SWP_NOSIZE | SWP_NOREDRAW);
}


//-----------------------------------------------------------------------------
int32 Win32Window::mainLoop(const HandleFunc& handle_func)
{
    MSG msg;
    memset(&msg, 0, sizeof(MSG));
    while (WM_QUIT != msg.message)
    {
        if (0 < PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE))
        {
            GetMessage(&msg, 0, 0, 0);
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            this->handleKeyInput();
            handle_func();
        }
    }

    return static_cast<int32>(msg.wParam);
}


//-----------------------------------------------------------------------------
void* Win32Window::getWindowHandle() const
{
    return this->windowHandle;
}


void Win32Window::getSize(OUT uint32& width, OUT uint32& height) const
{
    RECT rect;
    GetWindowRect(this->windowHandle, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;
}


//-----------------------------------------------------------------------------
uint32 Win32Window::getWidth() const
{
    uint32 width, height;
    this->getSize(width, height);
    return width;
}


//-----------------------------------------------------------------------------
uint32 Win32Window::getHeight() const
{
    uint32 width, height;
    this->getSize(width, height);
    return height;
}


//-----------------------------------------------------------------------------
void Win32Window::addKeyInputHandler(
    const String& key
    , const String& desc
    , float period
    , const KeyInputHandleFunc& handle_func)
{
    static std::unordered_map<StaticString, uint8> s_keyMapper;
    if (true == s_keyMapper.empty())
    {
        s_keyMapper["shift"] = VK_SHIFT;
        s_keyMapper["alt"] = VK_MENU;
        s_keyMapper["ctrl"] = VK_CONTROL;
        s_keyMapper["left"] = VK_LEFT;
        s_keyMapper["right"] = VK_RIGHT;
        s_keyMapper["up"] = VK_UP;
        s_keyMapper["down"] = VK_DOWN;
        s_keyMapper["pgup"] = VK_PRIOR;
        s_keyMapper["pgdn"] = VK_NEXT;
        s_keyMapper["end"] = VK_END;
        s_keyMapper["inc"] = VK_INSERT;
        s_keyMapper["home"] = VK_HOME;
        s_keyMapper["del"] = VK_DELETE;
        s_keyMapper["back"] = VK_BACK;
        s_keyMapper["/"] = VK_DIVIDE;
        s_keyMapper["space"] = VK_SPACE;
        s_keyMapper["enter"] = VK_RETURN;
        s_keyMapper["tab"] = VK_TAB;
        for (int32 k = 0x30, i = 0; k < 0x39; ++k, ++i)
        {
            s_keyMapper[String::fromFormat("%c", i + '0')] = k;
        }
        for (int32 k = 0x41, i = 0; k < 0x5A; ++k, ++i)
        {
            s_keyMapper[String::fromFormat("%c", i + 'a')] = k;
        }
    }

    KeyInputDesc* key_input_desc = new KeyInputDesc;
    key_input_desc->desc = desc;
    key_input_desc->period = period;
    key_input_desc->handleFunc = handle_func;
    key_input_desc->excuted = false;
    key_input_desc->executedSec = 0;

    std::vector<String> tokens;
    key.split("+", tokens);
    for (auto& key_str : tokens)
    {
        auto i = s_keyMapper.find(StaticString(key_str.lower()));
        if (s_keyMapper.end() == i) continue;

        key_input_desc->keyCombinations.push_back(i->second);
    }

    this->keyInputDescs.emplace_back(key_input_desc);
    this->keyInputDescs.sort(
        [](const KeyInputDescPtr& l
            , const KeyInputDescPtr& r)
        {
            return l->keyCombinations.size()
        > r->keyCombinations.size();
        });
}


//-----------------------------------------------------------------------------
void Win32Window::handleKeyInput()
{
    if (GetActiveWindow() != this->windowHandle)
    {
        return;
    }

    bool executed = false;
    for (auto& desc : this->keyInputDescs)
    {
        bool hit = true;
        for (auto& key : desc->keyCombinations)
        {
            if (0 == GetAsyncKeyState(key))
            {
                hit = false;
                break;
            }
        }

        if (true == hit)
        {
            if (desc->period < 0)
            {
                if (false == desc->excuted)
                {
                    desc->handleFunc(true);
                    desc->excuted = true;
                    executed = true;
                }
            }
            else
            {
                if (desc->executedSec < TimeMgr::instance()->now())
                {
                    desc->handleFunc(true);
                    desc->excuted = true;
                    desc->executedSec =
                        static_cast<float>
                            (TimeMgr::instance()->now() + desc->period);
                    executed = true;
                }
            }
        }
        else
        {
            continue;
        }
    }

    if (false == executed)
    {
        for (auto& desc : this->keyInputDescs)
        {
            if (true == desc->excuted)
            {
                desc->handleFunc(false);
                desc->excuted = false;
            }
        }
    }
}


//-----------------------------------------------------------------------------
void Win32Window::setMouseInputHandler(const MouseInputHandleFunc& handle_func)
{
    this->mouseInputHandleFunc = handle_func;
}


//-----------------------------------------------------------------------------
LRESULT Win32Window::handleMsg(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
        this->mouseInputState.lbutton = true;
        break;
    case WM_LBUTTONUP:
        this->mouseInputState.lbutton = false;
        break;
    case WM_RBUTTONDOWN:
        this->mouseInputState.rbutton = true;
        break;
    case WM_RBUTTONUP:
        this->mouseInputState.rbutton = false;
        break;
    }

    switch (msg)
    {
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        this->mouseInputState.x = LOWORD(lparam);
        this->mouseInputState.y = HIWORD(lparam);
        if (nullptr != this->mouseInputHandleFunc)
        {
            this->mouseInputHandleFunc(this->mouseInputState);
        }
        break;
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}


//-----------------------------------------------------------------------------
LRESULT CALLBACK Win32Window::callback(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_NCCREATE:
    {
        CREATESTRUCT* cs1 = reinterpret_cast<CREATESTRUCT*>(lparam);
        CREATESTRUCT* cs2 = reinterpret_cast<CREATESTRUCT*>(cs1->lpCreateParams);
        auto self = reinterpret_cast<Win32Window*>(cs2->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
        return self->handleMsg(hwnd, msg, wparam, lparam);
    }
    case WM_CLOSE:
        PostQuitMessage(0);
        break;
    default:
    {
        auto self = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (nullptr != self)
        {
            return self->handleMsg(hwnd, msg, wparam, lparam);
        }
        break;
    }
    }

    return DefWindowProc(hwnd, msg, wparam, lparam);
}

}