#include "todeditor/editablelabel.h"
#include <QMouseEvent>
#include <QVBoxLayout>
#include "tod/platformdef.h"
#include "todeditor/editlinewithspin.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
EditableLabel::EditableLabel(bool is_numeric, bool is_readonly)
: isNumeric(is_numeric)
, isReadonly(is_readonly)
, normalState(new QLabel)
, editState(nullptr)
{
    this->setLayout(new QVBoxLayout);
    this->layout()->setContentsMargins(0, 0, 0, 0);
    if (false == this->isReadonly)
    {
        this->normalState->setStyleSheet(".QLabel { background:none; color:#3daee9; font-weight: bold; }");
    }    
    this->layout()->addWidget(this->normalState);
}


//-----------------------------------------------------------------------------
void EditableLabel::setText(const QString& text)
{
    this->normalState->setText(text);
}


//-----------------------------------------------------------------------------
void EditableLabel::mousePressEvent(QMouseEvent* event)
{
    if (true == this->isReadonly) return;

    if (event->buttons() & Qt::LeftButton)
    {
        this->prevValue = this->normalState->text();
        this->normalState->hide();

        EditLineWithSpin* edit = new EditLineWithSpin(this->isNumeric);
        this->editState = edit;

        this->layout()->addWidget(this->editState);
        this->layout()->activate();
        this->editState->setText(this->normalState->text());
        this->editState->selectAll();
        this->editState->setFocus();

        this->connect(edit, &EditLineWithSpin::cancelEditing,
            [this]()
            {
                this->setText(this->prevValue);
                endEdit();

                emit textEdited(this->prevValue);
                emit cancelEditing();
            });

        this->connect(this->editState, &QLineEdit::textChanged,
            [this](const QString& text)
            {
                emit textChanged(text);
            });

        this->connect(this->editState, &QLineEdit::textEdited,
            [this](const QString& text)
            {
                emit textEdited(text);
            });

        this->connect(this->editState, &QLineEdit::returnPressed,
            [this]()
            {
                this->setText(this->editState->text());
                this->endEdit();

                emit returnPressed();
            });

        emit mousePressed();
    }
}


//-----------------------------------------------------------------------------
void EditableLabel::endEdit()
{
    this->normalState->show();
    this->layout()->removeWidget(this->editState);
    this->editState->deleteLater();
    this->layout()->activate();
}

}