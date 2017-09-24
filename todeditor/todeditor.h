#ifndef TODEDITOR_H
#define TODEDITOR_H

#include <QMainWindow>

namespace Ui {
class TodEditor;
}

class TodEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit TodEditor(QWidget *parent = 0);
    ~TodEditor();

private:
    Ui::TodEditor *ui;
};

#endif // TODEDITOR_H
