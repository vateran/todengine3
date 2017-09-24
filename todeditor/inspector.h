#pragma once
#include <QWidget>
#include "tod/node.h"
#include "todeditor/common.h"
#include "todeditor/dockwidget.h"
namespace tod::editor
{

class Inspector : public DockWidget<Inspector>
{
public:
    
    
private:
    void clear_object_info();
    void build_object_info(Node* node);
    void setSelections(const NodeSelections& selections);
    
private:
    Inspector();
    friend class DockWidget<Inspector>;
    
private:
    QWidget* objectInfo;
};

}
