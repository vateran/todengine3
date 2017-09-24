#pragma once
#include <QAction>
#include <QDockWidget>
#include "todeditor/todeditor.h"
namespace tod::editor
{
    
class DockWidgetOption
{
public:
    DockWidgetOption& setName(const QString& value)
    {
        this->name = value;
        return *this;
    }
    DockWidgetOption& setMinSize(const QSize& value)
    {
        this->minSize = value;
        return *this;
    }
    DockWidgetOption& setDockArea(const Qt::DockWidgetArea& value)
    {
        this->dockArea = value;
        return *this;
    }
    
    QString name;
    QSize minSize;
    Qt::DockWidgetArea dockArea;
};

template <typename T, typename BASE=QWidget>
class DockWidget : public BASE
{
public:
    DockWidget(const DockWidgetOption& option):
      menuAction(nullptr)
    , dockWidget(nullptr)
    {
        this->menuAction = new QAction(option.name);
        this->menuAction->setCheckable(true);
        this->connect(this->menuAction, &QAction::triggered,
        [this, option](bool checked)
        {
            if (nullptr == this->dockWidget)
            {
                this->dockWidget = new QDockWidget(option.name);
                this->dockWidget->setWidget(this);
                this->dockWidget->setMinimumSize(option.minSize);
                TodEditor::instance()->addDockWidget(
                    option.dockArea,
                    this->dockWidget);
                this->connect(this->dockWidget, &QDockWidget::visibilityChanged,
                [this](bool visibility)
                { this->menuAction->setChecked(visibility); });
                TodEditor::instance()->addToolMenu(this->menuAction);
            }
            if (checked)
            {
                this->dockWidget->show();
            }
            else
            {
                this->dockWidget->close();
            }
        });
    }
    void show()
    {
        if (!this->menuAction->isChecked())
            this->menuAction->trigger();
    }
    
public:
    static T* instance()
    {
        static T* s_instance = nullptr;
        if (s_instance) return s_instance;
        return (s_instance = new T());
    }
    
private:
    QAction* menuAction;
    QDockWidget* dockWidget;
};
    
}
