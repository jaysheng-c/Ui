/**
  ********************************************************************************
  * @file           : remove_row_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "remove_row_cmd.h"
#include <QUndoCommand>

#include "table/table_view.h"

REFLECT(RemoveRowCmd)

namespace {
struct Data {
    int begin = -1;
    int count = -1;
    QVariantList data;

    bool operator<(const Data &other) const { return begin < other.begin; }
    bool operator==(const Data &other) const { return begin == other.begin; }
    bool operator!=(const Data &other) const { return begin != other.begin; }
    bool operator>(const Data &other) const { return begin > other.begin; }
};

class Cmd final : public QUndoCommand {
public:
    Cmd(TableView *table, QVariant &&old, QVariant &&cur)
        : QUndoCommand("RemoveRow"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur))
    {
    }

    void redo() override
    {
        auto dataList = m_cur.value<QVector<Data> >();
        for (const auto &data : dataList) {
            m_table->model()->removeRows(data.begin, data.count);
        }
    }

    void undo() override
    {
        auto dataList = m_old.value<QVector<Data> >();
        const int columnCount = m_table->model()->columnCount();
        for (const auto &[begin, count, data]: dataList) {
            m_table->model()->insertRows(begin, count);
            for (auto i{0}; i < count; ++i) {
                const int row = begin + i;
                for (auto column{0}; column < columnCount; ++column) {
                    const auto &index = m_table->model()->index(row, column);
                    m_table->model()->setData(index, data.at(column + columnCount * i), Qt::UserRole);
                }
            }
        }
    }

private:
    TableView *m_table;
    QVariant m_old;
    QVariant m_cur;
};
}


RemoveRowCmd::RemoveRowCmd(TableView *table) : TableCmd(table)
{
}

void RemoveRowCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_ASSERT_X(contextObject, "RemoveRowCmd::cmd", "contextObject is null");
    QVector<Data> oldList, curList;
    for (const auto &item : selectionItem) {
        const int columnCount = m_table->model()->columnCount();
        curList.append(Data{item.top(), item.height()});

        Data data{item.top(), item.height(), QVariantList(item.height() * columnCount)};
        for (auto i {0}; i < data.count; ++i) {
            const int row = data.begin + i;
            for (auto column {0}; column < columnCount; ++column) {
                const auto &index = m_table->model()->index(row, column);
                data.data[column + columnCount * i] = m_table->model()->data(index, Qt::UserRole);
            }
        }
        oldList.append(std::move(data));
    }
    auto old = QVariant::fromValue(Smaller<Data>()(oldList));
    auto cur = QVariant::fromValue(Greater<Data>()(curList));
    TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur)));
}
