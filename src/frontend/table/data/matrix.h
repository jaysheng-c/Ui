/**
  ********************************************************************************
  * @file           : matrix.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/16
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MATRIX_H
#define MATRIX_H

#include <QVector>
#include "frontend/global/macro.h"

#define MIN_ROWS 100
#define MIN_COLUMNS 20

enum class ExpandType {Column, Row};

template<class T>
class Matrix {
public:
    using reference = T &;
    using const_reference = const T &;
public:

    Matrix();
    Matrix(int rows, int columns, ExpandType type = ExpandType::Row);
    Matrix(const Matrix &other);
    Matrix(const Matrix &&other) noexcept;
    Matrix &operator=(const Matrix &other);
    Matrix &operator=(Matrix &&other) noexcept;
    ~Matrix() = default;

    // get
    NODISCARD ExpandType type() const { return m_type; }
    NODISCARD int columns() const { return m_columns; }
    NODISCARD int rows() const { return m_rows; }
    NODISCARD qsizetype count() { return m_columns * m_rows; }
    NODISCARD const QVector<T> &data() const { return m_data; }
    NODISCARD const_reference at(int row, int column) const noexcept;
    NODISCARD reference ref(int row, int column);

    // set
    bool setColumns(int count);
    bool setRows(int rows);
    bool setData(int row, int column, T &&data);
    bool setData(int row, int column, const T &data);

    MAYBE_UNUSED inline bool insertColumn(int column) { return insertColumns(column, 1); }
    MAYBE_UNUSED inline bool insertRow(int row) { return insertRows(row, 1); }
    MAYBE_UNUSED inline bool removeColumn(int column) { return removeColumns(column, 1); }
    MAYBE_UNUSED inline bool removeRow(int row) { return removeRows(row, 1); }

    MAYBE_UNUSED bool insertColumns(int column, int count, const T &data = T())
    {
        if (column < 0 || column > m_columns || count <= 0) {
            qWarning() << "column out of range or count small than 0";
            return false;
        }
        switch (m_type) {
            case ExpandType::Column: {
                auto nextColumns = m_columns + count;
                // 获得每一行的开始索引
                QVector<qsizetype> indexes(m_rows, -1);
                for (int i {0}; i < m_rows; ++i) {
                    indexes[i] = i * m_columns;
                }
                // 将数据移动新的容器中
                QVector<T> tempData(m_rows * nextColumns, data);
                auto it = tempData.begin();
                if (column == 0) {
                    for (auto i : indexes) {
                        it += count;
                        it = std::move(m_data.begin() + i, m_data.begin() + i + m_columns, it);
                    }
                } else if (column == m_columns - 1) {
                    for (auto i : indexes) {
                        it = std::move(m_data.begin() + i, m_data.begin() + i + m_columns, it);
                        it += count;
                    }
                } else {
                    for (auto i : indexes) {
                        it = std::move(m_data.begin() + i, m_data.begin() + i + column, it);
                        it += count;
                        it = std::move(m_data.begin() + i + column, m_data.begin() + i + m_columns, it);
                    }
                }
                // 重新将数据移动回原容器中
                m_data = std::move(tempData);
                // 更新列数
                m_columns = nextColumns;
                break;
            }
            case ExpandType::Row: {
                auto i = column * m_rows;
                m_data.insert(i, m_rows * count, T());
                m_columns += count;
                break;
            }
        }
        return true;
    }
    MAYBE_UNUSED bool insertRows(int row, int count, const T &data = T())
    {
        if (row < 0 || row > m_rows || count <= 0) {
            qWarning() << "row out of range or count small than 0";
            return false;
        }
        switch (m_type) {
            case ExpandType::Column: {
                auto i = row * m_columns;
                m_data.insert(i, m_columns * count, T());
                m_rows += count;
                break;
            }
            case ExpandType::Row: {
                auto nextRows = m_rows + count;
                // 获得每一列的开始索引
                QVector<qsizetype> indexes(m_columns, -1);
                for (int i {0}; i < m_columns; ++i) {
                    indexes[i] = i * m_rows;
                }
                // 将数据移动新的容器中
                QVector<T> tempData(m_columns * nextRows, data);
                auto it = tempData.begin();
                if (row == 0) {
                    for (auto i : indexes) {
                        it += count;
                        it = std::move(m_data.begin() + i, m_data.begin() + i + m_rows, it);
                    }
                } else if (row == m_rows - 1) {
                    for (auto i : indexes) {
                        it = std::move(m_data.begin() + i, m_data.begin() + i + m_rows, it);
                        it += count;
                    }
                } else {
                    for (auto i : indexes) {
                        it = std::move(m_data.begin() + i, m_data.begin() + i + row, it);
                        it += count;
                        it = std::move(m_data.begin() + i + row, m_data.begin() + i + m_rows, it);
                    }
                }
                // 重新将数据移动回原容器中
                m_data = std::move(tempData);
                // 更新行数
                m_rows = nextRows;
                break;
            }
        }
        return true;
    }
    MAYBE_UNUSED bool removeColumns(int column, int count)
    {
        if (column < 0 || column >= m_columns || count <= 0) {
            qWarning() << "column out of range or count small than 0";
            return false;
        }
        count = m_columns - count < 0 ? m_columns : count;
        switch (m_type) {
            case ExpandType::Column: {
                auto nextColumns = m_columns - count;
                // 获得删除的列的索引范围
                QVector<QPair<qsizetype, qsizetype> > indexes(m_rows, {-1, -1});
                for (int i {0}; i < m_rows; ++i) {
                    auto beginPos = column + i * m_columns;
                    indexes[i] = {beginPos, beginPos + count - 1};
                }
                // 获得待移动索引范围
                QVector<QPair<qsizetype, qsizetype>> moveRanges(indexes.size() + 1, {-1, -1});
                qsizetype i = 0;
                for (auto j {0}; j < indexes.size(); ++j) {
                    moveRanges[j] = {i, indexes.at(j).first};
                    i = indexes.at(j).second + 1;
                }
                if (i != 0) {
                    moveRanges[indexes.size()] = {i, m_data.size()};
                }
                (void) moveRanges.removeIf([](const QPair<qsizetype, qsizetype> &pair) {
                    return pair.first >= pair.second;
                });
                // 将数据移动新的容器中
                QVector<T> tempData(m_rows * nextColumns);
                auto it = tempData.begin();
                for (const auto &range : moveRanges) {
                    it = std::move(m_data.begin() + range.first, m_data.begin() + range.second, it);
                }
                // 重新将数据移动回原容器中
                m_data = std::move(tempData);
                // 更新列数
                m_columns = nextColumns;
                break;
            }
            case ExpandType::Row: {
                qsizetype i = column * m_rows;
                m_data.remove(i, m_rows * count);
                m_columns -= count;
                break;
            }
            default:
                qWarning() << "unknown expand type";
                return false;
        }
        // 调整行列数
        adjustRowColumn();
        return true;
    }
    MAYBE_UNUSED bool removeRows(int row, int count)
    {
        if (row < 0 || row >= m_rows || count <= 0) {
            qWarning() << "row out of range or count small than 0";
            return false;
        }

        count = m_rows - count < 0 ? m_rows : count;
        switch (m_type) {
            case ExpandType::Column: {
                auto i = row * m_columns;
                m_data.remove(i, m_columns * count);
                m_rows -= count;
                break;
            }
            case ExpandType::Row: {
                auto nextRows = m_rows - count;
                // 获得待删除行的索引范围
                QVector<QPair<qsizetype, qsizetype> > indexes(m_columns, {-1, -1});
                for (int i {0}; i < m_columns; ++i) {
                    auto beginPos = row + i * m_rows;
                    indexes[i] = {beginPos, beginPos + count - 1};
                }
                // 获得待移动索引范围
                QVector<QPair<qsizetype, qsizetype>> moveRanges(indexes.size() + 1, {-1, -1});
                qsizetype i = 0;
                for (auto j {0}; j < indexes.size(); ++j) {
                    moveRanges[j] = {i, indexes.at(j).first};
                    i = indexes.at(j).second + 1;
                }
                if (i != 0) {
                    moveRanges[indexes.size()] = {i, m_data.size()};
                }
                (void) moveRanges.removeIf([](const QPair<qsizetype, qsizetype> &pair) {
                    return pair.first >= pair.second;
                });
                // 将数据移动新的容器中
                QVector<T> tempData(nextRows * m_columns);
                auto it = tempData.begin();
                for (const auto &range : moveRanges) {
                    it = std::move(m_data.begin() + range.first, m_data.begin() + range.second, it);
                }
                // 重新将数据移动回原容器中
                m_data = std::move(tempData);
                // 更新行数
                m_rows = nextRows;
                break;
            }
        }
        // 调整行列数
        adjustRowColumn();
        return true;
    }


protected:
    NODISCARD qsizetype realIndex(int row, int column) const
    {
        switch (m_type) {
            case ExpandType::Column:    return row * m_columns + column;
            case ExpandType::Row:       return column * m_rows + row;
        }
        return -1;
    }
    void adjustRowColumn()
    {
        if (m_columns <= 0) {
            m_columns = MIN_COLUMNS;
        }
        if (m_rows <= 0) {
            m_rows = MIN_ROWS;
        }
        m_data.resize(m_columns * m_rows);
    }

    ExpandType m_type;
    int m_columns;
    int m_rows;
    QVector<T> m_data;
};

template<class T>
Matrix<T>::Matrix()
    : m_columns(MIN_COLUMNS), m_rows(MIN_ROWS), m_data(MIN_COLUMNS * MIN_ROWS), m_type(ExpandType::Row) {}

template<class T>
Matrix<T>::Matrix(int rows, int columns, ExpandType type)
    : m_rows(rows), m_columns(columns), m_type(type), m_data(columns * rows, T()) {}

template<class T>
Matrix<T>::Matrix(const Matrix &other)
{
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = other.m_data;
}

template<class T>
Matrix<T>::Matrix(const Matrix &&other) noexcept
{
    m_type = other.m_type;
    m_columns = other.m_columns;
    m_rows = other.m_rows;
    m_data = std::move(other.m_data);
}

template<class T>
Matrix<T> &Matrix<T>::operator=(const Matrix &other)
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
Matrix<T> &Matrix<T>::operator=(Matrix &&other) noexcept
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
typename Matrix<T>::const_reference Matrix<T>::at(int row, int column) const noexcept
{
    auto i = realIndex(row, column);
    Q_ASSERT_X(i >= 0 && i < m_data.size(), "QVector::at", "index out of range");
    return m_data.at(i);
}

template<class T>
typename Matrix<T>::reference Matrix<T>::ref(int row, int column)
{
    auto i = realIndex(row, column);
    Q_ASSERT_X(i >= 0 && i < m_data.size(), "QVector::at", "index out of range");
    return m_data[i];
}

template<class T>
bool Matrix<T>::setColumns(int count)
{
    if (count <= m_columns) {
        qWarning() << "count must be greater than current count";
        return false;
    }
    m_columns = count;
    m_data.resize(count * m_rows, T());
    return true;
}

template<class T>
bool Matrix<T>::setRows(int rows)
{
    if (rows <= m_rows) {
        qWarning() << "rows must be greater than current rows";
        return false;
    }
    m_rows = rows;
    m_data.resize(m_columns * rows, T());
    return true;
}

template<class T>
bool Matrix<T>::setData(int row, int column, T &&data)
{
    qsizetype i = realIndex(row, column);
    if (i < 0) {
        qWarning() << "index out of range";
        return false;
    }
    m_data[i] = std::move(data);
    return true;
}

template<class T>
bool Matrix<T>::setData(int row, int column, const T &data)
{
    qsizetype i = realIndex(row, column);
    if (i < 0) {
        qWarning() << "index out of range";
        return false;
    }
    m_data[i] = data;
    return true;
}

#endif //MATRIX_H
