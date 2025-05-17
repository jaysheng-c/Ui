/**
  ********************************************************************************
  * @file           : move_up_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/17
  * @version        : 1.0
  ********************************************************************************
  */

#include "move_up_cmd.h"
#include <QUndoCommand>
#include "table/table_view.h"
#include "table/table_model.h"

REFLECT(MoveUpCmd)

namespace {

class Cmd final : public QUndoCommand {
public:
    Cmd(TableView *table, const TableCmd::Range &range, QVariant &&old, QVariant &&cur)
        : QUndoCommand("MoveUp"), m_table(table), m_range(range), m_old(std::move(old)),
        m_cur(std::move(cur))
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
        for (int c{0}; c < cCount; ++c) {
            const auto &d = data.at(c);
            for (int r{0}; r < rCount; ++r) {
                if (!model->setDataWithoutCommit(model->index(r + top, c + left), d.at(r), Qt::UserRole)) {
                    break;
                }
            }
        }
        model->submit();
    }

    void redo() override
    {
        const auto model = m_table->tableModel();
        const auto data = m_cur.value<QVector<QVector<QVariant>>>();
        const int top = m_range.top;
        const int left = m_range.left;
        const int cCount = m_range.right - m_range.left + 1;
        const int rCount = m_range.bottom - m_range.top + 1;
        for (int c{0}; c < cCount; ++c) {
            const auto &d = data.at(c);
            for (int r{0}; r < rCount; ++r) {
                if (!model->setDataWithoutCommit(model->index(r + top, c + left), d.at(r), Qt::UserRole)) {
                    break;
                }
            }
        }
        model->submit();
    }
private:
    QPointer<TableView> m_table;
    TableCmd::Range m_range;
    QVariant m_old;
    QVariant m_cur;
};

}

MoveUpCmd::MoveUpCmd(TableView *table) : TableCmd(table)
{
}

void MoveUpCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_ASSERT_X(contextObject, "MoveRightCmd", "contextObject is nullptr");
    if (selectionItem.isEmpty()) {
        qInfo() << "selectionItem is empty";
        return;
    }
    auto sortSelection = selectionItem;
    // 从下向上排序
    std::sort(sortSelection.begin(), sortSelection.end(),
        [](const QItemSelectionRange &lhs, const QItemSelectionRange &rhs) {
        return lhs.top() > rhs.top();
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
    const int colCount = model->columnCount();
    int lastRowIndex = model->rowCount() - 1;
    bool stopFlag = false;
    while (lastRowIndex >= 0) {
        for (int column{0}; column < colCount; ++column) {
            if (!model->data(model->index(lastRowIndex, column), Qt::DisplayRole).toString().isEmpty()) {
                stopFlag = true;
                break;
            }
        }
        if (stopFlag) {
            break;
        }
        --lastRowIndex;
    }
    // 获取数据，并插入数据
    const int maxRow = lastRowIndex - range.top + 1;
    QVector<QVector<QVariant>> curData(range.right - range.left + 1, QVector<QVariant>(maxRow));
    for (int col{range.left}; col <= range.right; ++col) {
        for (int row{range.top}; row <= lastRowIndex; ++row) {
            curData[col - range.left][row - range.top] = model->data(model->index(row, col), Qt::UserRole);
        }
    }
    auto oldData = curData;
    for (qsizetype i{0}; i < sortSelection.size(); ++i) {
        const auto &curRange = sortSelection.at(i);
        for (int col{curRange.left()}; col <= curRange.right(); ++col) {
            const int j = col - range.left;
            curData[j].remove(curRange.top() - range.top, curRange.height());
        }
    }
    // 更新底部索引，并对齐数据
    range.bottom = range.top + maxRow - 1;
    for (qsizetype i{0}; i < curData.size(); ++i) {
        curData[i].resize(maxRow);
        oldData[i].resize(maxRow);
    }
    TableView::undoStack().push(new Cmd(m_table, range, std::move(QVariant::fromValue(oldData)),
        std::move(QVariant::fromValue(curData))));
}
