#include <QGridLayout>
#include <QLabel>
#include <QAction>
#include <QComboBox>
#include <QCheckBox>
#include <QFileDialog>
#include "tod/filesystem.h"
#include "tod/graphics/color.h"
#include "tod/graphics/vector3.h"
#include "tod/graphics/vector4.h"
#include "todeditor/propertyset.h"
#include "todeditor/colorpickpushbutton.h"
#include "todeditor/editlinewithspin.h"
#include "todeditor/editablelabel.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
PropertySet::PropertySet(Object* object, Type* cur_type):
  FoldWidget(cur_type->getName().c_str(), cur_type->getEditorIcon().c_str())
, object(object)
{
    auto prop_list = new QWidget();
    prop_list->setStyleSheet(
        "QLineEdit { border:none; background-color:#171a1c; padding:1px; } "
        "QCheckBox { background:none; } QLabel { background:none; }");
    auto prop_list_layout = new QGridLayout(prop_list);
    prop_list_layout->setAlignment(Qt::AlignTop);
    prop_list_layout->setContentsMargins(0, 0, 0, 0);
    prop_list_layout->setSpacing(0);
    prop_list_layout->setColumnStretch(1, 1);
    prop_list_layout->setColumnMinimumWidth(0, 80);
    this->setContent(prop_list);
    int32 row = 0;
    for (auto& prop : cur_type->getPropertyOrder())
    {
        QWidget* label_background = nullptr;
        if (1 == (row % 2))
        {
            label_background = new QWidget();
            label_background->setStyleSheet(".QWidget { background-color:#1d2022; border-radius:0px; border:0px; }");
            label_background->setLayout(new QVBoxLayout);
            label_background->layout()->setContentsMargins(0, 0, 0, 0);
        }

        auto label = new QLabel(prop->getName().c_str());
        label->setContentsMargins(5, 3, 5, 3);
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        if (prop->isReadOnly())
            label->setStyleSheet(".QLabel { background:none; color:#464d53; }");
        else 
            label->setStyleSheet(".QLabel { background:none; color:#ababab; }");
        if (nullptr != label_background)
        {
            label_background->layout()->addWidget(label);
            prop_list_layout->addWidget(label_background, row, 0);
        }
        else
        {
            prop_list_layout->addWidget(label, row, 0);
        }
        
        QWidget* edit = nullptr;
        if (prop->isEnum())
        {
            auto combobox = new QComboBox();
            edit = combobox;
            typedef EnumProperty<int> EP;
            auto eprop = static_cast<EP*>(prop);
            QStringList combo_items;
            for (auto& s : eprop->getEnumList())
                combo_items.append(
                    QString::fromLocal8Bit(std::get<1>(s).c_str()));
            combobox->addItems(combo_items);
            combobox->setCurrentIndex(eprop->get(this->object));
            this->updaters.insert(std::make_pair(prop,
                [this, combobox, eprop]()
                { combobox->setCurrentIndex(eprop->get(this->object)); }));
        }
        else if (prop->isEqualType(typeid(bool)))
        {
            auto checkbox = new QCheckBox();
            edit = checkbox;
            typedef SimpleProperty<bool> SP;
            auto sprop = static_cast<SP*>(prop);
            checkbox->setChecked(sprop->get(object));
            this->connect(checkbox, &QCheckBox::stateChanged,
                [this, sprop](int32 state)
                {
                    sprop->set(this->object, state == Qt::Checked);
                });
            this->updaters.insert(std::make_pair(prop,
                [this, sprop, checkbox]()
                { checkbox->setChecked(sprop->get(this->object)); }));
        }
        else if (prop->isEqualType(typeid(const graphics::Color&)))
        {
            typedef SimpleProperty<const graphics::Color&> SP;
            auto sprop = static_cast<SP*>(prop);
        
            auto btn = new ColorPickPushButton(this->object, sprop);
            edit = btn;
            btn->setColor(sprop->get(this->object));
            this->updaters.insert(std::make_pair(prop,
                [this, sprop, btn]()
                {
                    btn->setColor(sprop->get(this->object));
                }));
        }
        else if (prop->isEqualType(typeid(const graphics::Vector3&)))
        {
            edit = new QWidget();
            auto h_layout = new QHBoxLayout(edit);
            h_layout->setContentsMargins(0, 0, 0, 0);
            h_layout->setSpacing(8);
            h_layout->setAlignment(Qt::AlignLeft);
            
            typedef SimpleProperty<const graphics::Vector3&> SP;
            auto sprop = static_cast<SP*>(prop);
            auto value { sprop->get(object) };
            for (int32 i=0;i<3;++i)
            {
                auto editbox = new EditableLabel(true, prop->isReadOnly());
                editbox->setText(QString::number(value.array[i], 'f', 3));
                h_layout->addWidget(editbox);
                this->connect(editbox, &EditableLabel::textEdited,
                    [this, sprop, editbox, i](const QString& text)
                    {
                        auto value{ sprop->get(this->object) };
                        value.array[i] = text.toFloat();
                        sprop->set(this->object, value);
                        editbox->setText(QString::number(value.array[i], 'f', 3));
                    });
            }
            this->updaters.insert(std::make_pair(prop,
                [this, sprop, h_layout]()
                {
                    for (auto i=0;i<3;++i)
                    {
                        auto editbox = static_cast<EditableLabel*>
                            (h_layout->itemAt(i)->widget());
                        String value;
                        sprop->toString(this->object, value);
                        editbox->setText(QString::fromLocal8Bit(value.c_str()));
                    };
                }));
        }
        else if (prop->isEqualType(typeid(const graphics::Vector4&)))
        {
            edit = new QWidget();
            auto h_layout = new QHBoxLayout(edit);
            h_layout->setContentsMargins(0, 0, 0, 0);
            h_layout->setSpacing(8);
            h_layout->setAlignment(Qt::AlignLeft);
            
            typedef SimpleProperty<const graphics::Vector4&> SP;
            auto sprop = static_cast<SP*>(prop);
            auto value { sprop->get(object) };
            for (int32 i=0;i<4;++i)
            {
                auto editbox = new EditableLabel(true, prop->isReadOnly());
                editbox->setText(QString::number(value.array[i], 'f', 3));
                editbox->setMaximumWidth(50);
                h_layout->addWidget(editbox);
                this->connect(editbox, &EditableLabel::textEdited,
                    [this, sprop, editbox, i](const QString& text)
                    {
                        auto value{ sprop->get(this->object) };
                        value.array[i] = text.toFloat();
                        sprop->set(this->object, value);
                        editbox->setText(QString::number(value.array[i], 'f', 3));
                    });
            }
            this->updaters.insert(std::make_pair(prop,
                [this, sprop, h_layout]()
                {
                    for (auto i=0;i<4;++i)
                    {
                        auto editbox = static_cast<EditableLabel*>
                            (h_layout->itemAt(i)->widget());
                        String value;
                        sprop->toString(this->object, value);
                        editbox->setText(QString::fromLocal8Bit(value.c_str()));
                    };
                }));
        }
        else
        {            
            EditableLabel* editbox = new EditableLabel(prop->isNumeric(), prop->isReadOnly());
            edit = editbox;

            String prop_value;
            prop->toString(this->object, prop_value);
            editbox->setText(QString::fromLocal8Bit(prop_value.c_str()));
            connect(editbox, &EditableLabel::textEdited,
                [this, prop, editbox](const QString& text)
                {
                    prop->fromString(this->object,
                        text.toLocal8Bit().data());
                });
            this->updaters.insert(std::make_pair(prop,
                [this, prop, editbox]()
                {
                    String value;
                    prop->toString(this->object, value);
                    editbox->setText(QString::fromLocal8Bit(value.c_str()));
                }));
        }

        edit->setEnabled(!prop->isReadOnly());
        
        QWidget* edit_background = nullptr;
        if (1 == (row % 2))
        {
            edit_background = new QWidget();
            edit_background->setStyleSheet(".QWidget { background-color:#1d2022; border-radius:0px; border:0px; }");
            edit_background->setLayout(new QVBoxLayout);
            edit_background->layout()->setContentsMargins(0, 0, 0, 0);
            edit_background->layout()->addWidget(edit);
            prop_list_layout->addWidget(edit_background, row, 1);
        }
        else
        {
            prop_list_layout->addWidget(edit, row, 1);
        }

        switch (prop->getEditorType())
        {
        case Property::EDITORTYPE_FILEDIALOG:
            {
                auto btn = new QPushButton("...");
                btn->setMaximumHeight(18);
                btn->setFlat(true);
                prop_list_layout->addWidget(btn, row, 2);
                connect(btn, &QPushButton::clicked,
                    [this, prop, edit]()
                    {
                        auto path = QFileDialog::getOpenFileName(this, tr("Open"));
                        if (path.isEmpty()) return;
                        auto rel_path = FileSystem::instance()->
                            getRelativePath(path.toLocal8Bit().data());
                        prop->fromString(this->object, rel_path);
                        this->updateValue(prop);
                    });
                break;
            }
        default:
            break;
        }
        
        ++row;
    }
}


//-----------------------------------------------------------------------------
void PropertySet::updateValues()
{
    for (auto& i : this->updaters)
        i.second();
}


//-----------------------------------------------------------------------------
void PropertySet::updateValue(Property* prop)
{
    auto i = this->updaters.find(prop);
    if (this->updaters.end() == i) TOD_RETURN_TRACE(;);
    i->second();
}

}
