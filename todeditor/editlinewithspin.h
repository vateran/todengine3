#pragma once
#include <QLineEdit>
namespace tod::editor
{

class EditLineWithSpin : public QLineEdit
{
private:
    void focusInEvent(QFocusEvent* event) override;
    void wheelEvent(QWheelEvent* event);
};

}
