/**
  ********************************************************************************
  * @file           : table_global.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/22
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_GLOBAL_H
#define TABLE_GLOBAL_H

namespace Table {
Q_NAMESPACE

enum class TypeFlag {
    None = 0,
    Cut,
    Copy,
    Paste,
    Insert,
    Remove,
    Clear,
};
Q_DECLARE_FLAGS(Type, TypeFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(Type)
Q_ENUM_NS(TypeFlag)
}
#endif //TABLE_GLOBAL_H
