/**
  ********************************************************************************
  * @file           : paste_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#include "paste_cmd.h"

#include <QHeaderView>
#include <QUndoCommand>
#include <QVariant>
#include <QItemSelectionRange>

#include "table/data/copy_data.h"
#include "table/table_view.h"

namespace {
struct Range {
    int left;
    int right;
    int top;
    int bottom;
};
struct Data {
    QVector<Range> ranges;
    QVariantList values;
};
struct OptData {
    QVector<int> insertRows;
    QVector<int> insertColumns;
};
class CopyCmd : public QUndoCommand {
public:
    CopyCmd(TableView *table, QVariant &&old, QVariant &&cur, OptData &&optData, int role = Qt::UserRole)
        : QUndoCommand("copy_paste"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur)),
          m_optData(optData), m_role(role) {}

    void redo() override
    {
        // 判断是否需要插入行或列
        insert();
        auto data = m_cur.value<Data>();
        for (const auto &range : data.ranges) {
            qsizetype i = 0;
            for (auto row{range.top}; row <= range.bottom; ++row) {
                for (auto column{range.left}; column <= range.right; ++column) {
                    m_table->model()->setData(m_table->model()->index(row, column), data.values.at(i++), m_role);
                }
            }
        }
    }

    void undo() override
    {
        remove();
        auto data = m_old.value<Data>();
        for (const auto &range : data.ranges) {
            qsizetype i = 0;
            for (auto row{range.top}; row <= range.bottom; ++row) {
                for (auto column{range.left}; column <= range.right; ++column) {
                    m_table->model()->setData(m_table->model()->index(row, column), data.values.at(i++), m_role);
                }
            }
        }
    }
private:
    void insert()
    {
        if (!m_optData.insertColumns.isEmpty()) {
            m_table->model()->insertColumns(m_optData.insertColumns.first(),
                                            static_cast<int>(m_optData.insertColumns.size()));
        }
        if (!m_optData.insertRows.isEmpty()) {
            m_table->model()->insertRows(m_optData.insertRows.first(),
                                         static_cast<int>(m_optData.insertRows.size()));
        }
    }
    void remove()
    {
        if (!m_optData.insertColumns.isEmpty()) {
            m_table->model()->removeColumns(m_optData.insertColumns.first(),
                                            static_cast<int>(m_optData.insertColumns.size()));
        }
        if (!m_optData.insertRows.isEmpty()) {
            m_table->model()->removeRows(m_optData.insertRows.first(),
                                         static_cast<int>(m_optData.insertRows.size()));
        }
    }
    TableView *m_table;
    QVariant m_old;
    QVariant m_cur;

    OptData m_optData;
    int m_role;
};


class AddList {
public:
    void operator()(QVector<int> &list, int begin, int count)
    {
        if (count > 0) {
            auto i = list.size();
            list.resize(i + count, -1);
            for (int j {0}; j < count; ++j) {
                auto v = begin + j;
                if (!list.contains(v)) {
                    list[i + j] = v;
                }
            }
            list.removeAll(-1);
        }
    }
};

}

PasteCmd::PasteCmd(TableView *table) : TableCmd(table)
{

}

void PasteCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_UNUSED(contextObject)
    auto model = m_table->model();
    auto copyData = CopyData::instance().data<QVariantList>();
    auto copyRange = CopyData::instance().range<QItemSelectionRange>();

    QVector<Range> pasteSelection(selectionItem.size());
    QVector<int> insertRows, insertColumns;
    AddList addList;
    // 计算获得待处理的区域
    for (qsizetype i {0}; i < selectionItem.size(); ++i) {
        const auto &topLeft = selectionItem.at(i).topLeft();
        Range range{topLeft.column(), topLeft.column() + copyRange.width() - 1,
                    topLeft.row(), topLeft.row() + copyRange.height() - 1};

        pasteSelection[i] = range;
        // 判断是否需要插入行列
        addList(insertColumns, model->columnCount(), range.right - model->columnCount() + 1);
        addList(insertRows, model->rowCount(), range.bottom - model->rowCount() + 1);
    }
    // 对需要插入的行列进行排序
    insertRows = Smaller<int>()(insertRows);
    insertColumns = Smaller<int>()(insertColumns);

    // 获取旧数据
    auto [preSelection, preData] = data(pasteSelection, Qt::UserRole);

    auto cur = QVariant::fromValue(Data{pasteSelection, copyData});
    auto old = QVariant::fromValue(Data{preSelection, preData});

    TableView::undoStack().push(new CopyCmd(m_table, std::move(old), std::move(cur),
                                            OptData{std::move(insertRows), std::move(insertColumns)}));
}

PasteCmd::DataPair PasteCmd::data(const QVector<Range> &origin, int role) const
{
    QVariantList data;
    QItemSelection itemSelection(origin.size());
    QVector<Range> itemRanges(origin.size());

    auto model = m_table->model();
    // 计算需要获取的数据大小
    for (qsizetype i {0}; i < origin.size(); ++i) {
        const auto &range = origin.at(i);
        int right = range.right < model->columnCount() ? range.right : model->columnCount() - 1;
        int bottom = range.bottom < model->rowCount() ? range.bottom : model->rowCount() - 1;
        itemSelection[i] = {model->index(range.top, range.left), model->index(bottom, right)};
        itemRanges[i] = {range.left, right, range.top, bottom};
    }
    // 获取旧数据
    const auto &indexes = itemSelection.indexes();
    data.resize(indexes.size());
    for (qsizetype i {0}; i < indexes.size(); ++i) {
        data[i] = model->data(indexes.at(i), role);
    }
    return {itemRanges, data};
}
