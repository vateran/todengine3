#include "todeditor/colorpickpushbutton.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
void ColorPickPushButton::setColor(const graphics::Color& value)
{
    this->setStyleSheet(QString::fromStdString(
        String::fromFormat(".QPushButton { background-color:#%s; }",
            value.toWebColorStr().c_str())));
}

}
