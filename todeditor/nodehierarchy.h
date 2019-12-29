#pragma once
#include <QTreeView>
#include <QMenu>
#include "tod/node.h"
#include "todeditor/dockwidget.h"
namespace tod::editor
{

class NodeHierarchy : public DockWidget<NodeHierarchy>
{
    Q_OBJECT
public:
    class TreeModel : public QAbstractItemModel
    {
    public:
        TreeModel(Node* root_node);
        virtual~TreeModel();
        
        QModelIndex index(int32 row, int32 column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int32 rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int32 columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex& index, int32 role = Qt::DisplayRole) const override;
        bool insertRows(int32 row, int32 count, const QModelIndex& parent) override;
        bool removeRows(int32 row, int32 count, const QModelIndex& parent) override;
        bool moveRows(const QModelIndex &sourceParent, int32 sourceRow,
            int32 count, const QModelIndex &destinationParent, int32 destinationChild) override;
        Qt::DropActions supportedDragActions() const override;
        Qt::DropActions supportedDropActions() const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QStringList mimeTypes() const override;
        QMimeData* mimeData(const QModelIndexList& indexes) const override;
        bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                          int32 row, int, const QModelIndex& parent) override;
        
    private:
        Node* getNode(const QModelIndex& index) const;
        
    private:
        Node* rootNode;
    };
    
private:
    NodeHierarchy();
    virtual~NodeHierarchy();
    friend class DockWidget<NodeHierarchy>;
    
private:
    QTreeView* nodeTree;
    QMenu* contextMenu;
    
};

}

