/**
  ********************************************************************************
  * @file           : discrete_matrix.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/06/08
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef DISCRETE_MATRIX_H
#define DISCRETE_MATRIX_H

#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>
#include <QThreadPool>
#include "frontend/global/macro.h"

namespace DiscreteConstexpr {

constexpr int g_shardCount{16};
constexpr int g_colCacheIndex{0};
constexpr int g_rowCacheIndex{1};

}

template<class T>
class DiscreteMatrix {
public:
    DiscreteMatrix() : DiscreteMatrix(MIN_ROWS, MIN_COLUMNS) {}
    DiscreteMatrix(const int rows, const int columns)
        : m_rows(rows), m_columns(columns), m_rowIndexes(IndexInit()(rows)), m_columnIndexes(IndexInit()(columns))
    {
        const int count = qCeil(static_cast<double>(m_rows * m_columns) / DiscreteConstexpr::g_shardCount);
        for (auto &[data, lock] : m_data) {
            data.reserve(count);
        }
    }
    DiscreteMatrix(const DiscreteMatrix &other)
    {
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_rowIndexes = other.m_rowIndexes;
        m_columnIndexes = other.m_columnIndexes;
        m_invalidCache = other.m_invalidCache;
        for (int i{0}; i < other.m_data.size(); ++i) {
            auto &[d, l] = m_data[i];
            QWriteLocker locker(&l);
            d = other.m_data[i].data;
        }
    }
    DiscreteMatrix(DiscreteMatrix &&other) noexcept
    {
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_rowIndexes = std::move(other.m_rowIndexes);
        m_columnIndexes = std::move(other.m_columnIndexes);
        m_invalidCache = std::move(other.m_invalidCache);
        m_data = std::move(other.m_data);
        for (int i{0}; i < other.m_data.size(); ++i) {
            auto &[d, l] = m_data[i];
            QWriteLocker locker(&l);
            d = std::move(other.m_data[i].data);
        }
    }
    DiscreteMatrix &operator=(const DiscreteMatrix &other)
    {
        if (this == &other) {
            return *this;
        }
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_rowIndexes = other.m_rowIndexes;
        m_columnIndexes = other.m_columnIndexes;
        m_invalidCache = other.m_invalidCache;
        for (int i{0}; i < other.m_data.size(); ++i) {
            auto &[d, l] = m_data[i];
            QWriteLocker locker(&l);
            d = other.m_data[i].data;
        }
        return *this;
    }
    DiscreteMatrix &operator=(DiscreteMatrix &&other) noexcept
    {
        if (this == &other) {
            return *this;
        }
        m_rows = other.m_rows;
        m_columns = other.m_columns;
        m_rowIndexes = std::move(other.m_rowIndexes);
        m_columnIndexes = std::move(other.m_columnIndexes);
        m_invalidCache = std::move(other.m_invalidCache);
        m_data = std::move(other.m_data);
        for (int i{0}; i < other.m_data.size(); ++i) {
            auto &[d, l] = m_data[i];
            QWriteLocker locker(&l);
            d = std::move(other.m_data[i].data);
        }
        return *this;
    }
    ~DiscreteMatrix() = default;

    // get
    NODISCARD int rows() const { return m_rows; }
    NODISCARD int columns() const { return m_columns; }
    NODISCARD qsizetype count() const { return m_rows * m_columns; }
    NODISCARD T at(const int row, const int column) const noexcept
    {
        QReadLocker locker(&m_lock);
        const qsizetype i = this->index(row, column);
        const auto &[map, l] = m_data.at(shard(i));
        QReadLocker dLocker(&l);
        return map.value(i, T());
    }
    NODISCARD qsizetype dataShard(const int row, const int column) const noexcept
    {
        const auto i = this->index(row, column);
        if (i == -1) {
            return -1;
        }
        return shard(i);
    }

    // set
    MAYBE_UNUSED bool setColumns(const int columns)
    {
        int count = 0;
        {
            QReadLocker locker(&m_lock);
            count = m_columns - columns;
        }

        if (count == 0) {
            return false;
        }
        if (count > 0) {
            return removeColumns(columns, count);
        }
        QWriteLocker locker(&m_lock);
        auto &cache = m_invalidCache[DiscreteConstexpr::g_colCacheIndex];
        const int oldCount = static_cast<int>(m_columnIndexes.size());
        m_columnIndexes.resize(columns, -1);
        for (int idx{oldCount}; idx < columns; ++idx) {
            if (!cache.isEmpty()) {
                // 存在缓存
                m_columnIndexes[idx] = cache.dequeue();
                continue;
            }
            m_columnIndexes[idx] = idx;
        }
        m_columns = columns;
        return true;
    }
    MAYBE_UNUSED bool setRows(const int rows)
    {
        int count = 0;
        {
            QReadLocker locker(&m_lock);
            count = m_rows - rows;
        }
        if (count == 0) {
            return false;
        }
        if (count > 0) {
            return removeRows(rows, count);
        }
        QWriteLocker locker(&m_lock);
        auto &cache = m_invalidCache[DiscreteConstexpr::g_rowCacheIndex];
        const int oldCount = static_cast<int>(m_rowIndexes.size());
        m_rowIndexes.resize(rows, -1);
        for (int idx{oldCount}; idx < rows; ++idx) {
            if (!cache.isEmpty()) {
                // 存在缓存
                m_rowIndexes[idx] = cache.dequeue();
                continue;
            }
            m_rowIndexes[idx] = idx;
        }
        m_rows = rows;
        return true;
    }
    MAYBE_UNUSED bool setData(const int row, const int column, T &&data)
    {
        QReadLocker locker(&m_lock);
        const qsizetype realIndex = this->index(row, column);
        if (realIndex == -1) {
            return false;
        }
        auto &[map, lock] = m_data[shard(realIndex)];
        QWriteLocker dataLocker(&lock);
        if (data == T()) {
            return map.remove(realIndex);
        }
        map[realIndex] = data;
        return true;
    }
    MAYBE_UNUSED bool setData(const int row, const int column, const T &data)
    {
        QReadLocker locker(&m_lock);
        const qsizetype realIndex = this->index(row, column);
        if (realIndex == -1) {
            return false;
        }
        auto &[map, lock] = m_data[shard(realIndex)];
        QWriteLocker dataLocker(&lock);
        if (data == T()) {
            return map.remove(realIndex);
        }
        map[realIndex] = std::move(data);
        return true;
    }

    MAYBE_UNUSED bool insertColumns(const int column, const int count)
    {
        QWriteLocker locker(&m_lock);
        if (column > m_columns || column < 0 || count < 1) {
            qWarning() << "column out of range";
            return false;
        }
        auto &cache = m_invalidCache[DiscreteConstexpr::g_colCacheIndex];
        qsizetype old = m_columns;
        m_columnIndexes.insert(column, count, -1);
        for (int idx{0}; idx < count; ++idx) {
            if (!cache.isEmpty()) {
                // 存在缓存
                m_columnIndexes[column + idx] = cache.dequeue();
                continue;
            }
            while (m_columnIndexes.contains(old)) {
                ++old;
            }
            m_columnIndexes[column + idx] = old;
        }
        m_columns += count;
        return true;
    }
    MAYBE_UNUSED bool insertRows(const int row, const int count)
    {
        QWriteLocker locker(&m_lock);
        if (row > m_rows || row < 0 || count < 1) {
            qWarning() << "row out of range";
            return false;
        }
        auto &cache = m_invalidCache[DiscreteConstexpr::g_rowCacheIndex];
        qsizetype old = m_rows;
        m_rowIndexes.insert(row, count, -1);
        for (int idx{0}; idx < count; ++idx) {
            if (!cache.isEmpty()) {
                // 存在缓存
                m_rowIndexes[row + idx] = cache.dequeue();
                continue;
            }
            while (m_rowIndexes.contains(old)) {
                ++old;
            }
            m_rowIndexes[row + idx] = old;
        }
        m_rows += count;
        return true;
    }
    MAYBE_UNUSED bool removeColumns(const int column, const int count)
    {
        QWriteLocker locker(&m_lock);
        if (column < 0 || column > m_columns || count < 1 || column + count > m_columns) {
            qWarning() << "column out of range";
            return false;
        }

        auto &cache = m_invalidCache[DiscreteConstexpr::g_colCacheIndex];
        // 删除数据及缓存索引
        QHash<qsizetype, QList<qsizetype>> removeIdxes;
        for (int idx{0}; idx < count; ++idx) {
            // 缓存索引
            cache.enqueue(m_columnIndexes[idx + column]);
            // 缓存删除数据索引
            for (int row{0}; row < m_rows; ++row) {
                const qsizetype i = this->index(row, idx + column);
                removeIdxes[shard(i)].append(i);
            }
        }
        // 多线程删除数据
        for (auto it = removeIdxes.begin(); it != removeIdxes.end(); ++it) {
            const qsizetype shard = it.key();
            const QList<qsizetype> &idxes = it.value();
            QThreadPool::globalInstance()->start([this, shard, idxes]() {
                auto &[data, lock] = m_data[shard];
                QWriteLocker dLocker(&lock);
                for (const qsizetype i : idxes) {
                    (void) data.remove(i);
                }
                data.squeeze();
            });
        }
        // 删除索引
        m_columnIndexes.remove(column, count);
        m_columns -= count;

        return true;
    }
    MAYBE_UNUSED bool removeRows(const int row, const int count)
    {
        QWriteLocker locker(&m_lock);
        if (row < 0 || row > m_rows || count < 1 || row + count > m_rows) {
            qWarning() << "row out of range";
            return false;
        }
        auto &cache = m_invalidCache[DiscreteConstexpr::g_rowCacheIndex];
        // 删除数据及缓存索引
        QHash<qsizetype, QList<qsizetype>> removeIdxes;
        for (int idx{0}; idx < count; ++idx) {
            // 缓存索引
            cache.enqueue(m_rowIndexes[row + idx]);
            // 缓存删除数据索引
            for (int col{0}; col < m_columns; ++col) {
                const qsizetype i = this->index(row + idx, col);
                removeIdxes[shard(i)].append(i);
            }
        }
        // 多线程删除数据
        for (auto it = removeIdxes.begin(); it != removeIdxes.end(); ++it) {
            const qsizetype shard = it.key();
            const QList<qsizetype> &idxes = it.value();
            QThreadPool::globalInstance()->start([this, shard, idxes]() {
                auto &[data, lock] = m_data[shard];
                QWriteLocker dLocker(&lock);
                for (const qsizetype i : idxes) {
                    (void) data.remove(i);
                }
                data.squeeze();
            });
        }
        // 删除索引
        m_rowIndexes.remove(row, count);
        m_rows -= count;
        return true;
    }

private:
    class IndexInit {
    public:
        QVector<qsizetype> operator()(const qsizetype count) const
        {
            QVector<qsizetype> res(count);
            for (int i{0}; i < count; ++i) {
                res[i] = i;
            }
            return res;
        }
    };

    struct Shard {
        QHash<qsizetype, T> data;
        mutable QReadWriteLock lock;
    };

    static qsizetype shard(const qsizetype index)
    {
        return static_cast<qsizetype>(qHash(index) % DiscreteConstexpr::g_shardCount);
    }
    NODISCARD qsizetype index(const qsizetype row, const qsizetype column) const noexcept
    {
        if (column >= m_columnIndexes.size()) {
            qWarning() << "column out of range";
            return -1;
        }
        if (row >= m_rowIndexes.size()) {
            qWarning() << "row out of range";
            return -1;
        }
        return m_rowIndexes.at(row) << 32 | m_columnIndexes.at(column);
    }

    int m_rows;
    int m_columns;
    QVector<qsizetype> m_rowIndexes;
    QVector<qsizetype> m_columnIndexes;
    std::array<QQueue<qsizetype>, 2> m_invalidCache;
    std::array<Shard, DiscreteConstexpr::g_shardCount> m_data;
    mutable QReadWriteLock m_lock;
};

#endif //DISCRETE_MATRIX_H
