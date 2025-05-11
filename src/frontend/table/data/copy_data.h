/**
  ********************************************************************************
  * @file           : copy_data.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef UI_COPY_DATA_H
#define UI_COPY_DATA_H

#include <QVariant>
#include <QPointer>

#include "global/macro.h"

namespace Table { enum class TypeFlag; }
class TableView;

class CopyData final {
public:
    enum Type { None, Cell, Column, Row };
    ~CopyData() = default;

    static CopyData &instance();

    template<class T>
    NODISCARD T data() const { return m_data.value<T>(); }

    template<class T>
    NODISCARD T range() const { return m_range.value<T>(); }

    NODISCARD const QPointer<TableView> &sourceTable() const { return m_table; }
    NODISCARD Table::TypeFlag opt() const { return m_opt; }
    NODISCARD Type copyType() const { return m_copyType; }

    void setData(Table::TypeFlag opt, Type copyTyp, const QVariant &data, const QVariant &range, TableView *table);

    void clear();

protected:
    CopyData();
    CopyData(const CopyData &) = default;
    CopyData(CopyData &&) noexcept = default;
    CopyData &operator=(const CopyData &) = default;
    CopyData &operator=(CopyData &&) noexcept = default;


    QVariant m_data;
    QVariant m_range;
    QPointer<TableView> m_table;
    Table::TypeFlag m_opt;
    Type m_copyType = None;
};


#endif //UI_COPY_DATA_H
