/**
  ********************************************************************************
  * @file           : table_model.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_MODEL_H
#define TABLE_MODEL_H

#include <QAbstractTableModel>
#include "global/macro.h"
#include "data/table_data.h"
#include "data/matrix.h"

class QItemSelection;

class TableModel : public QAbstractTableModel {
Q_OBJECT
public:
    explicit TableModel(ExpandType type, QObject *parent = nullptr);

public: // override
    // read
    NODISCARD int rowCount(const QModelIndex &parent) const override;
    NODISCARD int columnCount(const QModelIndex &parent) const override;
    NODISCARD QVariant data(const QModelIndex &index, int role) const override;
    NODISCARD Qt::ItemFlags flags(const QModelIndex &index) const override;
    NODISCARD QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    // set & delete
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    bool insertRows(int position, int rows, const QModelIndex &parent) override;
    bool removeRows(int position, int rows, const QModelIndex &parent) override;
    bool insertColumns(int position, int columns, const QModelIndex &parent) override;
    bool removeColumns(int position, int columns, const QModelIndex &parent) override;

public:
    // read
    NODISCARD QVariant data(const QItemSelection &selection, int role = Qt::DisplayRole) const;

private:
    std::unique_ptr<Matrix<TableData>> m_data;
};


#endif //TABLE_MODEL_H
