#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include "todeditor/inspector.h"
#include "todeditor/propertyset.h"
#include "todeditor/addobjectdialog.h"
#include "todeditor/objectselectmgr.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
void Inspector::clear_object_info()
{
    while (auto item = this->objectInfo->layout()->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
}


//-----------------------------------------------------------------------------
void Inspector::build_object_info(Node* node)
{
    auto main_layout = static_cast<QBoxLayout*>(this->objectInfo->layout());

    //Inherit Type list
    auto cur_type = node->getType();
    while (nullptr != cur_type)
    {
        auto prop_set = new PropertySet(node, cur_type);
        main_layout->addWidget(prop_set);
        cur_type = cur_type->getBase();
    }
    
    //Component list
    for (auto& compo : node->getComponents())
    {
        auto prop_set = new PropertySet(compo, compo->getType());
        main_layout->addWidget(prop_set);
    }
    
    //Add Component Button
    main_layout->addSpacing(20);
    auto add_component_btn = new QPushButton("Add Component");
    main_layout->addWidget(add_component_btn);
    connect(add_component_btn, &QPushButton::clicked, [this, node]()
        {
            AddObjectDialog dlg(Component::get_type());
            if (QDialog::Accepted != dlg.exec()) return;
            
            auto new_compo = static_cast<Component*>(dlg.getCreatedObject());
            node->addComponent(new_compo);
            
            this->clear_object_info();
            this->build_object_info(node);
        });
    
    this->objectInfo->show();
}


//-----------------------------------------------------------------------------
void Inspector::setSelections(const NodeSelections& selections)
{
    this->clear_object_info();
    
    if (selections.empty()) return;
    
    this->build_object_info(const_cast<Node*>(selections.front().get()));
}


//-----------------------------------------------------------------------------
Inspector::Inspector():
DockWidget<Inspector>
(DockWidgetOption()
 .setName("Inspector")
 .setMinSize(QSize(250, 100))
 .setDockArea(Qt::RightDockWidgetArea))
{
    this->setStyleSheet(".QWidget { background-color:#232629; }");
    auto main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(1);
    main_layout->setAlignment(Qt::AlignTop);
    
    //Filter
    auto filter_edit = new QLineEdit();
    filter_edit->setPlaceholderText("Filter");
    filter_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filter_edit->setClearButtonEnabled(true);
    main_layout->addWidget(filter_edit);
    
    //Object Info
    auto object_info_layout = new QVBoxLayout();
    object_info_layout->setContentsMargins(1, 1, 1, 1);
    object_info_layout->setAlignment(Qt::AlignTop);
    object_info_layout->setSpacing(1);
    this->objectInfo = new QWidget();
    this->objectInfo->setLayout(object_info_layout);
    main_layout->addWidget(this->objectInfo);
    
    main_layout->addStretch(1);
    
    
    ObjectSelectMgr::instance()->addEventHandler(
    ObjectSelectMgr::EVENT_SELECTION_CHANGED, this, [this](Params*)
    { this->setSelections(ObjectSelectMgr::instance()->getSelections()); });
}

}
