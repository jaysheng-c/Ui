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
        const auto [selection, values] = m_cur.value<Data>();
        for (const auto &item : selection.indexes()) {
            m_table->model()->setData(item, "", Qt::DisplayRole);
        }
    }

    void undo() override
    {
        const auto [selection, values] = m_old.value<Data>();
        const auto indexes = selection.indexes();
        const auto dataList = values.value<QVariantList>();
        for (qsizetype i {0}; i < selection.indexes().size(); ++i) {
            m_table->model()->setData(indexes.at(i), dataList.at(i), Qt::DisplayRole);
        }
    }

private:
    TableView *m_table;
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
    auto *model = qobject_cast<TableModel*>(m_table->model());
    if (!model) {
        qWarning() << "ClearCmd::cmd" << "model is nullptr";
        return;
    }
    auto old = QVariant::fromValue(Data{selection, model->data(selection, Qt::DisplayRole)});
    auto cur = QVariant::fromValue(Data{selection, {}});
    TableView::undoStack().push(new Cmd(m_table, std::move(old), std::move(cur)));
}
