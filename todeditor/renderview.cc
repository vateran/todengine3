#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QWindow>
#include <QVBoxLayout>
#include "tod/timemgr.h"
#include "tod/kernel.h"
#include "tod/graphics/camera.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercache.h"
#include "todeditor/renderview.h"
namespace tod::editor
{   

//-----------------------------------------------------------------------------
RenderView::RenderView(QWidget *parent)
    #ifdef TOD_PLATFORM_WINDOWS
    : QWidget(parent)
    #else
    : QOpenGLWidget(parent)
    #endif
    , cameraTransformCompo(nullptr)
    , cameraCompo(nullptr)
{
    qApp->installEventFilter(this);
    
    this->setAutoFillBackground(false);
    this->setAttribute(Qt::WA_NoSystemBackground, true);
    this->setAttribute(Qt::WA_OpaquePaintEvent, false);
    this->setAttribute(Qt::WA_PaintOnScreen, true);

    this->window.create(reinterpret_cast<void*>(this->winId()));
    this->renderInterface.create();
    tod::graphics::Renderer::instance()->renderInterface = &this->renderInterface;
    this->viewport = this->renderInterface.createViewport();
    this->viewport->create(&this->window, 8);
    
    this->shader = tod::graphics::ShaderCache::instance()->getShader("EngineData/hlsl/BasicForward.fx");

    this->renderPath.create(&this->renderInterface);

    #ifdef TOD_PLATFORM_WINDOWS
    initializeDirectX11();
    #endif
}


//-----------------------------------------------------------------------------
RenderView::~RenderView()
{
}


//-----------------------------------------------------------------------------
#ifdef TOD_PLATFORM_MAC
void RenderView::initializeGL()
{
    this->initializeOpenGLFunctions();

    this->renderer = static_cast<tod::graphics::Renderer*>
        (Kernel::instance()->create("OpenGlRenderer", "/sys/renderer"));
    this->renderer->initialize((QOpenGLExtraFunctions*)this, 1540, 1316, true);
}


//-----------------------------------------------------------------------------
void RenderView::resizeGL(int32 w, int32 h)
{
    this->renderer->setViewport(w, h);
    if (this->cameraCompo)
    {
        this->cameraCompo->setWidth(w * graphics::Renderer::ScreenScale());
        this->cameraCompo->setHeight(h * graphics::Renderer::ScreenScale());
    }
}


//-----------------------------------------------------------------------------
void RenderView::paintGL()
{
    TimeMgr::instance()->update();
    
    this->renderer->render(tod::Kernel::instance()->lookup("/scene"));
}
#else
void RenderView::initializeDirectX11()
{
    //this->renderer = static_cast<tod::graphics::Renderer*>
    //  (tod::Kernel::instance()->create("Dx11Renderer", "/sys/renderer"));
    //this->renderer->initialize((void*)(this->winId()), 1540, 1316, true);
}

void RenderView::paintEvent(QPaintEvent* event)
{
    if (this->mainCamera.invalid())
    {
        this->mainCamera = Kernel::instance()->lookup("/scene/camera");
        if (this->mainCamera.invalid())
        {
            return;
        }

        this->cameraTransformCompo = this->mainCamera->findComponent<tod::graphics::TransformComponent>();
        this->cameraCompo = this->mainCamera->findComponent<tod::graphics::CameraComponent>();
    }

    TimeMgr::instance()->update();

    float step = 200 * TimeMgr::instance()->delta();
    if (this->keyPressed.contains(Qt::Key_Shift))
        step *= 20;
    if (this->keyPressed.contains(Qt::Key_W))
        this->cameraTransformCompo->moveForward(step);
    if (this->keyPressed.contains(Qt::Key_S))
        this->cameraTransformCompo->moveForward(-step);
    if (this->keyPressed.contains(Qt::Key_A))
        this->cameraTransformCompo->moveSideward(-step);
    if (this->keyPressed.contains(Qt::Key_D))
        this->cameraTransformCompo->moveSideward(step);
    if (this->keyPressed.contains(Qt::Key_Q))
        this->cameraTransformCompo->moveUpward(step);
    if (this->keyPressed.contains(Qt::Key_E))
        this->cameraTransformCompo->moveUpward(-step);
    if (this->keyPressed.contains(Qt::Key_Space))
        this->cameraTransformCompo->reset();
    
    this->viewport->use();
    this->viewport->clear(tod::graphics::ColorF(0, 0, 1, 1));

    this->renderPath.render(Kernel::instance()->lookup("/scene"), this->viewport);

    this->viewport->swap();

    this->update();
}
#endif


//-----------------------------------------------------------------------------
void RenderView::moveCamera(QEvent* event)
{   
    if (this->mainCamera.invalid())
    {
        return;
    }

    //마우스커서가 스크린 안에 있는지 확인
    auto geo = this->geometry();
    auto global_pt = this->mapToGlobal(QPoint(0, 0));
    geo.moveTo(global_pt.x(), global_pt.y());
    if (!geo.contains(QCursor::pos())) return;
    
    switch (event->type())
    {
    case QEvent::KeyPress:
        {
            auto key_event = static_cast<QKeyEvent*>(event);            
            this->keyPressed += key_event->key();
        }
        break;
    case QEvent::KeyRelease:
        {
            auto key_event = static_cast<QKeyEvent*>(event);
            this->keyPressed -= key_event->key();
        }
        break;
    case QEvent::MouseButtonPress:
        {
            auto mouse_event = static_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::RightButton)
            {
                this->prevMousePos = mouse_event->pos();
            }
        }
        break;
    case QEvent::MouseMove:
        {
            if (this->prevMousePos.isNull()) return;

            auto mouse_event = static_cast<QMouseEvent*>(event);            
            if (mouse_event->buttons() & Qt::RightButton)
            {
                if (this->mainCamera.valid())
                {
                    auto pos = this->cameraTransformCompo->getRotation();
                    auto delta = -(mouse_event->pos() - this->prevMousePos);
                    this->prevMousePos = mouse_event->pos();
                    pos.y += delta.x() * TimeMgr::instance()->delta() * 100;
                    pos.x += delta.y() * TimeMgr::instance()->delta() * 100;
                    this->cameraTransformCompo->setRotation(pos);
                }
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        {
            auto mouse_event = static_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() == Qt::RightButton)
            {
                this->prevMousePos.setX(0);
                this->prevMousePos.setY(0);
            }
        }
        break;
    default: break;
    }
}


//-----------------------------------------------------------------------------
void RenderView::onResize(QEvent* event)
{
    QResizeEvent* resize_event = static_cast<QResizeEvent*>(event);
    this->viewport->resize(
          resize_event->size().width()
        , resize_event->size().height());
}


//-----------------------------------------------------------------------------
bool RenderView::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::KeyRelease:
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
        this->moveCamera(event);
        break;
    case QEvent::Resize:
        this->onResize(event);
        break;
    default: break;
    }

    #ifdef TOD_PLATFORM_WINDOWS
    return QWidget::eventFilter(watched, event);
    #else
    return QOpenGLWidget::eventFilter(watched, event);
    #endif
}

}
