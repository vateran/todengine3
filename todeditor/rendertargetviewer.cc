#include "todeditor/rendertargetviewer.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QMenu>
#include "tod/kernel.h"
#include "tod/graphics/renderer.h"
namespace tod::editor
{

//----------------------------------------------------------------------------
class RenderTargetViewer::View
    #ifdef TOD_PLATFORM_WINDOWS
    : public QWidget
    #else
    : public QOpenGLWidget
    , protected QOpenGLExtraFunctions
    #endif
{
public:
    //------------------------------------------------------------------------
    View(RenderTargetViewer* self)
        : self(self)
    {
        auto update_timer = new QTimer();
        this->connect(update_timer, &QTimer::timeout, [this]()
        {
            this->update();
        });
        update_timer->start(0);
        update_timer->setInterval(20);
    }



    //------------------------------------------------------------------------
#ifdef TOD_PLATFORM_MAC
    void initializeGL()
    {
        this->initializeOpenGLFunctions();
    }


    //------------------------------------------------------------------------
    void resizeGL(int32 w, int32 h)
    {
        this->viewPortWidth = w;
        this->vierPortHeight = h;
    }


    //------------------------------------------------------------------------
    void paintGL()
    {
        if (nullptr == this->self->targetTexture) return;

        this->renderer->setViewport(
            this->viewPortWidth * graphics::Renderer::ScreenScale(),
            this->vierPortHeight * graphics::Renderer::ScreenScale());
        this->renderer->drawTexture(this->self->targetTexture);
    }
#endif

private:
    int32 viewPortWidth;
    int32 vierPortHeight;
    tod::graphics::Renderer* renderer;
    RenderTargetViewer* self;
};


//----------------------------------------------------------------------------
RenderTargetViewer::RenderTargetViewer()
: DockWidget<RenderTargetViewer, QWidget>
(DockWidgetOption()
    .setName("RenderTarget Viewer")
    .setMinSize(QSize(100, 100))
    .setDockArea(Qt::LeftDockWidgetArea))
, targetTexture(nullptr)
{
    auto main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(1);

    auto menu_bar = new QWidget;
    main_layout->addWidget(menu_bar);
    auto menu_bar_layout = new QHBoxLayout(menu_bar);
    menu_bar_layout->setContentsMargins(0, 0, 0, 0);

    auto select_rt_btn = new QPushButton("RenderTargets");
    this->connect(select_rt_btn, &QPushButton::clicked, [this]()
    {
        auto menu = new QMenu();

        ObjRef<graphics::Renderer> renderer(S("/sys/renderer"));
        std::vector<QString> names;
        /*names.reserve(renderer->getNamedTextures().size());
        for (auto& t : renderer->getNamedTextures())
        {
            names.push_back(QString::fromLocal8Bit(
                t.second->getName().c_str()));
        }
        std::sort(names.begin(), names.end());
        for (auto& name : names)
        {
            auto menu_action = menu->addAction(name);
            this->connect(menu_action, &QAction::triggered,
                [this, renderer, name](bool)
            {
                ObjRef<graphics::Renderer> renderer(S("/sys/renderer"));
                this->targetTexture = renderer->createTexture(name.toLocal8Bit().data());

            });
        }*/

        menu->move(QCursor::pos());
        menu->exec();
        delete menu;
    });
    menu_bar_layout->addWidget(select_rt_btn);
    menu_bar_layout->addStretch(1);

    auto view = new View(this);
    main_layout->addWidget(view, 1);

    //this->targetTexture = renderer->createTexture(S(""));
}

}