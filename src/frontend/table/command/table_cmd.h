/**
  ********************************************************************************
  * @file           : table_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_CMD_H
#define TABLE_CMD_H

#include <QVector>

class TableView;
class QObject;
class QItemSelection;

class TableCmd {
public:
    explicit TableCmd(TableView *table) : m_table(table) {}
    virtual ~TableCmd() = default;

    virtual void cmd(QObject *contextObject, const QItemSelection &selectionItem) = 0;

protected:
    TableView *m_table;
};

template<class T>
class Greater {
public:
    QVector<T> operator()(const QVector<T> &data) const
    {
        auto d = data;
        std::sort(d.begin(), d.end(), std::greater<T>());
        return d;
    }
};

template<class T>
class Smaller {
public:
    QVector<T> operator()(const QVector<T> &data) const
    {
        auto d = data;
        std::sort(d.begin(), d.end(), std::less<T>());
        return d;
    }
};

#endif //TABLE_CMD_H
