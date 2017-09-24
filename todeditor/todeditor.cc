#include "todeditor.h"
#include "ui_todeditor.h"

TodEditor::TodEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TodEditor)
{
    ui->setupUi(this);
}

TodEditor::~TodEditor()
{
    delete ui;
}
