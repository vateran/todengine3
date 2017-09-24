#pragma once
#include <QDialog>
#include <QTableWidget>
#include "tod/object.h"
namespace tod::editor
{

class AddObjectDialog : public QDialog
{
public:
    AddObjectDialog(Type* base_type=Object::get_type());
    Object* getCreatedObject();
    
private:
    void add_derived_objects(QTableWidget* table, Type* type);
    
private:
    Object* createdObject;
};

}
