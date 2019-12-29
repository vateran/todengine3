#pragma once
#include "todeditor/dockwidget.h"
#include "tod/graphics/texture.h"
#ifdef TOD_PLATFORM_MAC
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#endif
namespace tod::editor
{

class RenderTargetViewer
    #ifdef TOD_PLATFORM_WINDOWS
    : public DockWidget<RenderTargetViewer, QWidget>
    #else
    : public DockWidget<RenderTargetViewer, QOpenGLWidget>
    , protected QOpenGLExtraFunctions
    #endif
{
protected:
    class View;

    RenderTargetViewer();
    friend class DockWidget<RenderTargetViewer, QWidget>;

private:
    graphics::Texture* targetTexture;
};

}
