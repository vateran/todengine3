#pragma once
#include <QTextEdit>
#include "tod/node.h"
#include "todeditor/dockwidget.h"
namespace tod::editor
{

class LogOutput : public DockWidget<LogOutput>
{
    Q_OBJECT
private:
    LogOutput();
    virtual~LogOutput();
    friend class DockWidget<LogOutput>;

private:
    QTextEdit* logText;
};

}

