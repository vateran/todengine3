#include <QVBoxLayout>
#include <QLineEdit>
#include <QTableWidget>
#include <QTreeWidget>
#include <QHeaderView>
#include <QDialogButtonBox>
#include "tod/kernel.h"
#include "todeditor/addobjectdialog.h"
namespace tod::editor
{

//-----------------------------------------------------------------------------
AddObjectDialog::AddObjectDialog(Type* base_type):
createdObject(nullptr)
{
    auto main_layout = new QVBoxLayout(this);
    
    auto filter = new QLineEdit();
    filter->setPlaceholderText("Search");
    main_layout->addWidget(filter);

    auto obj_list = new QTableWidget();
    obj_list->setColumnCount(1);
    obj_list->setHorizontalHeaderLabels({ "Type" });
    obj_list->setSelectionMode(QTreeWidget::SingleSelection);
    obj_list->setGridStyle(Qt::NoPen);
    obj_list->verticalHeader()->hide();
    obj_list->verticalHeader()->setDefaultSectionSize(18);
    obj_list->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    obj_list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->add_derived_objects(obj_list, base_type);
    main_layout->addWidget(obj_list);
    
    auto btns = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    main_layout->addWidget(btns);
    
    this->connect(filter, &QLineEdit::textChanged,
        [filter, obj_list](const QString& text)
        {
            String filter_text(text.toLower().toLocal8Bit().data());
            int32 first_row = -1;
            for (int32 i=0;i<obj_list->rowCount();++i)
            {
                auto item = obj_list->item(i, 0);
                String node_name = (item->text().toLower().toLocal8Bit().data());
                if (node_name.find(filter_text)!=String::npos)
                {
                    obj_list->setRowHidden(i, false);
                    if (-1 == first_row) first_row = i;
                }
                else
                {
                    obj_list->setRowHidden(i, true);
                }
            }
            obj_list->setCurrentIndex(obj_list->model()->index(first_row, 0));
        });
    this->connect(btns, &QDialogButtonBox::accepted,
        [this, obj_list]()
        {
            auto cur_item = obj_list->currentItem();
            if (nullptr == cur_item) return;
            auto new_object = Kernel::instance()->create(
                cur_item->text().toLocal8Bit().data());
            if (nullptr == new_object) return;
            this->createdObject = new_object;
            this->accept();
        });
    this->connect(btns, &QDialogButtonBox::rejected, [this]()
        {
            this->createdObject = nullptr;
            this->reject();
        });
}


//-----------------------------------------------------------------------------
Object* AddObjectDialog::getCreatedObject()
{
    return this->createdObject;
}


//-----------------------------------------------------------------------------
void AddObjectDialog::add_derived_objects(QTableWidget* table, Type* type)
{
    if (!type->isAbstract())
    {
        auto row = table->rowCount();
        table->insertRow(row);
        table->setItem(row, 0,
            new QTableWidgetItem(
                QString::fromLocal8Bit(type->getName().c_str())));
    }

    for (auto& dtype : type->getDerivedTypes())
    {
        this->add_derived_objects(table, dtype);
    }
}

}
