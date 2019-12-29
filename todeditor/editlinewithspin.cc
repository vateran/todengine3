#include <QWheelEvent>
#include "todeditor/editlinewithspin.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
EditLineWithSpin::EditLineWithSpin(bool is_numeric)
: isNumeric(is_numeric)
{
}

//-----------------------------------------------------------------------------
void EditLineWithSpin::focusInEvent(QFocusEvent* event)
{
    QLineEdit::focusInEvent(event);
    this->selectAll();
}


//-----------------------------------------------------------------------------
void EditLineWithSpin::wheelEvent(QWheelEvent* event)
{
    if (false == this->isNumeric) return;
    if (false == this->hasFocus()) return;

    auto prev = this->text().toFloat();
    auto new_value = prev + (event->delta() / 120.0f * 10);
    this->setText(QString::number(new_value, 'f', 3));
    emit textEdited(this->text());

    QLineEdit::wheelEvent(event);
}


//-----------------------------------------------------------------------------
void EditLineWithSpin::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        emit cancelEditing();
        return;
    }

    QLineEdit::keyPressEvent(event);
}

}

