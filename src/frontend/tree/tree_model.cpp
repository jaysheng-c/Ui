/**
  ********************************************************************************
  * @file           : tree_mdoel.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#include "tree_model.h"

struct TreeModel::Node {
    ~Node() {qDebug() << data;}
    
    QVariant data;
    QVector<Node*> children;
    Node *parent = nullptr;
};

class TreeModel::FreeNodeChild {
public:
    void operator()(Node *root) const
    {
        if (root) {
            for (const auto &child : root->children) {
                free(child);
            }
            root->children.resize(0);
        }
    }
private:
    static void free(Node *node)
    {
        if (node) {
            for (const auto &child : node->children) {
                free(child);
            }
            delete node;
        }
    }
};


TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent), m_root(std::make_unique<Node>())
{
    m_root->data = "root";
}

TreeModel::~TreeModel()
{
    clearModel();
}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()) {
        return {};
    }
    const auto *childNode = static_cast<Node*>(child.internalPointer());
    const auto *parent = childNode->parent;
    if (parent == m_root.get()) {
        return {};
    }
    int row = 0;
    if (parent->parent) {
        row = parent->parent->children.indexOf(parent);
    }
    return createIndex(row, 0, parent);
}

QModelIndex TreeModel::index(const int row, const int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        const auto *parentNode = nodeFromIndex(parent);
        if (const auto childNode = parentNode->children.value(row, nullptr)) {
            return createIndex(row, column, childNode);
        }
    }
    return {};
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }
    const auto *parentNode = nodeFromIndex(parent);
    return parentNode->children.size();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant TreeModel::data(const QModelIndex &index, const int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (auto *node = static_cast<Node*>(index.internalPointer())) {
        if (role == Qt::DisplayRole) {
            return node->data;
        }
    }
    return {};
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(const int section, const Qt::Orientation orientation, const int role) const
{
    if (orientation == Qt::Horizontal || role == Qt::DisplayRole) {
        return "节点名称";
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, const int role)
{
    if (index.isValid()) {
        if (auto *node = static_cast<Node*>(index.internalPointer())) {
            if (role == Qt::DisplayRole || Qt::EditRole) {
                node->data = value;
                emit dataChanged(index, index, {role});
                return true;
            }
        }
    }
    return QAbstractItemModel::setData(index, value, role);
}

bool TreeModel::insertRows(const int position, const int count, const QModelIndex &parent)
{
    auto *parentNode = nodeFromIndex(parent);

    beginInsertRows(parent, position, position + count - 1);
    for (int i = 0; i < count; ++i) {
        auto newNode = new Node() ;
        newNode->parent = parentNode;
        parentNode->children.insert(position + i, std::move(newNode));
    }
    endInsertRows();
    return true;
}

bool TreeModel::removeRows(const int position, const int count, const QModelIndex &parent)
{
    auto *parentNode = nodeFromIndex(parent);

    beginRemoveRows(parent, position, position + count - 1);
    for (int i = 0; i < count; ++i) {
        auto node = parentNode->children.takeAt(position);
    }
    endRemoveRows();

    return true;
}

bool TreeModel::insertColumns(const int position, const int columns, const QModelIndex &parent)
{
    Q_UNUSED(position)
    Q_UNUSED(columns)
    Q_UNUSED(parent)
    qInfo() << "can not support insertColumns";
    return false;
}

bool TreeModel::removeColumns(const int position, const int columns, const QModelIndex &parent)
{
    Q_UNUSED(position)
    Q_UNUSED(columns)
    Q_UNUSED(parent)
    qInfo() << "can not support removeColumns";
    return false;
}

void TreeModel::resetData(const QString &data)
{
    beginResetModel();
    FreeNodeChild()(m_root.get());
    const auto node = new Node{"23", {}, m_root.get()};
    node->children.append(new Node{"122", {}, node});
    m_root->children.append(node);
    m_root->children.append(new Node{"12", {}, m_root.get()});
    m_root->children.append(new Node{"3", {}, m_root.get()});

    endResetModel();
}

void TreeModel::clearModel()
{
    beginResetModel();
    FreeNodeChild()(m_root.get());
    endResetModel();
}

TreeModel::Node *TreeModel::nodeFromIndex(const QModelIndex &index) const
{
    if (!index.isValid())
        return m_root.get();
    return static_cast<Node*>(index.internalPointer());
}
