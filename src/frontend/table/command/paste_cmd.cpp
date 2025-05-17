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

#include <QApplication>
#include <QClipboard>
#include <QUndoCommand>

#include "table/table_global.h"
#include "table/data/copy_data.h"
#include "table/table_view.h"
#include "table/table_model.h"

REFLECT(PasteCmd)

namespace {

struct Data {
    QVector<TableCmd::Range> ranges;
    QVariantList values;
};

struct CutData {
    Data data;
    QPointer<TableView> sourceTable;
};

struct OptData {
    QVector<int> insertRows;
    QVector<int> insertColumns;
};

using DataPair = QPair<QVector<TableCmd::Range>, QVariantList>;

class CopyCmd : public QUndoCommand {
public:
    CopyCmd(const QPointer<TableView> &table, QVariant &&old, QVariant &&cur, OptData &&optData,
            const int role = Qt::UserRole)
        : QUndoCommand("copy_paste"), m_table(table), m_old(std::move(old)), m_cur(std::move(cur)),
          m_optData(optData), m_role(role)
    {
    }

    void redo() override
    {
        // 判断是否需要插入行或列
        insert();
        auto [ranges, values] = m_cur.value<Data>();
        for (const auto [left, right, top, bottom]: ranges) {
            qsizetype i = 0;
            for (int row{top}; row <= bottom; ++row) {
                for (int column{left}; column <= right; ++column) {
                    m_table->tableModel()->setData(m_table->model()->index(row, column), values.at(i++), m_role);
                }
            }
        }
        m_table->tableModel()->submit();
    }

    void undo() override
    {
        remove();
        auto [ranges, values] = m_old.value<Data>();
        for (const auto [left, right, top, bottom]: ranges) {
            qsizetype i = 0;
            for (int row{top}; row <= bottom; ++row) {
                for (int column{left}; column <= right; ++column) {
                    m_table->tableModel()->setData(m_table->model()->index(row, column), values.at(i++), m_role);
                }
            }
        }
        m_table->tableModel()->submit();
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
protected:
    int m_role;

private:
    QPointer<TableView> m_table;
    QVariant m_old;
    QVariant m_cur;

    OptData m_optData;
};

class CutCmd final : public CopyCmd {
public:
    CutCmd(const CutData &source, const QPointer<TableView> &table, QVariant &&old, QVariant &&cur,
           OptData &&optData)
        : CopyCmd(table, std::move(old), std::move(cur), std::move(optData))
    {
        m_sourceTable = source.sourceTable;
        m_sourceData = source.data;
    }

    void redo() override
    {
        if (!m_sourceTable.isNull()) {
            for (const auto [left, right, top, bottom]: m_sourceData.ranges) {
                for (int row{top}; row <= bottom; ++row) {
                    for (int column{left}; column <= right; ++column) {
                        m_sourceTable->model()->setData(m_sourceTable->model()->index(row, column), "",
                                                        Qt::DisplayRole);
                    }
                }
            }
        }
        CopyCmd::redo();
    }

    void undo() override
    {
        CopyCmd::undo();
        if (!m_sourceTable.isNull()) {
            for (const auto [left, right, top, bottom]: m_sourceData.ranges) {
                qsizetype i = 0;
                for (int row{top}; row <= bottom; ++row) {
                    for (int column{left}; column <= right; ++column) {
                        m_sourceTable->model()->setData(m_sourceTable->model()->index(row, column),
                                                        m_sourceData.values.at(i++), m_role);
                    }
                }
            }
        }
    }

private:
    QPointer<TableView> m_sourceTable;
    Data m_sourceData;
};

class AddList {
public:
    void operator()(QVector<int> &list, const int begin, const int count) const
    {
        if (count > 0) {
            const qsizetype i = list.size();
            list.resize(i + count, -1);
            for (int j{0}; j < count; ++j) {
                int v = begin + j;
                if (!list.contains(v)) {
                    list[i + j] = v;
                }
            }
            list.removeAll(-1);
        }
    }
};

class TableDataAndRange {
public:
    explicit TableDataAndRange(const int role) : m_role(role)
    {
    }

    DataPair operator()(const QVector<TableCmd::Range> &origin, const TableView *table) const
    {
        QVariantList data;
        QItemSelection itemSelection(origin.size());
        QVector<TableCmd::Range> itemRanges(origin.size());

        const auto *model = table->model();
        // 计算需要获取的数据大小
        for (qsizetype i{0}; i < origin.size(); ++i) {
            auto [left, right, top, bottom] = origin.at(i);
            right = right < model->columnCount() ? right : model->columnCount() - 1;
            bottom = bottom < model->rowCount() ? bottom : model->rowCount() - 1;
            itemSelection[i] = {model->index(top, left), model->index(bottom, right)};
            itemRanges[i] = {left, right, top, bottom};
        }
        // 获取旧数据
        const auto &indexes = itemSelection.indexes();
        data.resize(indexes.size());
        for (qsizetype i{0}; i < indexes.size(); ++i) {
            data[i] = model->data(indexes.at(i), m_role);
        }
        return {itemRanges, data};
    }

private:
    int m_role;
};
}

PasteCmd::PasteCmd(TableView *table) : TableCmd(table)
{
}

void PasteCmd::cmd(QObject *contextObject, const QItemSelection &selectionItem)
{
    Q_UNUSED(contextObject)
    if (!CopyData::instance().range<QItemSelectionRange>().isEmpty()) {
        // 系统内部的复制、剪切操作
        tableCmd(selectionItem);
    } else if (const auto copyText = QApplication::clipboard()->text(); !copyText.isEmpty()) {
        // 系统外部的复制剪切操作
        // 获得复制的数据
        const auto rowData = copyText.split('\n');
        const qsizetype rowCount = rowData.size();
        qsizetype columnCount = 0;
        QMap<qsizetype, QStringList> colDataMap;
        for (qsizetype i{0}; i < rowData.size(); ++i) {
            colDataMap[i] = rowData.at(i).split('\t');
            columnCount = qMax(columnCount, colDataMap.value(i).size());
        }
        QList<QVariant> copyData(rowCount * columnCount, "");
        auto it = copyData.begin();
        for (qsizetype i{0}; i < rowCount; ++i) {
            it = std::move(colDataMap[i].begin(), colDataMap[i].end(), it);
        }
        if (copyData.isEmpty()) {
            return;
        }

        const auto copyRange = CopyData::instance().range<QItemSelectionRange>();
        const auto *model = m_table->model();
        QVector<Range> pasteSelection(selectionItem.size());
        QVector<int> insertRows, insertColumns;
        // 计算待处理的数据
        for (qsizetype i{0}; i < selectionItem.size(); ++i) {
            const auto &topLeft = selectionItem.at(i).topLeft();
            const Range range{
                topLeft.column(), topLeft.column() + static_cast<int>(columnCount) - 1,
                topLeft.row(), topLeft.row() + static_cast<int>(rowCount) - 1
            };
            pasteSelection[i] = range;
            AddList addList;
            addList(insertColumns, model->columnCount(), range.right - model->columnCount() + 1);
            addList(insertRows, model->rowCount(), range.bottom - model->rowCount() + 1);
        }
        // 获取旧数据
        auto [preSelection, preData] = TableDataAndRange(Qt::DisplayRole)(pasteSelection, m_table);
        auto cur = QVariant::fromValue(Data{pasteSelection, copyData});
        auto old = QVariant::fromValue(Data{preSelection, preData});
        TableView::undoStack().push(new CopyCmd(m_table, std::move(old), std::move(cur),
                                                OptData{std::move(insertRows), std::move(insertColumns)},
                                                Qt::EditRole));
    }
}

void PasteCmd::tableCmd(const QItemSelection &selectionItem) const
{
    const auto copyData = CopyData::instance().data<QVariantList>();
    const auto copyRange = CopyData::instance().range<QItemSelectionRange>();
    const auto *model = m_table->model();
    if (copyData.isEmpty() && QApplication::clipboard()->text().isEmpty()) {
        return;
    }

    QVector<Range> pasteSelection(selectionItem.size());
    QVector<int> insertRows, insertColumns;
    // 计算获得待处理的区域
    for (qsizetype i{0}; i < selectionItem.size(); ++i) {
        AddList addList;
        const auto &topLeft = selectionItem.at(i).topLeft();
        const Range range{
            topLeft.column(), topLeft.column() + copyRange.width() - 1,
            topLeft.row(), topLeft.row() + copyRange.height() - 1
        };

        pasteSelection[i] = range;
        // 判断是否需要插入行列
        addList(insertColumns, model->columnCount(), range.right - model->columnCount() + 1);
        addList(insertRows, model->rowCount(), range.bottom - model->rowCount() + 1);
    }
    // 对需要插入的行列进行排序
    insertRows = Smaller<int>()(insertRows);
    insertColumns = Smaller<int>()(insertColumns);
    // 获取旧数据
    auto [preSelection, preData] = TableDataAndRange(Qt::UserRole)(pasteSelection, m_table);

    auto cur = QVariant::fromValue(Data{pasteSelection, copyData});
    auto old = QVariant::fromValue(Data{preSelection, preData});

    const auto opt = CopyData::instance().opt();
    if (Table::TypeFlag::Copy == opt) {
        TableView::undoStack().push(new CopyCmd(m_table, std::move(old), std::move(cur),
                                                OptData{std::move(insertRows), std::move(insertColumns)}));
    } else if (Table::TypeFlag::Cut == opt) {
        CutData sourceData;
        sourceData.sourceTable = CopyData::instance().sourceTable();
        sourceData.data = {{{copyRange.left(), copyRange.right(), copyRange.top(), copyRange.bottom()}}, copyData};
        TableView::undoStack().push(new CutCmd(sourceData, m_table, std::move(old), std::move(cur),
                                               OptData{std::move(insertRows), std::move(insertColumns)}));
    }
}
