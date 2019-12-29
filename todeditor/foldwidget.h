#pragma once
#include <QWidget>
namespace tod::editor
{

class FoldWidget : public QWidget
{
public:
    FoldWidget(const QString& name, const QString& icon_name="");
    
    void setContent(QWidget* widget);
    
private:
    QWidget* content;
};

}
