#pragma once
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QRasterWindow>
#include "tod/node.h"
#include "tod/objref.h"
#include "tod/win32window.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/viewport.h"
#include "tod/graphics/forwardrenderpath.h"
#include "tod/graphics/constantbuffer.h"
#include "tod/graphics/shader.h"
#include "tod/graphics/dx11renderinterface.h"
namespace tod::editor
{

class RenderView
    #ifdef TOD_PLATFORM_WINDOWS
    : public QWidget
    #else
    : public QOpenGLWidget
    , protected QOpenGLExtraFunctions
    #endif
{
public:
    RenderView(QWidget *parent=nullptr);
    virtual~RenderView();

protected:
    #ifdef TOD_PLATFORM_MAC
    void initializeGL() override;
    void resizeGL(int32 w, int32 h) override;
    void paintGL() override;
    #else
    void initializeDirectX11();
    void paintEvent(QPaintEvent* event) override;
    QPaintEngine* paintEngine() const override { return nullptr; }
    #endif
    void moveCamera(QEvent* event);
    void onResize(QEvent* event);
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    tod::graphics::Renderer* renderer;
    ObjRef<Node> mainCamera;
    graphics::TransformComponent* cameraTransformCompo;
    graphics::CameraComponent* cameraCompo;
    QPoint prevMousePos;
    QSet<int32> keyPressed;

    tod::graphics::Viewport* viewport;
    tod::graphics::DX11RenderInterface renderInterface;
    tod::graphics::ForwardRenderPath renderPath;
    tod::graphics::ConstantBuffer* frameCB;
    tod::graphics::Shader* shader;
    tod::windows::Win32Window window;


};

}
