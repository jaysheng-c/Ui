/**
  ********************************************************************************
  * @file           : table_model.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_model.h"

#include <QItemSelection>

#define DEFAULT_VALUE TableData()

namespace {

QHash<Qt::ItemDataRole, int> QRoleToDataType {
        {Qt::EditRole, TableData::Real},
        {Qt::DisplayRole, TableData::Display},
        {Qt::TextAlignmentRole, TableData::Align},
        {Qt::BackgroundRole, TableData::Background},
        {Qt::ForegroundRole, TableData::Foreground},
        {Qt::FontRole, TableData::Font},
};

QString alpha(int section) {
    QString colName;
    static QVector<QChar> alpha = {
            'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
            'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'
    };
    int num2 = section + 1;
    while (num2 > 0) {
        num2 = num2 - 1;
        colName = alpha[num2 % alpha.size()] + colName;
        num2 /= static_cast<int>(alpha.size());
    }
    return colName;
}

}

TableModel::TableModel(ExpandType type, QObject *parent)
    : QAbstractTableModel(parent), m_data(std::make_unique<Matrix<TableData>>(MIN_ROWS, MIN_COLUMNS, type))
{
    for (int column { 0 }; column < m_data->columns(); ++column) {
        const QString &a = alpha(column);
        for (int row { 0 }; row < m_data->rows(); ++row) {
            m_data->setData(row, column, TableData(a + QString::number(row + 1)));
        }
    }

}

int TableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data->rows();
}

int TableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data->columns();
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }
    if (role == Qt::UserRole) {
        return QVariant::fromValue(m_data->at(index.row(), index.column()));
    }
    const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1);
    if (dataType != -1) {
        return m_data->at(index.row(), index.column()).data(static_cast<TableData::Type>(dataType));
    }
    return {};
}

Qt::ItemFlags TableModel::flags(const QModelIndex &index) const
{
    if (index.isValid()) {
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return QAbstractTableModel::flags(index);
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return alpha(section);
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

bool TableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid()) {
        return false;
    }
    if (role == Qt::UserRole) {
        m_data->ref(index.row(), index.column()) = value.value<TableData>();
        emit dataChanged(index, index);
        return true;
    }
    const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1);
    if (dataType != -1) {
        m_data->ref(index.row(), index.column()).setData(static_cast<TableData::Type>(dataType), value);
        return true;
    }
    return QAbstractItemModel::setData(index, value, role);
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(parent, position, position + rows - 1);
    const bool flag = m_data->insertRows(position, rows, DEFAULT_VALUE);
    endInsertRows();
    return flag;
}

bool TableModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    beginRemoveRows(parent, position, position + rows - 1);
    const bool flag = m_data->removeRows(position, rows);
    endRemoveRows();
    if (flag) {
        beginResetModel();
        endResetModel();
    }
    return flag;
}

bool TableModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    beginInsertColumns(parent, position, position + columns - 1);
    const bool flag = m_data->insertColumns(position, columns, DEFAULT_VALUE);
    endInsertColumns();
    return flag;
}

bool TableModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    beginRemoveColumns(parent, position, position + columns - 1);
    const bool flag = m_data->removeColumns(position, columns);
    endRemoveColumns();
    if (flag) {
        beginResetModel();
        endResetModel();
    }
    return flag;
}

QVariant TableModel::data(const QItemSelection &selection, int role) const
{
    QVector<QVariant> d(selection.indexes().size());
    qsizetype i = 0;
    if (role == Qt::UserRole) {
        for (const auto &item: selection) {
            for (const auto &index: item.indexes()) {
                d[i++] = QVariant::fromValue(m_data->at(index.row(), index.column()));
            }
        }
    } else {
        auto dataType = QRoleToDataType.value(Qt::ItemDataRole(role), -1);
        for (const auto &item: selection) {
            for (const auto &index: item.indexes()) {
                d[i++] = m_data->at(index.row(), index.column()).data(TableData::Type(dataType));
            }
        }
    }
    return d;
}
