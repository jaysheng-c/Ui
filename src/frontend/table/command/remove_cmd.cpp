/**
  ********************************************************************************
  * @file           : remove_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#include "remove_cmd.h"
#include <QHeaderView>
#include <QUndoCommand>
#include <QVariant>

#include "table/table_view.h"

REFLECT(RemoveCmd)

namespace {

class Cmd final : public QUndoCommand {
public:
    struct Data {
        int begin = -1;
        int count = -1;
        QVariantList data;

        bool operator<(const Data &other) const { return begin < other.begin; }
        bool operator==(const Data &other) const { return begin == other.begin; }
        bool operator!=(const Data &other) const { return begin != other.begin; }
        bool operator>(const Data &other) const { return begin > other.begin; }
    };

    enum Type { Column, Row, };
    Cmd(TableView *table, QVariant &&old, QVariant &&cur, const Type type)
            : QUndoCommand("remove"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur)), m_type(type) {}
    void redo() override
    {
        auto dataList = m_cur.value<QVector<Data>>();
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
    void undo() override
    {
        auto dataList = m_old.value<QVector<Data>>();
        switch (m_type) {
            case Column: {
                const int rowCount = m_table->model()->rowCount();
                for (const auto &data: dataList) {
                    m_table->model()->insertColumns(data.begin, data.count);
                    for (auto i{0}; i < data.count; ++i) {
                        const int column = data.begin + i;
                        for (auto row{0}; row < rowCount; ++row) {
                            const auto &index = m_table->model()->index(row, column);
                            m_table->model()->setData(index, data.data.at(row + rowCount * i), Qt::UserRole);
                        }
                    }
                }
                break;
            }
            case Row: {
                const int columnCount = m_table->model()->columnCount();
                for (const auto &data: dataList) {
                    m_table->model()->insertRows(data.begin, data.count);
                    for (auto i{0}; i < data.count; ++i) {
                        const int row = data.begin + i;
                        for (auto column{0}; column < columnCount; ++column) {
                            const auto &index = m_table->model()->index(row, column);
                            m_table->model()->setData(index, data.data.at(column + columnCount * i), Qt::UserRole);
                        }
                    }
                }
                break;
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

RemoveCmd::RemoveCmd(TableView *table) : TableCmd(table)
{

}

void RemoveCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    if (contextObject == nullptr) {
        return;
    }
    if (contextObject == m_table->horizontalHeader()) {
        QVector<Cmd::Data> oldList, curList;
        for (const auto &item : selectionItem) {
            const int rowCount = m_table->model()->rowCount();
            curList.append(Cmd::Data{item.left(), item.width()});

            Cmd::Data data{item.left(), item.width(), QVariantList(item.width() * rowCount)};
            for (auto i {0}; i < data.count; ++i) {
                const int column = data.begin + i;
                for (auto row {0}; row < rowCount; ++row) {
                    const auto &index = m_table->model()->index(row, column);
                    data.data[row + rowCount * i] = m_table->model()->data(index, Qt::UserRole);
                }
            }
            oldList.append(std::move(data));
        }
        auto old = QVariant::fromValue(Smaller<Cmd::Data>()(oldList));
        auto cur = QVariant::fromValue(Greater<Cmd::Data>()(curList));
        return TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur), Cmd::Column));
    }
    if (contextObject == m_table->verticalHeader()) {
        QVector<Cmd::Data> oldList, curList;
        for (const auto &item : selectionItem) {
            const int columnCount = m_table->model()->columnCount();
            curList.append(Cmd::Data{item.top(), item.height()});

            Cmd::Data data{item.top(), item.height(), QVariantList(item.height() * columnCount)};
            for (auto i {0}; i < data.count; ++i) {
                const int row = data.begin + i;
                for (auto column {0}; column < columnCount; ++column) {
                    const auto &index = m_table->model()->index(row, column);
                    data.data[column + columnCount * i] = m_table->model()->data(index, Qt::UserRole);
                }
            }
            oldList.append(std::move(data));
        }
        auto old = QVariant::fromValue(Smaller<Cmd::Data>()(oldList));
        auto cur = QVariant::fromValue(Greater<Cmd::Data>()(curList));
        return TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur), Cmd::Row));
    }
}
