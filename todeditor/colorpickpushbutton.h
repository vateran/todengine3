#pragma once
#include <QPushButton>
#include <QColorDialog>
#include "tod/object.h"
#include "tod/graphics/color.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
class ColorPickPushButton : public QPushButton
{
public:
    template <typename PROPERTY_TYPE>
    ColorPickPushButton(Object* object, SimpleProperty<PROPERTY_TYPE>* sprop);
    
    void setColor(const graphics::Color& value);
};


//-----------------------------------------------------------------------------
template <typename PROPERTY_TYPE>
ColorPickPushButton::ColorPickPushButton
(Object* object, SimpleProperty<PROPERTY_TYPE>* sprop)
{
    this->setMaximumSize(24, 14);
    this->setColor(sprop->get(object));
    
    this->connect(this, &QPushButton::clicked, [this, object, sprop]()
    {
        QColorDialog dlg(QColorDialog::ShowAlphaChannel, this);
        const auto& prev_color = sprop->get(object);
        dlg.setCurrentColor(QColor(prev_color.r, prev_color.g, prev_color.b, prev_color.a));
        this->connect(&dlg, &QColorDialog::currentColorChanged,
        [this, object, sprop](const QColor& color)
        {
            graphics::Color c(
                color.red(), color.green(), color.blue(), color.alpha());
            sprop->set(object, c);
            this->setColor(c);
        });
        dlg.exec();
    });
}

}
