#include "todeditor.h"
#include <QVBoxLayout>
#include <QTimer>
#include <QPushButton>
#include <QSettings>
#include "ui_todeditor.h"
#include "todeditor/common.h"
#include "tod/kernel.h"
#include "tod/objref.h"
#include "tod/filesystem.h"
#include "tod/serializer.h"
#include "tod/graphics/main.h"
#include "tod/graphics/renderer.h"
#include "tod/graphics/texture.h"
#include "tod/graphics/transform44.h"
#include "tod/graphics/transformcomponent.h"
#include "tod/graphics/shadercomponent.h"
#include "tod/graphics/cameracomponent.h"
#include "tod/graphics/modelcomponent.h"
#include "tod/graphics/inputlayoutcache.h"
#include "todeditor/objectselectmgr.h"
#include "todeditor/inspector.h"
#include "todeditor/nodehierarchy.h"
#include "todeditor/renderview.h"
#include "todeditor/logoutput.h"
#include "todeditor/rendertargetviewer.h"
namespace tod::editor
{

//compo_list->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);


//-----------------------------------------------------------------------------
TodEditor* TodEditor::s_instance = nullptr;
TodEditor::TodEditor(QWidget *parent)
: QMainWindow(parent)
, ui(new Ui::TodEditor)
{
    TodEditor::s_instance = this;
    ui->setupUi(this);
    
    Logger::instance()->setStandardLog(Logger::StandardLog().console());
    Kernel::instance()->init();
    std::list<tod::String> paths;
    paths.push_back("../..");
    paths.push_back("../../../..");
    FileSystem::instance()->setSearchPaths(paths);
    tod::graphics::init();
    tod::graphics::InputLayoutCache::instance()->createFromConfig("EngineData/InputLayout.config");
    


    Node::get_type()->setEditorIcon(S(":/Resources/icons/ic_folder_open_white_18dp.png"));
    tod::graphics::TransformComponent::get_type()->setEditorIcon(S(":/Resources/icons/ic_all_out_white_18dp.png"));    
    

    auto tab_scene_view = new QTabWidget();
    this->setCentralWidget(tab_scene_view);
    
    auto render_view = new RenderView();
    tab_scene_view->addTab(render_view, "Editor");
    

         



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
    LogOutput::instance()->show();







    Serializer s;
    auto scene_node = s.deserializeFromJsonFile("autosave.json");
    if (nullptr != scene_node)
    {
        Kernel::instance()->getRootNode()->addChild(scene_node);
    }
    else
    {
        Kernel::instance()->create("Node", "/scene");
    }


    readSettings();
}


//-----------------------------------------------------------------------------
TodEditor::~TodEditor()
{
    delete ui;
}
    

//-----------------------------------------------------------------------------
void TodEditor::addToolMenu(QAction* menu_action)
{
    this->ui->menuTool->addAction(menu_action);
}


//-----------------------------------------------------------------------------
void TodEditor::closeEvent(QCloseEvent* event)
{
    QSettings settings("vateran.net", "TodEditor");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
}


//-----------------------------------------------------------------------------
void TodEditor::readSettings()
{
    QSettings settings("vateran.net", "TodEditor");
    restoreGeometry(settings.value("myWidget/geometry").toByteArray());
    restoreState(settings.value("myWidget/windowState").toByteArray());
}

}
