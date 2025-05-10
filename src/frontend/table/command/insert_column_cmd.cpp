/**
  ********************************************************************************
  * @file           : insert_column_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "insert_column_cmd.h"
#include <QUndoCommand>
#include "table/table_view.h"

REFLECT(InsertColumnCmd)

namespace {
struct Data {
    int begin = -1;
    int count = -1;

    bool operator<(const Data &other) const { return begin < other.begin; }
    bool operator==(const Data &other) const { return begin == other.begin; }
    bool operator!=(const Data &other) const { return begin != other.begin; }
    bool operator>(const Data &other) const { return begin > other.begin; }
};

class Cmd final : public QUndoCommand {
public:
    Cmd(TableView *table, QVariant &&old, QVariant &&cur)
        : QUndoCommand("InsertColumn"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur))
    {
    }

    void redo() override
    {
        auto dataList = m_cur.value<QVector<Data> >();
        for (const auto &[begin, count]: dataList) {
            m_table->model()->insertColumns(begin, count);
        }
    }

    void undo() override
    {
        auto dataList = m_old.value<QVector<Data> >();
        for (const auto &[begin, count]: dataList) {
            m_table->model()->removeColumns(begin, count);
        }
    }

private:
    TableView *m_table;
    QVariant m_old;
    QVariant m_cur;
};
}

InsertColumnCmd::InsertColumnCmd(TableView *table) : TableCmd(table)
{
}

void InsertColumnCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_ASSERT_X(contextObject, "InsertColumnCmd::cmd", "contextObject is null");
    QVector<Data> list;
    for (const auto &item : selectionItem) {
        list.append(Data{item.left(), item.width()});
    }
    auto old = QVariant::fromValue(Smaller<Data>()(list));
    auto cur = QVariant::fromValue(Greater<Data>()(list));
    TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur)));
}
