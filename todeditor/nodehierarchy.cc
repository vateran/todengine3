#include <QMimeData>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include "tod/kernel.h"
#include "tod/serializer.h"
#include "todeditor/common.h"
#include "todeditor/objectselectmgr.h"
#include "todeditor/nodehierarchy.h"
#include "todeditor/addobjectdialog.h"
namespace tod::editor
{


//-----------------------------------------------------------------------------
NodeHierarchy::NodeHierarchy():
DockWidget<NodeHierarchy>
(DockWidgetOption()
 .setName("Node Hierarchy")
 .setMinSize(QSize(100, 100))
 .setDockArea(Qt::LeftDockWidgetArea))
{
    auto main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(1);
    
    
    //Filter
    auto filter_edit = new QLineEdit();
    filter_edit->setPlaceholderText("Filter");
    filter_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    filter_edit->setClearButtonEnabled(true);
    main_layout->addWidget(filter_edit);
    
    
    //Node Tree
    auto model = new TreeModel(Kernel::instance()->lookup("/"));
    this->nodeTree = new QTreeView();
    this->nodeTree->setHeaderHidden(true);
    this->nodeTree->setRootIsDecorated(true);
    this->nodeTree->setIndentation(10);
    this->nodeTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    this->nodeTree->setContextMenuPolicy(Qt::CustomContextMenu);
    this->nodeTree->setDragEnabled(true);
    this->nodeTree->setAcceptDrops(true);
    this->nodeTree->setDropIndicatorShown(true);
    this->nodeTree->setModel(model);
    main_layout->addWidget(this->nodeTree, 1);
    this->connect(
        this->nodeTree->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        [this](const QItemSelection& selected,
               const QItemSelection&)
        {
            NodeSelections selections;
            for (auto& item : selected.indexes())
            {
                selections.push_back(static_cast<Node*>(item.internalPointer()));
            }
            ObjectSelectMgr::instance()->setSelections(selections);
        });
    this->connect(this->nodeTree,
        &QTreeView::customContextMenuRequested,
        [this](const QPoint& pt)
        {
            QModelIndex index = this->nodeTree->indexAt(pt);
            if (index.isValid())
            {
                auto node = static_cast<Node*>(index.internalPointer());
                if (nullptr == node) return;
                this->contextMenu->exec(this->nodeTree->mapToGlobal(pt));
            }
        });
    
    
    
    //ContextMenu
    this->contextMenu = new QMenu();
    auto add_node_action = new QAction("Add Node");
    this->connect(add_node_action, &QAction::triggered, [this, model]()
        {
            auto index = this->nodeTree->currentIndex();
            if (!index.isValid()) return;
            auto node = static_cast<Node*>(index.internalPointer());
            
            AddObjectDialog dlg(Node::get_type());
            if (QDialog::Accepted != dlg.exec()) return;
            
            auto new_node = static_cast<Node*>(dlg.getCreatedObject());
            new_node->setName(new_node->getType()->getName());
            node->addChild(new_node);
            
            model->insertRow(node->getChildCount(), index);
        });
    auto remove_node_action = new QAction("Remove Node");
    this->connect(remove_node_action, &QAction::triggered, [this, model]()
        {
            auto ret = QMessageBox::question(this, tr("Remove Node"), tr("Really?"));
            if (QMessageBox::StandardButton::No == ret) return;
            
            auto selection_model = this->nodeTree->selectionModel();
            for (auto& i : selection_model->selectedIndexes())
            {
                if (!i.isValid()) continue;
                model->removeRows(i.row(), 1, i.parent());
            }
        });
    auto save_action = new QAction("Save");
    this->connect(save_action, &QAction::triggered, [this]()
        {
            auto uri = QFileDialog::getSaveFileName(this, tr("Save"));
            if (uri.isEmpty()) return;
            auto index = this->nodeTree->currentIndex();
            if (!index.isValid()) return;
            auto node = static_cast<Node*>(index.internalPointer());
            Serializer s;
            s.serializeToJsonFile(node, uri.toLocal8Bit().data());
        });
    auto load_action = new QAction("Load");
    this->connect(load_action, &QAction::triggered, [this]()
        {
            auto uri = QFileDialog::getOpenFileName(this, tr("Load"));
            if (uri.isEmpty()) return;
            auto index = this->nodeTree->currentIndex();
            if (!index.isValid()) return;
            auto parent_node = static_cast<Node*>(index.internalPointer());
            Serializer s;
            auto node = s.deserializeFromJsonFile(uri.toLocal8Bit().data());
            if (nullptr == node) return;
            parent_node->addChild(node);
        });
    auto editor_action = new QAction("Editor");
    this->connect(editor_action, &QAction::triggered, [this]()
        {
        });
    this->contextMenu->addAction(add_node_action);
    this->contextMenu->addAction(remove_node_action);
    this->contextMenu->addSeparator();
    this->contextMenu->addAction(save_action);
    this->contextMenu->addAction(load_action);
    this->contextMenu->addSeparator();
    this->contextMenu->addAction(editor_action);
    
    
    //Copy
    this->addAction(TodEditorAction::instance()->copyAction);
    this->connect(TodEditorAction::instance()->copyAction,
    &QAction::triggered, [this, model](bool)
    {
        if (QApplication::focusWidget() != this->nodeTree) return;
        auto* clipboard = QApplication::clipboard();
        auto mime_data = new QMimeData;
        clipboard->setMimeData(mime_data);
        
        Serializer serializer;
        auto selection_model = this->nodeTree->selectionModel();
        for (auto& i : selection_model->selectedIndexes())
        {
            if (!i.isValid()) continue;
            auto node = static_cast<Node*>(i.internalPointer());
            if (nullptr == node) continue;
            
            String j_node;
            if (!serializer.serializeToJson(node, j_node)) continue;
            
            mime_data->setData(QString("text/node/%1")
                .arg(node->getAbsolutePath().hash()),
                QString::fromLocal8Bit(j_node.c_str()).toLocal8Bit());
        }
    });
    
    //Paste
    this->addAction(TodEditorAction::instance()->pasteAction);
    this->connect(TodEditorAction::instance()->pasteAction,
    &QAction::triggered, [this, model](bool)
    {
        if (QApplication::focusWidget() != this->nodeTree) return;
        auto* clipboard = QApplication::clipboard();
        auto mime_data = clipboard->mimeData();
        
        Serializer serializer;
        auto selection_model = this->nodeTree->selectionModel();
        for (auto& i : selection_model->selectedIndexes())
        {
            if (!i.isValid()) continue;
            auto node = static_cast<Node*>(i.internalPointer());
            if (nullptr == node) continue;
            
            for (auto& format_str : mime_data->formats())
            {
                auto data = mime_data->data(format_str);
                auto child_node = serializer.deserializeFromJson(data.data());
                if (nullptr == child_node) continue;
                node->addChild(child_node);
                model->insertRow(node->getChildCount(), i);
            }
        }
    });
}


//-----------------------------------------------------------------------------
NodeHierarchy::~NodeHierarchy()
{
    SAFE_DELETE(this->contextMenu);
}


//-----------------------------------------------------------------------------
NodeHierarchy::TreeModel::TreeModel(Node* root_node)
{
    this->rootNode = root_node;
}


//-----------------------------------------------------------------------------
NodeHierarchy::TreeModel::~TreeModel()
{
}


//-----------------------------------------------------------------------------
QModelIndex NodeHierarchy::TreeModel::index
(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
    auto parent_node = this->getNode(parent);
    if (nullptr == parent_node)
        return QModelIndex();
    if (parent_node->getChildCount() <= row)
        return QModelIndex();
    auto child_node = parent_node->getChildAt(row);
    if (nullptr == child_node)
        return QModelIndex();
    return this->createIndex(row, column, child_node);
}


//-----------------------------------------------------------------------------
QModelIndex NodeHierarchy::TreeModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    auto child_node = static_cast<Node*>(index.internalPointer());
    auto parent_node = child_node->getParent();
    if (nullptr == parent_node || parent_node == this->rootNode)
        return QModelIndex();
    if (nullptr == parent_node->getParent())
        return QModelIndex();
    auto row = parent_node->getParent()->indexOf(parent_node);
    return this->createIndex(row, 0, parent_node);
}


//-----------------------------------------------------------------------------
int NodeHierarchy::TreeModel::rowCount(const QModelIndex& parent) const
{
    if (parent.column() > 0) return 0;
    Node* parent_node = nullptr;
    if (!parent.isValid())
        parent_node = this->rootNode;
    else
        parent_node = static_cast<Node*>(parent.internalPointer());
    
    return parent_node->getChildCount();
}


//-----------------------------------------------------------------------------
int NodeHierarchy::TreeModel::columnCount(const QModelIndex&) const
{
    return 1;
}


//-----------------------------------------------------------------------------
QVariant NodeHierarchy::TreeModel::data
(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();
    auto node = this->getNode(index);
    return QVariant(node->getName().c_str());
}


//-----------------------------------------------------------------------------
bool NodeHierarchy::TreeModel::insertRows
(int row, int count, const QModelIndex& parent)
{
    auto parent_node = this->getNode(parent);
    if (nullptr == parent_node) return false;
    
    int begin = row;
    int end = row + count - 1;
    this->beginInsertRows(parent, begin, end);
    this->endInsertRows();
    
    return true;
}


//-----------------------------------------------------------------------------
bool NodeHierarchy::TreeModel::removeRows
(int row, int count, const QModelIndex& parent)
{
    auto parent_node = this->getNode(parent);
    if (nullptr == parent_node) return false;
    
    Node::Nodes holder;
    int begin = row;
    int end = row + count - 1;
    for (int i=begin;i<=end;++i)
    {
        auto node = parent_node->getChildAt(i);
        if (nullptr == node) continue;
        holder.push_back(node);
    }
    this->beginRemoveRows(parent, begin, end);
    for (auto& c : holder)
        c->removeFromParent();
    holder.clear();
    this->endRemoveRows();
    
    return true;
}


//-----------------------------------------------------------------------------
bool NodeHierarchy::TreeModel::moveRows
(const QModelIndex &sourceParent, int sourceRow,
 int count, const QModelIndex &destinationParent, int destinationChild)
{
    auto src_parent_node = static_cast<Node*>(sourceParent.internalPointer());
    auto dst_parent_node = static_cast<Node*>(destinationParent.internalPointer());
    if (src_parent_node == dst_parent_node) return false;
    
    Node::Nodes holder;
    for (int row=sourceRow;row<=sourceRow+count-1;++row)
    {
        auto node = src_parent_node->getChildAt(row);
        holder.push_back(node);
    }
    
    beginMoveRows(sourceParent, sourceRow, sourceRow+count-1, destinationParent, destinationChild);
    for (auto& node : holder)
    {
        node->removeFromParent();
        dst_parent_node->addChild(node);
    }
    endMoveRows();
    
    return true;
}


//-----------------------------------------------------------------------------
Qt::DropActions NodeHierarchy::TreeModel::supportedDragActions() const
{
    return Qt::MoveAction;
}


//-----------------------------------------------------------------------------
Qt::DropActions NodeHierarchy::TreeModel::supportedDropActions() const
{
    return Qt::MoveAction;
}


//-----------------------------------------------------------------------------
Qt::ItemFlags NodeHierarchy::TreeModel::flags(const QModelIndex& index) const
{
    auto prev_flags = QAbstractItemModel::flags(index);
    
    if (index.isValid())
        return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | prev_flags;
    else
        return Qt::ItemIsDropEnabled | prev_flags;
}


//-----------------------------------------------------------------------------
QStringList NodeHierarchy::TreeModel::mimeTypes() const
{
    QStringList types;
    types << "application/vnd.text.list";
    return types;
}


//-----------------------------------------------------------------------------
QMimeData* NodeHierarchy::TreeModel::mimeData
(const QModelIndexList& indexes) const
{
    QByteArray encoded_data;
    QDataStream stream(&encoded_data, QIODevice::WriteOnly);

    for (auto& index : indexes)
    {
        if (!index.isValid()) continue;
        auto node = getNode(index);
        if (nullptr == node) continue;
        stream << QString::fromLocal8Bit(node->getAbsolutePath().c_str());
    }

    auto mime_data = new QMimeData();
    mime_data->setData("application/vnd.text.list", encoded_data);
    return mime_data;
}


//-----------------------------------------------------------------------------
bool NodeHierarchy::TreeModel::dropMimeData
(const QMimeData* data, Qt::DropAction action,
 int row, int, const QModelIndex& parent)
{
    if (action != Qt::MoveAction) return false;
    if (!data->hasFormat("application/vnd.text.list")) return false;
    auto parent_node = this->getNode(parent);
    if (nullptr == parent_node) return false;
    
    if (row == -1) row = parent_node->getChildCount();
    
    QByteArray encoded_data = data->data("application/vnd.text.list");
    QDataStream stream(&encoded_data, QIODevice::ReadOnly);
    
    int rows = 0;
    while (!stream.atEnd())
    {
        QString text;
        stream >> text;
        
        auto drop_node = Kernel::instance()->lookup(text.toLocal8Bit().data());
        auto parent_index = drop_node->getParent()->getSelfIndex();
        auto source_parent = this->createIndex(parent_index, 0, drop_node->getParent());
        auto src_row = drop_node->getParent()->indexOf(drop_node);
        this->moveRow(source_parent, src_row, parent, row);
        
        ++rows;
    }
    
    return false;
}


//-----------------------------------------------------------------------------
Node* NodeHierarchy::TreeModel::getNode(const QModelIndex& index) const
{
    if (index.isValid())
    {
        auto node = static_cast<Node*>(index.internalPointer());
        if (nullptr != node) return node;
    }
    return this->rootNode;
}

}
