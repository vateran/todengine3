#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QApplication>
#include "tod/timemgr.h"
#include "todeditor/renderview.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
RenderView::RenderView(QWidget *parent)
: QOpenGLWidget(parent)
, cameraTransformCompo(nullptr)
, cameraCompo(nullptr)
{
    qApp->installEventFilter(this);

    auto update_timer = new QTimer();
    this->connect(update_timer, &QTimer::timeout, [this]()
        {
            this->update();
        });
    update_timer->start(0);
    update_timer->setInterval(20);
}


//-----------------------------------------------------------------------------
RenderView::~RenderView()
{
}


//-----------------------------------------------------------------------------
void RenderView::initializeGL()
{
    this->initializeOpenGLFunctions();

    this->renderer = static_cast<tod::graphics::Renderer*>
        (Kernel::instance()->create("OpenGlRenderer", "/sys/renderer"));
    this->renderer->initialize((QOpenGLExtraFunctions*)this, 1540, 1316, true);
}


//-----------------------------------------------------------------------------
void RenderView::resizeGL(int w, int h)
{
    this->renderer->setViewport(w, h);
    if (this->cameraCompo)
    {
        this->cameraCompo->setWidth(w * graphics::Renderer::ScreenScale());
        this->cameraCompo->setHeight(h * graphics::Renderer::ScreenScale());
    }
    printf("%d %d\n", w, h);
}


//-----------------------------------------------------------------------------
void RenderView::paintGL()
{
    TimeMgr::instance()->update();
    
    this->renderer->render(tod::Kernel::instance()->lookup("/scene"));
}


//-----------------------------------------------------------------------------
void RenderView::moveCamera(QEvent* event)
{
    if (this->mainCamera.invalid())
    {
        this->mainCamera = "/scene/camera";
        if (this->mainCamera.valid())
        {
            this->cameraTransformCompo =
                this->mainCamera->findComponent<graphics::TransformComponent>();
            this->cameraCompo =
                this->mainCamera->findComponent<graphics::CameraComponent>();
        }
    }
    if (this->mainCamera.invalid()) return;
    
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
            float step = 5 * TimeMgr::instance()->delta();
            if (QApplication::keyboardModifiers() & Qt::ShiftModifier)
                step *= 20;
            if (key_event->key() == Qt::Key_W)
                this->cameraTransformCompo->moveForward(-step);
            if (key_event->key() == Qt::Key_S)
                this->cameraTransformCompo->moveForward(step);
            if (key_event->key() == Qt::Key_A)
                this->cameraTransformCompo->moveSideward(-step);
            if (key_event->key() == Qt::Key_D)
                this->cameraTransformCompo->moveSideward(step);
            if (key_event->key() == Qt::Key_Q)
                this->cameraTransformCompo->moveUpward(step);
            if (key_event->key() == Qt::Key_E)
                this->cameraTransformCompo->moveUpward(-step);
        }
        break;
    case QEvent::MouseButtonPress:
        {
            auto mouse_event = static_cast<QMouseEvent*>(event);
            this->prevMousePos = mouse_event->pos();
        }
        break;
    case QEvent::MouseMove:
        {
            if (this->prevMousePos.isNull()) return;
            auto mouse_event = static_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                if (this->mainCamera.valid())
                {
                    auto pos = this->cameraTransformCompo->getRotation();
                    auto delta = mouse_event->pos() - this->prevMousePos;
                    this->prevMousePos = mouse_event->pos();
                    pos.y += delta.x() * TimeMgr::instance()->delta() * 10;
                    pos.x += delta.y() * TimeMgr::instance()->delta() * 10;
                    this->cameraTransformCompo->setRotation(pos);
                }
            }
        }
        break;
    case QEvent::MouseButtonRelease:
        this->prevMousePos.setX(0);
        this->prevMousePos.setY(0);
        break;
    default: break;
    }
}


//-----------------------------------------------------------------------------
bool RenderView::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
    case QEvent::KeyPress:
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
        this->moveCamera(event);
        break;
    default: break;
    }
    return QOpenGLWidget::eventFilter(watched, event);
}

}
