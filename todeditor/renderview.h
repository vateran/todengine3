#pragma once
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include "tod/node.h"
#include "tod/objref.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/cameracomponent.h"
namespace tod::editor
{

class RenderView : public QOpenGLWidget, protected QOpenGLExtraFunctions
{
public:
    RenderView(QWidget *parent=nullptr);
    virtual~RenderView();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void moveCamera(QEvent* event);
    bool eventFilter(QObject* watched, QEvent* event) override;
private:
    tod::graphics::Renderer* renderer;
    ObjRef<Node> mainCamera;
    graphics::TransformComponent* cameraTransformCompo;
    graphics::CameraComponent* cameraCompo;
    QPoint prevMousePos;

};

}
