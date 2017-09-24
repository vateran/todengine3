#pragma once
#include <QWidget>
namespace tod::editor
{

class FoldWidget : public QWidget
{
public:
    FoldWidget(const QString& name);
    
    void setContent(QWidget* widget);
    
private:
    QWidget* content;
};

}
