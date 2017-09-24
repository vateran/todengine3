#include "todeditor.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include "ui_todeditor.h"
#include "todeditor/common.h"
#include "tod/kernel.h"
#include "tod/objref.h"
#include "tod/filesystem.h"
#include "tod/graphics/main.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/transform44.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/modelcomponent.h"
#include "todeditor/objectselectmgr.h"
#include "todeditor/inspector.h"
#include "todeditor/nodehierarchy.h"
#include "todeditor/renderview.h"
namespace tod::editor
{

//compo_list->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);



class RenderTargetViewer :
  public DockWidget<RenderTargetViewer, QOpenGLWidget>
, protected QOpenGLExtraFunctions
{
protected:
    class View : public QOpenGLWidget, protected QOpenGLExtraFunctions
    {
    public:
        View(RenderTargetViewer* self):
        self(self)
        {
            auto update_timer = new QTimer();
            this->connect(update_timer, &QTimer::timeout, [this]()
                {
                    this->update();
                });
            update_timer->start(0);
            update_timer->setInterval(20);
        }
        
        void initializeGL()
        {
            this->initializeOpenGLFunctions();

            this->renderer = static_cast<tod::graphics::Renderer*>
                (Kernel::instance()->create("OpenGlRenderer", "/sys/renderer"));
            this->renderer->initialize((QOpenGLExtraFunctions*)this, 1540, 1316, true);
        }


        void resizeGL(int w, int h)
        {
            this->viewPortWidth = w;
            this->vierPortHeight = h;
        }


        void paintGL()
        {
            if (nullptr == this->self->targetTexture) return;
            
            this->renderer->setViewport(
                this->viewPortWidth * graphics::Renderer::ScreenScale(),
                this->vierPortHeight * graphics::Renderer::ScreenScale());
            this->renderer->drawTexture(this->self->targetTexture);
        }
        
    private:
        int viewPortWidth;
        int vierPortHeight;
        tod::graphics::Renderer* renderer;
        RenderTargetViewer* self;
    };

    RenderTargetViewer():
    DockWidget<RenderTargetViewer, QOpenGLWidget>
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
            names.reserve(renderer->getNamedTextures().size());
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
            }
            
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
    friend class DockWidget<RenderTargetViewer, QOpenGLWidget>;
    
private:
    graphics::Texture* targetTexture;
};



    
TodEditor* TodEditor::s_instance = nullptr;
TodEditor::TodEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TodEditor)
{
    TodEditor::s_instance = this;
    ui->setupUi(this);
    
    Kernel::instance()->init();
    std::list<tod::String> paths;
    paths.push_back(".");
    paths.push_back("../..");
    paths.push_back("../../../..");
    FileSystem::instance()->setSearchPaths(paths);
    auto ss = FileSystem::instance()->getCurrentWorkingDirectory();
    printf("%s\n", ss.c_str());
    tod::graphics::init();
    

    

    auto tab_scene_view = new QTabWidget();
    this->setCentralWidget(tab_scene_view);
    
    auto render_view = new RenderView();
    tab_scene_view->addTab(render_view, "Editor");
    



    Serializer s;
    auto scene_node = s.deserializeFromJsonFile("autosave.json");
    if (nullptr != scene_node)
    {
        Kernel::instance()->getRootNode()->addChild(scene_node);
    }



    //test code
    /*Kernel::instance()->create("Node", "/scene");
    
    auto model = Kernel::instance()->create("Node", "/scene/model");
    auto model_t = new graphics::TransformComponent();
    //model_t->setRotation({200, 0, 0});
    model->addComponent(model_t);
    auto model_compo = new graphics::ModelComponent();
    model->addComponent(model_compo);
    model_compo->setUri("nanosuit/nanosuit.blend");
    auto model_s = new graphics::ShaderComponent;
    model_s->setVShaderFileName("BasicShader.glvs");
    model_s->setPShaderFileName("BasicShader.glfs");
    model->addComponent(model_s);
    
    
    auto camera = Kernel::instance()->create("Node", "/scene/camera");
    auto camera_t = new graphics::TransformComponent;
    //camera_t->setTranslation({0, 0, 1.5});
    camera_t->setTranslation({5, 13, 18});
    camera_t->setRotation({14, -11, 0});
    camera->addComponent(camera_t);
    auto camera_s = new graphics::ShaderComponent;
    camera_s->setVShaderFileName("BasicShader.glvs");
    camera_s->setPShaderFileName("BasicShader.glfs");
    camera->addComponent(camera_s);
    auto camera_c = new graphics::CameraComponent;
    camera->addComponent(camera_c);*/
    
    
    
    
    
    
    
    NodeHierarchy::instance()->show();
    Inspector::instance()->show();
    RenderTargetViewer::instance()->show();
}

TodEditor::~TodEditor()
{
    delete ui;
}
    
    
void TodEditor::addToolMenu(QAction* menu_action)
{
    this->ui->menuTool->addAction(menu_action);
}

}
