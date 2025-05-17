/**
  ********************************************************************************
  * @file           : clear_cmd.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/14
  * @version        : 1.0
  ********************************************************************************
  */

#include "clear_cmd.h"
#include <QUndoCommand>
#include "table/table_view.h"
#include "table/table_model.h"

REFLECT(ClearCmd)

namespace {

struct Data {
    QItemSelection selection;
    QVariant values;
};

class Cmd final : public QUndoCommand {
public:
    Cmd(TableView *table, QVariant &&old, QVariant &&cur)
        : QUndoCommand("Clear"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur))
    {
    }

    void redo() override
    {
        const auto model = m_table->tableModel();
        const auto [selection, values] = m_cur.value<Data>();
        int left{std::numeric_limits<int>::max()}, top{std::numeric_limits<int>::max()}, right{0}, bottom{0};
        for (const auto &d : selection) {
            left = qMin(d.left(), left);
            top = qMin(d.top(), top);
            right = qMax(d.right(), right);
            bottom = qMax(d.bottom(), bottom);
        }
        for (const auto &item : selection.indexes()) {
            (void) model->setDataWithoutCommit(item, "", Qt::DisplayRole);
        }
        model->dataChanged(model->index(top, left), model->index(bottom, right));
    }

    void undo() override
    {
        const auto model = m_table->tableModel();
        const auto [selection, values] = m_old.value<Data>();
        const auto indexes = selection.indexes();
        const auto dataList = values.value<QVariantList>();
        int left{std::numeric_limits<int>::max()}, top{std::numeric_limits<int>::max()}, right{0}, bottom{0};
        for (const auto &d : selection) {
            left = qMin(d.left(), left);
            top = qMin(d.top(), top);
            right = qMax(d.right(), right);
            bottom = qMax(d.bottom(), bottom);
        }
        for (qsizetype i {0}; i < selection.indexes().size(); ++i) {
            (void) model->setDataWithoutCommit(indexes.at(i), dataList.at(i), Qt::DisplayRole);
        }
        model->dataChanged(model->index(top, left), model->index(bottom, right));
    }

private:
    QPointer<TableView> m_table;
    QVariant m_old;
    QVariant m_cur;
};

}

ClearCmd::ClearCmd(TableView *table) : TableCmd(table)
{
}

void ClearCmd::cmd(QObject *contextObject, const QItemSelection &selection)
{
    Q_ASSERT_X(contextObject, "ClearCmd::cmd", "contextObject is nullptr");
    if (selection.isEmpty()) {
        qInfo() << "ClearCmd::cmd" << "selection is empty";
        return;
    }

    auto old = QVariant::fromValue(Data{selection, m_table->tableModel()->data(selection, Qt::DisplayRole)});
    auto cur = QVariant::fromValue(Data{selection, {}});
    TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur)));
}
