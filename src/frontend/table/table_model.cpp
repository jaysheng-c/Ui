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

#include <QtZlib/zlib.h>
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

QString alpha(const int section) {
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

QByteArray compressData(const QByteArray &data)
{
    QByteArray compressed;
    constexpr int BUFF_SIZE = 128 * 1024;
    char buf[BUFF_SIZE];

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    deflateInit(&strm, Z_DEFAULT_COMPRESSION);

    strm.avail_in = data.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data.data()));

    do {
        strm.avail_out = BUFF_SIZE;
        strm.next_out = reinterpret_cast<Bytef*>(buf);
        deflate(&strm, Z_FINISH);
        compressed.append(buf, BUFF_SIZE - strm.avail_out);
    } while (strm.avail_out == 0);

    deflateEnd(&strm);
    return compressed;
}

QByteArray decompressData(const QByteArray &compressed)
{
    QByteArray decompressed;
    constexpr int BUFF_SIZE = 128 * 1024;
    char buf[BUFF_SIZE];

    z_stream strm;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    inflateInit(&strm);

    strm.avail_in = compressed.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressed.data()));

    int ret;
    do {
        strm.avail_out = BUFF_SIZE;
        strm.next_out = reinterpret_cast<Bytef*>(buf);
        ret = inflate(&strm, Z_NO_FLUSH);
        decompressed.append(buf, BUFF_SIZE - strm.avail_out);
    } while (ret == Z_OK);

    inflateEnd(&strm);
    return decompressed;
}

}

TableModel::TableModel(ExpandType type, QObject *parent)
    : QAbstractTableModel(parent)
{
#ifdef DISCRETE_MATRIX_MODEL
    m_data = std::make_unique<DiscreteMatrix<TableData>>(MIN_ROWS, MIN_COLUMNS);
#else
    m_data = std::make_unique<Matrix<TableData>>(MIN_ROWS, MIN_COLUMNS, type);
#endif
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
#ifdef DISCRETE_MATRIX_MODEL
    bool flag = true;
    if (role == Qt::UserRole) {
        flag &= m_data->setData(index.row(), index.column(), value.value<TableData>());
    } else {
        if (const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1); dataType != -1) {
            auto data = m_data->at(index.row(), index.column());
            flag &= data.setData(static_cast<TableData::Type>(dataType), value);
            flag &= m_data->setData(index.row(), index.column(), data);
        }
    }
    if (flag) {
        emit dataChanged(index, index);
        return flag;
    }
#else

    if (role == Qt::UserRole) {
        m_data->ref(index.row(), index.column()) = value.value<TableData>();
        emit dataChanged(index, index);
        return true;
    }
    const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1);
    if (dataType != -1) {
        m_data->ref(index.row(), index.column()).setData(static_cast<TableData::Type>(dataType), value);
        emit dataChanged(index, index);
        return true;
    }
#endif

    return QAbstractItemModel::setData(index, value, role);
}

bool TableModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    beginInsertRows(parent, position, position + rows - 1);
    const bool flag = m_data->insertRows(position, rows);
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
    const bool flag = m_data->insertColumns(position, columns);
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

QByteArray TableModel::serializeData() const
{
    QByteArray data;
    QDataStream out(&data, QDataStream::WriteOnly);
    // out << *m_data;
    // 压缩
    return compressData(data);
}

void TableModel::deserialize(const QByteArray &data)
{
    beginResetModel();
    QDataStream in(decompressData(data));
    // in >> *m_data;
    endResetModel();
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
        auto dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1);
        for (const auto &item: selection) {
            for (const auto &index: item.indexes()) {
                d[i++] = m_data->at(index.row(), index.column()).data(static_cast<TableData::Type>(dataType));
            }
        }
    }
    return d;
}

bool TableModel::setDataWithoutCommit(const QModelIndex &index, const QVariant &value, int role) const
{
    if (!index.isValid()) {
        return false;
    }
#ifdef DISCRETE_MATRIX_MODEL
    if (role == Qt::UserRole) {
        return m_data->setData(index.row(), index.column(), value.value<TableData>());
    }
    if (const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1); dataType != -1) {
        bool flag = true;
        auto data = m_data->at(index.row(), index.column());
        flag &= data.setData(static_cast<TableData::Type>(dataType), value);
        flag &= m_data->setData(index.row(), index.column(), data);
        return flag;
    }
#else
    if (role == Qt::UserRole) {
        m_data->ref(index.row(), index.column()) = value.value<TableData>();
        return true;
    }
    const int dataType = QRoleToDataType.value(static_cast<Qt::ItemDataRole>(role), -1);
    if (dataType != -1) {
        m_data->ref(index.row(), index.column()).setData(static_cast<TableData::Type>(dataType), value);
        return true;
    }
#endif
    return false;
}

void TableModel::resetData(Data &&data)
{
    beginResetModel();
    m_data = std::make_unique<Data>(data);
    endResetModel();
}

void TableModel::resetData(const Data &data)
{
    beginResetModel();
    m_data = std::make_unique<Data>(data);
    endResetModel();
}

void TableModel::resetData()
{
    beginResetModel();
#ifdef DISCRETE_MATRIX_MODEL
    m_data = std::make_unique<Data>(MIN_ROWS, MIN_COLUMNS);
#else
    m_data = std::make_unique<Data>(MIN_ROWS, MIN_COLUMNS, m_data->type());
#endif
    endResetModel();
}
