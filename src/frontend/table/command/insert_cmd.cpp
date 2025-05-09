/**
  ********************************************************************************
  * @file           : insert_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#include "insert_cmd.h"
#include <QHeaderView>
#include <QUndoCommand>
#include <QVariant>

#include "table/table_view.h"

REFLECT(InsertCmd)

namespace {

class Cmd final : public QUndoCommand {
public:
    struct Data {
        int begin = -1;
        int count = -1;

        bool operator<(const Data &other) const { return begin < other.begin; }
        bool operator==(const Data &other) const { return begin == other.begin; }
        bool operator!=(const Data &other) const { return begin != other.begin; }
        bool operator>(const Data &other) const { return begin > other.begin; }
    };
    enum Type { Column, Row, };

    Cmd(TableView *table, QVariant &&old, QVariant &&cur, Type type)
            : QUndoCommand("insert"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur)), m_type(type) {}
    void redo() override
    {
        auto dataList = m_cur.value<QVector<Data>>();
        switch (m_type) {
            case Column:
                for (const auto &data : dataList) {
                    m_table->model()->insertColumns(data.begin, data.count);
                }
                break;
            case Row:
                for (const auto &data : dataList) {
                    m_table->model()->insertRows(data.begin, data.count);
                }
            default:
                break;
        }
    }

    void undo() override
    {
        auto dataList = m_old.value<QVector<Data>>();
        switch (m_type) {
            case Column:
                for (const auto &data : dataList) {
                    m_table->model()->removeColumns(data.begin, data.count);
                }
                break;
            case Row:
                for (const auto &data : dataList) {
                    m_table->model()->removeRows(data.begin, data.count);
                }
            default:
                break;
        }
    }

private:
    TableView *m_table;
    QVariant m_old;
    QVariant m_cur;
    int m_type;
};

}

InsertCmd::InsertCmd(TableView *table) : TableCmd(table)
{

}

void InsertCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    if (contextObject == nullptr) {
        return;
    }
    if (contextObject == m_table->horizontalHeader()) {
        QVector<Cmd::Data> list;
        for (const auto &item : selectionItem) {
            list.append(Cmd::Data{item.left(), item.width()});
        }
        auto old = QVariant::fromValue(Smaller<Cmd::Data>()(list));
        auto cur = QVariant::fromValue(Greater<Cmd::Data>()(list));
        return TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur), Cmd::Column));
    } else if (contextObject == m_table->verticalHeader()) {
        QVector<Cmd::Data> list;
        for (const auto &item : selectionItem) {
            list.append(Cmd::Data{item.top(), item.height()});
        }
        auto old = QVariant::fromValue(Smaller<Cmd::Data>()(list));
        auto cur = QVariant::fromValue(Greater<Cmd::Data>()(list));
        return TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur), Cmd::Row));
    }
}
