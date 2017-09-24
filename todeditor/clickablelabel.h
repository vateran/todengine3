#pragma once
#include <QLabel>
#include <QMouseEvent>

class ClickableLabel : public QLabel
{
    Q_OBJECT
public:
    ClickableLabel(const QString& text):QLabel(text) {}
    
signals:
    void mousePressed();
    
private:
    void mousePressEvent(QMouseEvent* event) override
    {
        if (event->buttons() & Qt::LeftButton)
        {
            emit mousePressed();
        }
    }
};
