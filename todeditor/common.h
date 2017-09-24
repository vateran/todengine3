#pragma once
#include <QAction>
#include <QApplication>
#include <list>
#include "tod/node.h"
#include "tod/singleton.h"
namespace tod::editor
{

typedef std::list<ObjRef<Node>> NodeSelections;

class TodEditorAction : public Singleton<TodEditorAction, QObject>
{
public:
    TodEditorAction()
    {
        this->copyAction = new QAction("Copy");
        this->copyAction->setShortcut(tr("Ctrl+C"));
        
        this->pasteAction = new QAction("Paste");
        this->pasteAction->setShortcut(tr("Ctrl+V"));
    }
    
public:
    QAction* copyAction;
    QAction* pasteAction;
    
};
    
}
