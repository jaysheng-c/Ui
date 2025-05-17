/**
  ********************************************************************************
  * @file           : move_right.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/14
  * @version        : 1.0
  ********************************************************************************
  */

#include "move_right_cmd.h"
#include <QUndoCommand>
#include "table/table_view.h"
#include "table/table_model.h"

REFLECT(MoveRightCmd)

namespace {

class Cmd final : public QUndoCommand {
public:
    Cmd(TableView *table, const TableCmd::Range &range, QVariant &&old, QVariant &&cur, const int insertCount)
        : QUndoCommand("MoveRight"), m_table(table), m_range(range), m_old(std::move(old)),
        m_cur(std::move(cur)), m_insertCount(insertCount)
    {
    }

    void undo() override
    {
        const auto model = m_table->tableModel();
        const auto data = m_old.value<QVector<QVector<QVariant>>>();
        const int top = m_range.top;
        const int left = m_range.left;
        const int cCount = m_range.right - m_range.left + 1;
        const int rCount = m_range.bottom - m_range.top + 1;
        if (m_insertCount > 0) {
            model->removeColumns(model->columnCount({}) - m_insertCount, m_insertCount, {});
        }
        for (int r{0}; r < rCount; ++r) {
            const auto &d = data.at(r);
            for (int c{0}; c < cCount; ++c) {
                if (!model->setDataWithoutCommit(model->index(r + top, c + left), d.at(c), Qt::UserRole)) {
                    break;
                }
            }
        }
        emit model->dataChanged(model->index(top, left), model->index(m_range.bottom, m_range.right));
    }

    void redo() override
    {
        const auto model = m_table->tableModel();
        const auto data = m_cur.value<QVector<QVector<QVariant>>>();
        const int top = m_range.top;
        const int left = m_range.left;
        const int cCount = m_range.right - m_range.left + 1;
        const int rCount = m_range.bottom - m_range.top + 1;
        if (m_insertCount > 0) {
            model->insertColumns(model->columnCount({}), m_insertCount, {});
        }
        for (int r{0}; r < rCount; ++r) {
            const auto &d = data.at(r);
            for (int c{0}; c < cCount; ++c) {
                if (!model->setDataWithoutCommit(model->index(r + top, c + left), d.at(c), Qt::UserRole)) {
                    break;
                }
            }
        }
        emit model->dataChanged(model->index(top, left), model->index(m_range.bottom, m_range.right));
    }

private:
    QPointer<TableView> m_table;
    TableCmd::Range m_range;
    QVariant m_old;
    QVariant m_cur;
    int m_insertCount;
};

}

MoveRightCmd::MoveRightCmd(TableView *table) : TableCmd(table)
{
}

void MoveRightCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_ASSERT_X(contextObject, "MoveRightCmd", "contextObject is nullptr");
    if (selectionItem.isEmpty()) {
        qInfo() << "selectionItem is empty";
        return;
    }
    auto sortSelection = selectionItem;
    // 从右向左排序
    std::sort(sortSelection.begin(), sortSelection.end(),
        [](const QItemSelectionRange &lhs, const QItemSelectionRange &rhs) {
        return lhs.left() > rhs.left();
    });

    const auto *model = m_table->model();

    // 获取选中范围的并集
    Range range{model->columnCount(), 0, model->rowCount(), 0};
    for (qsizetype i{0}; i < sortSelection.size(); ++i) {
        const auto &curRange = sortSelection.at(i);
        range.left = qMin(range.left, curRange.left());
        range.top = qMin(range.top, curRange.top());
        range.right = qMax(range.right, curRange.right());
        range.bottom = qMax(range.bottom, curRange.bottom());
    }

    // 获取最后列非空列索引
    const int rowCount = model->rowCount();
    int lastColIndex = model->columnCount() - 1;
    bool stopFlag = false;
    while (lastColIndex >= 0) {
        for (int row{0}; row < rowCount; ++row) {
            if (!model->data(model->index(row, lastColIndex), Qt::DisplayRole).toString().isEmpty()) {
                stopFlag = true;
                break;
            }
        }
        if (stopFlag) {
            break;
        }
        --lastColIndex;
    }
    // 获取数据，并插入数据
    int maxColumn = lastColIndex - range.left + 1;
    QVector<QVector<QVariant>> curData(range.bottom - range.top + 1, QVector<QVariant>(maxColumn));
    for (int row{range.top}; row <= range.bottom; ++row) {
        for (int col{range.left}; col <= lastColIndex; ++col) {
            curData[row - range.top][col - range.left] = model->data(model->index(row, col), Qt::UserRole);
        }
    }
    auto oldData = curData;
    for (qsizetype i{0}; i < sortSelection.size(); ++i) {
        const auto &curRange = sortSelection.at(i);
        for (int row{curRange.top()}; row <= curRange.bottom(); ++row) {
            const int j = row - range.top;
            curData[j].insert(curRange.left() - range.left, curRange.width(), {});
            maxColumn = qMax(maxColumn, static_cast<int>(curData[j].size()));
        }
    }
    // 计算是否需要插入列
    const int newTableColCount = maxColumn + range.left;
    const int insertCount = newTableColCount > model->columnCount() ? newTableColCount - model->columnCount() : 0;
    // 更新右侧索引，并对齐数据
    range.right = range.left  + maxColumn - 1;
    for (qsizetype i{0}; i < curData.size(); ++i) {
        curData[i].resize(maxColumn);
        oldData[i].resize(maxColumn);
    }
    TableView::undoStack().push(new Cmd(m_table, range, std::move(QVariant::fromValue(oldData)),
        std::move(QVariant::fromValue(curData)), insertCount));
}
