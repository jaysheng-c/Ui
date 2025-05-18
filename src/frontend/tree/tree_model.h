/**
  ********************************************************************************
  * @file           : tree_mdoel.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TREE_MODEL_H
#define TREE_MODEL_H

#include <QAbstractListModel>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include "frontend/global/global.h"

class UI_EXPORT TreeModel : public QAbstractItemModel {
Q_OBJECT
public:
    explicit TreeModel(QObject *parent = nullptr);
    ~TreeModel() override;

public: // override
    NODISCARD QModelIndex parent(const QModelIndex &child) const override;
    NODISCARD QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    NODISCARD int rowCount(const QModelIndex &parent) const override;
    NODISCARD int columnCount(const QModelIndex &parent) const override;
    NODISCARD QVariant data(const QModelIndex &index, int role) const override;
    NODISCARD Qt::ItemFlags flags(const QModelIndex &index) const override;
    NODISCARD QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // set & delete
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int position, int count, const QModelIndex &parent) override;
    bool removeRows(int position, int count, const QModelIndex &parent) override;
    bool insertColumns(int position, int columns, const QModelIndex &parent) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent) override;

public:
    void resetData(const QString &data);
    void clearModel();

private:
    class FreeNodeChild;
    struct Node;
    NODISCARD Node *nodeFromIndex(const QModelIndex &index) const;

    std::shared_ptr<Node> m_root;
};



#endif //TREE_MODEL_H
