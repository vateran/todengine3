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
    virtual~NodeHierarchy();
    
    class TreeModel : public QAbstractItemModel
    {
    public:
        TreeModel(Node* root_node);
        virtual~TreeModel();
        
        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
        QModelIndex parent(const QModelIndex& index) const override;
        int rowCount(const QModelIndex& parent = QModelIndex()) const override;
        int columnCount(const QModelIndex&) const override;
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
        bool insertRows(int row, int count, const QModelIndex& parent) override;
        bool removeRows(int row, int count, const QModelIndex& parent) override;
        bool moveRows(const QModelIndex &sourceParent, int sourceRow,
            int count, const QModelIndex &destinationParent, int destinationChild) override;
        Qt::DropActions supportedDragActions() const override;
        Qt::DropActions supportedDropActions() const override;
        Qt::ItemFlags flags(const QModelIndex& index) const override;
        QStringList mimeTypes() const override;
        QMimeData* mimeData(const QModelIndexList& indexes) const override;
        bool dropMimeData(const QMimeData* data, Qt::DropAction action,
                          int row, int, const QModelIndex& parent) override;
        
    private:
        Node* getNode(const QModelIndex& index) const;
        
    private:
        Node* rootNode;
    };
    
private:
    NodeHierarchy();
    friend class DockWidget<NodeHierarchy>;
    
private:
    QTreeView* nodeTree;
    QMenu* contextMenu;
    
};

}

