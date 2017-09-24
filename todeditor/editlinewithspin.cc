#include <QWheelEvent>
#include "todeditor/editlinewithspin.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
void EditLineWithSpin::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    this->selectAll();
}


//-----------------------------------------------------------------------------
void EditLineWithSpin::wheelEvent(QWheelEvent* event)
{
    auto prev = this->text().toFloat();
    auto new_value = prev - (event->delta() / 120.0f * 10);
    this->setText(QString::number(new_value, 'f', 3));

    emit returnPressed();
    QLineEdit::wheelEvent(event);
}

}

