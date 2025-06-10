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
#include "data/discrete_matrix.h"
#include "data/matrix.h"
#include "data/table_data.h"
#include "frontend/global/macro.h"

class QItemSelection;

class UI_EXPORT TableModel : public QAbstractTableModel {
Q_OBJECT
public:

#ifdef DISCRETE_MATRIX_H
#define DISCRETE_MATRIX_MODEL
    using Data = DiscreteMatrix<TableData>;
#else
    using Data = Matrix<TableData>;
#endif

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
    // serialize
    NODISCARD QByteArray serializeData() const;
    void deserialize(const QByteArray &data);

    // read
    NODISCARD QVariant data(const QItemSelection &selection, int role = Qt::DisplayRole) const;
    // set
    NODISCARD bool setDataWithoutCommit(const QModelIndex &index, const QVariant &value, int role) const;

    void resetData(Data &&data);
    void resetData(const Data &data);
    void resetData();
private:
    std::unique_ptr<Data> m_data;
};


#endif //TABLE_MODEL_H
