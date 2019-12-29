#include <QVBoxLayout>
#include <QIcon>
#include <QPixmap>
#include "todeditor/foldwidget.h"
#include "todeditor/clickablelabel.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
FoldWidget::FoldWidget(const QString& name, const QString& icon_name)
: content(nullptr)
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
    auto title_frame = new QFrame();
    title_frame->setLayout(new QHBoxLayout());
    title_frame->layout()->setSpacing(0);
    title_frame->layout()->setContentsMargins(0, 0, 0, 0);
    auto name_label = new ClickableLabel(name);
    name_label->setStyleSheet(".QLabel { background:none; }");
    name_label->setContentsMargins(2, 2, 2, 2);
    name_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(name_label, &ClickableLabel::mousePressed,
    [this, name_label]()
    {
        if (this->content->isVisible()) { this->content->hide(); }
        else { this->content->show(); }
    });

    if (false == icon_name.isEmpty())
    {
        auto icon_widget = new QLabel(this);
        icon_widget->setStyleSheet(".QLabel { background:none; margin:0px 4px 0px 4px }");
        icon_widget->setPixmap(QPixmap(icon_name));
        title_frame->layout()->addWidget(icon_widget);
    }

    title_frame->layout()->addWidget(name_label);
    
    //정렬
    main_layout->addWidget(title_frame);
    main_layout->addWidget(content, 1);
}


//-----------------------------------------------------------------------------
void FoldWidget::setContent(QWidget* widget)
{
    while (auto item = this->content->layout()->takeAt(0))
    {
        delete item->widget();
        delete item;
    }
    static_cast<QVBoxLayout*>
    (this->content->layout())->addWidget(widget, 1);
}


}
