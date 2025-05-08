/**
  ********************************************************************************
  * @file           : copy_data.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#include "copy_data.h"
#include "table/table_view.h"
#include "table/table_global.h"

CopyData &CopyData::instance()
{
    static CopyData inst;
    return inst;
}

void CopyData::setData(const Table::TypeFlag opt, const QVariant &data, const QVariant &range, TableView *table)
{
    Q_ASSERT_X(table, "CopyData::setData", "table is nullptr");
    m_opt = opt;
    m_data = data;
    m_range = range;
    m_table = table;
}

void CopyData::clear()
{
    m_data.clear();
    m_range.clear();
}

CopyData::CopyData() : m_opt(Table::TypeFlag::None)
{

}
