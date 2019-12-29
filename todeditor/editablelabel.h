#pragma once
#include <QLabel>
#include <QLineEdit>
namespace tod::editor
{

class EditableLabel : public QWidget
{
    Q_OBJECT
public:
    EditableLabel(bool is_numeric, bool is_readonly);

    void setText(const QString& text);
    QString text() const { return this->normalState->text(); }

Q_SIGNALS:
    void mousePressed();
    void returnPressed();
    void textChanged(const QString&);   //setText ȣ��ɶ��� �̺�Ʈ �߻�
    void textEdited(const QString&);    //setText ȣ��ɶ��� �̺�Ʈ �߻� X
    void cancelEditing();

private:
    void endEdit();
    void mousePressEvent(QMouseEvent* event) override;

private:
    bool isNumeric;
    bool isReadonly;
    QString prevValue;
    QLabel* normalState;
    QLineEdit* editState;
};

}