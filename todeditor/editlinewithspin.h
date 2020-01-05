#pragma once
#include <QLineEdit>
namespace tod::editor
{

class EditLineWithSpin : public QLineEdit
{
Q_OBJECT
public:
    EditLineWithSpin(bool is_numeric=true);

Q_SIGNALS:
    void cancelEditing();

private:
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    bool isNumeric;
};

}
