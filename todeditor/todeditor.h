#pragma once
#include <QMainWindow>
#include "tod/singleton.h"

namespace Ui {
class TodEditor;
}

namespace tod::editor
{

class TodEditor : public QMainWindow
{
    Q_OBJECT
public:
    explicit TodEditor(QWidget *parent = 0);
    ~TodEditor();
    
    void addToolMenu(QAction* menu_action);
    
public:
    static TodEditor* instance()
    {
        return s_instance;
    }
    static TodEditor* s_instance;

private:
    Ui::TodEditor* ui;
};

}
