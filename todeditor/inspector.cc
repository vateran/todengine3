#pragma once
#include <QWidget>
#include <QVBoxLayout>
namespace tod::editor
{

class FoldWidget : public QWidget
{
public:
    FoldWidget(const QString& name):
    content(nullptr)
    {
        auto main_layout = new QVBoxLayout(this);
        main_layout->setContentsMargins(0, 0, 0, 0);
        main_layout->setSpacing(1);
        main_layout->setAlignment(Qt::AlignTop);
        
        //컨텐츠
        this->content = new QWidget();
        this->content->setLayout(new QVBoxLayout());
        this->content->layout()->setContentsMargins(0, 0, 0, 0);
        
        //제목
        auto name_label = new ClickableLabel(name);
        name_label->setContentsMargins(5, 5, 5, 5);
        name_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        connect(name_label, &ClickableLabel::mousePressed,
        [this, name_label]()
        {
            if (this->content->isVisible()) { this->content->hide(); }
            else { this->content->show(); }
        });
        
        //정렬
        main_layout->addWidget(name_label);
        main_layout->addWidget(content, 1);
    }
    
    void setContent(QWidget* widget)
    {
        while (auto item = this->content->layout()->takeAt(0))
        {
            delete item->widget();
            delete item;
        }
        static_cast<QVBoxLayout*>
        (this->content->layout())->addWidget(widget, 1);
    }
    
private:
    QWidget* content;
};

}
