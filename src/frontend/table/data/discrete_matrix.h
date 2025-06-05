/**
  ********************************************************************************
  * @file           : discrete_matrix.h
  * @author         : jaysheng
  * @brief          : 离散的矩阵结构
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef DISCRETE_MATRIX_H
#define DISCRETE_MATRIX_H

#include <QMap>
#include <QVector>
#include "frontend/global/macro.h"

template<class T>
class DiscreteMatrix final {
public:
    using reference = T &;
    using const_reference = const T &;
    using value_type = T;
    using const_value = const T;
public:
    DiscreteMatrix();
    DiscreteMatrix(int rows, int columns, ExpandType type = ExpandType::Row);
    DiscreteMatrix(const DiscreteMatrix &other);
    DiscreteMatrix(DiscreteMatrix &&other) noexcept;
    DiscreteMatrix &operator=(const DiscreteMatrix &other);
    DiscreteMatrix &operator=(DiscreteMatrix &&other) noexcept;
    ~DiscreteMatrix() = default;

    // get
    NODISCARD ExpandType type() const { return m_type; }
    NODISCARD int columns() const { return m_columns; }
    NODISCARD int rows() const { return m_rows; }
    NODISCARD qsizetype count() const { return m_columns * m_rows; }
    NODISCARD const QVector<T> &data() const { return m_data; }
    NODISCARD T at(int row, int column) const noexcept;
    NODISCARD reference ref(int row, int column);

    // set
    bool setColumns(int count);
    bool setRows(int rows);
    bool setData(int row, int column, T &&data);
    bool setData(int row, int column, const T &data);

    MAYBE_UNUSED inline bool insertColumn(const int column) { return insertColumns(column, 1); }
    MAYBE_UNUSED inline bool insertRow(const int row) { return insertRows(row, 1); }
    MAYBE_UNUSED inline bool removeColumn(const int column) { return removeColumns(column, 1); }
    MAYBE_UNUSED inline bool removeRow(const int row) { return removeRows(row, 1); }

    MAYBE_UNUSED bool insertColumns(int column, int count, const T &data = T());
    MAYBE_UNUSED bool insertRows(int row, int count, const T &data = T());
    MAYBE_UNUSED bool removeColumns(const int column, int count);
    MAYBE_UNUSED bool removeRows(const int row, int count);

    friend QDataStream &operator<<(QDataStream &out, const DiscreteMatrix &matrix)
    {
//        out << matrix.m_type << matrix.m_columns << matrix.m_rows << matrix.m_data;
        return out;
    }

    friend QDataStream &operator>>(QDataStream &in, DiscreteMatrix &matrix)
    {
//        in >> matrix.m_type >> matrix.m_columns >> matrix.m_rows >> matrix.m_data;
        return in;
    }

private:
    ExpandType m_type;
    int m_columns;
    int m_rows;
    QMap<int, QVector<T>> m_data;   // ExpandType::Row key=column, ExpandType::Column key=row
};

template<class T>
DiscreteMatrix<T>::DiscreteMatrix()
    : m_type(ExpandType::Row), m_columns(MIN_COLUMNS), m_rows(MIN_ROWS) {}

template<class T>
DiscreteMatrix<T>::DiscreteMatrix(int rows, int columns, ExpandType type)
    : m_type(type), m_columns(columns), m_rows(rows) {}

template<class T>
DiscreteMatrix<T>::DiscreteMatrix(const DiscreteMatrix &other)
{
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = other.m_data;
}

template<class T>
DiscreteMatrix<T>::DiscreteMatrix(DiscreteMatrix &&other) noexcept
{
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = std::move(other.m_data);
}

template<class T>
DiscreteMatrix<T> &DiscreteMatrix<T>::operator=(const DiscreteMatrix &other)
{
    if (this == &other) {
        return *this;
    }
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = other.m_data;
    return *this;
}

template<class T>
DiscreteMatrix<T> &DiscreteMatrix<T>::operator=(DiscreteMatrix &&other) noexcept
{
    if (this == &other) {
        return *this;
    }
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = std::move(other.m_data);
    return *this;
}

template<class T>
T DiscreteMatrix<T>::at(int row, int column) const noexcept
{
    if (m_type == ExpandType::Row) {
        if (m_data.contains(column) && row < m_data[column].size()) {
            return m_data[column][row];
        }
    } else if (m_type == ExpandType::Column) {
        if (m_data.contains(row) && column < m_data[row].size()) {
            return m_data[row][column];
        }
    }
    return {};
}

template<class T>
typename DiscreteMatrix<T>::reference DiscreteMatrix<T>::ref(int row, int column)
{
    if (m_type == ExpandType::Row) {
        if (!m_data.contains(column)) {
            m_data.insert(column, QVector<T>(row + 1));
            return m_data[column][row];
        } else if (row >= m_data[column].size()) {
            m_data[column].resize(row + 1);
            return m_data[column][row];
        }
        return m_data[column][row];
    }

    if (!m_data.contains(row)) {
        m_data.insert(row, QVector<T>(column + 1));
        return m_data[row][column];
    } else if (column >= m_data[row].size()) {
        m_data[row].resize(column + 1);
        return m_data[row][column];
    }
    return m_data[row][column];
}

template<class T>
bool DiscreteMatrix<T>::setColumns(const int count)
{
    if (count <= 0) {
        qWarning() << "count must be greater than zero";
        return false;
    }
    m_columns = count;
    if (m_type == ExpandType::Row) {
        const auto &keys = m_data.keys();
        for (auto key : keys) {
            if (key >= m_columns) {
                m_data.remove(key);
            }
        }
    } else {
        for (auto &d : m_data) {
            if (d.size() > m_columns) {
                d.resize(m_columns);
            }
        }
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::setRows(const int rows)
{
    if (rows <= 0) {
        qWarning() << "rows must be greater than zero";
        return false;
    }
    m_rows = rows;
    if (m_type == ExpandType::Row) {
        for (auto &d : m_data) {
            if (d.size() > m_rows) {
                d.resize(m_rows);
            }
        }
    } else {
        const auto &keys = m_data.keys();
        for (auto key : keys) {
            if (key >= m_rows) {
                m_data.remove(key);
            }
        }
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::setData(int row, int column, T &&data)
{
    if (row < 0 || column < 0 || row >= m_rows || column >= m_columns) {
        qWarning() << "row or column must be greater than zero and smaller than maxRange";
        return false;
    }
    if (m_type == ExpandType::Row) {
        auto &d = m_data[column];
        if (row >= d.size()) {
            d.resize(row + 1);
        }
        d[row] = std::move(data);
    } else {
        auto &d = m_data[row];
        if (column >= d.size()) {
            d.resize(column + 1);
        }
        d[column] = std::move(data);
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::setData(int row, int column, const T &data)
{
    if (row < 0 || column < 0 || row >= m_rows || column >= m_columns) {
        qWarning() << "row or column must be greater than zero and smaller than maxRange";
        return false;
    }
    if (m_type == ExpandType::Row) {
        auto &d = m_data[column];
        if (row >= d.size()) {
            d.resize(row + 1);
        }
        d[row] = data;
    } else {
        auto &d = m_data[row];
        if (column >= d.size()) {
            d.resize(column + 1);
        }
        d[column] = data;
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::insertColumns(int column, int count, const T &data)
{
    if (column < 0 || column > m_columns || count <= 0) {
        qWarning() << "column out of range or count small than 0";
        return false;
    }
    switch (m_type) {
        case ExpandType::Column: {
            // 插入的是map的value
            if (data == T()) {
                // 数据为空，仅对已有数据的行进行插入即可
                for (auto &d : m_data) {
                    if (column < d.size()) {
                        d.insert(column, count, data);
                    }
                }
            } else {
                // 行数特别多最坏的时间复杂度为m_rows * O(count + m_columns)
                for (int row = 0; row < m_rows; ++row) {
                    if (column < m_data[row].size()) {
                        if (m_data[row].size() < column) {
                            // 已有的数据索引小于column，需要resize
                            m_data[row].resize(column);
                        }
                        m_data[row].insert(column, count, data);
                    }
                }
            }
            break;
        }
        case ExpandType::Row: {
            // 插入的是map的key
            auto columns = m_data.keys();
            // 从大到小排序
            std::sort(columns.begin(), columns.end(), std::greater());
            for (auto col : columns) {
                if (col < column) break;
                if (col >= column) {
                    // 搬移数据
                    m_data[col + count] = std::move(m_data[col]);
                }
            }
            if (data == T()) {
                // 数据为空，直接清除对应列的数据，减少空间开销
                (void) m_data.removeIf([column, count](const int &key, const QVector<T> &value) {
                    Q_UNUSED(value)
                    return key >= column && key < column + count;
                });
            } else {
                for (int i = 0; i < count; ++i) {
                    m_data[column + i].resize(m_rows, data);
                }
            }
            break;
        }
        default:
            return false;
    }
    m_columns += count;
    return true;
}

template<class T>
bool DiscreteMatrix<T>::insertRows(int row, int count, const T &data)
{
    if (row < 0 || row > m_rows || count <= 0) {
        qWarning() << "row out of range or count small than 0";
        return false;
    }
    switch (m_type) {
        case ExpandType::Column:
            // 插入的是map的key
            if (data == T()) {
                break;
            }
        case ExpandType::Row:
            // 插入的是map的value
        default:
            return false;
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::removeColumns(const int column, int count)
{
    if (column < 0 || column >= m_columns || count <= 0) {
        qWarning() << "column out of range or count small than 0";
        return false;
    }
    switch (m_type) {
        case ExpandType::Column:
            break;
        case ExpandType::Row:
            break;
        default:
            return false;
    }
    return true;
}

template<class T>
bool DiscreteMatrix<T>::removeRows(const int row, int count)
{
    if (row < 0 || row >= m_rows || count <= 0) {
        qWarning() << "row out of range or count small than 0";
        return false;
    }
    switch (m_type) {
        case ExpandType::Column:
            break;
        case ExpandType::Row:
            break;
        default:
            return false;
    }
    return true;
}

#endif //DISCRETE_MATRIX_H
