/**
  ********************************************************************************
  * @file           : table_view.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_view.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>

#include <QHeaderView>
#include <QMouseEvent>
#include <QUndoStack>

#include <QMessageBox>

#include "table_model.h"
#include "table_delegate.h"
#include "table_menu.h"

#include "data/copy_data.h"
#include "command/insert_cmd.h"
#include "command/remove_cmd.h"
#include "command/paste_cmd.h"

QUndoStack TableView::s_stack;

TableView::TableView(QWidget *parent)
    : QTableView(parent), m_menu(nullptr), m_model(nullptr)
{
    this->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    this->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    this->verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setItemDelegate(new TableDelegate(this));
    setTableMenu(new TableMenu(this));
    setTableModel(new TableModel(ExpandType::Row, this));

    m_commands = {
        {Table::TypeFlag::Insert, std::move(std::make_shared<InsertCmd>(this))},
        {Table::TypeFlag::Remove, std::move(std::make_shared<RemoveCmd>(this))},
        {Table::TypeFlag::Paste, std::move(std::make_shared<PasteCmd>(this))},
    };

    (void) connect(this->verticalHeader(), &QTableView::customContextMenuRequested, this, &TableView::onExecMenu);
    (void) connect(this->horizontalHeader(), &QTableView::customContextMenuRequested, this, &TableView::onExecMenu);
    (void) connect(this, &QTableView::customContextMenuRequested, this, &TableView::onExecMenu);
}


void TableView::setCmd(Table::TypeFlag type, const std::shared_ptr<TableCmd> &cmd)
{
    if (m_commands.value(type, nullptr) == cmd) {
        return;
    }
    m_commands[type] = cmd;
}

void TableView::setTableMenu(TableMenu *menu) noexcept
{
    if (m_menu == menu) {
        return;
    }
    delete m_menu;
    m_menu = menu;
    if (m_menu) {
        (void) connect(m_menu, &TableMenu::triggered, this, &TableView::onMenuTriggered);
    }
}

void TableView::setTableModel(TableModel *model)
{
    if (m_model == model) {
        return;
    }
    QTableView::setModel(model);
    m_model = model;
}

void TableView::mousePressEvent(QMouseEvent *event)
{
    QTableView::mousePressEvent(event);
}

void TableView::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Z && s_stack.canUndo()) {
            s_stack.undo();
        } else if (event->key() == Qt::Key_Y && s_stack.canRedo()) {
            s_stack.redo();
        } else if (event->key() == Qt::Key_C || event->key() == Qt::Key_X) {
            onMenuTriggered(this, event->key() == Qt::Key_C ? Table::TypeFlag::Copy : Table::TypeFlag::Cut);
        } else if (event->key() == Qt::Key_V) {
            onMenuTriggered(this, Table::TypeFlag::Paste);
        }
    }
    QTableView::keyPressEvent(event);
}

void TableView::onExecMenu(const QPoint &pos) const
{
    auto *selectionModel = this->selectionModel();
    auto selections = selectionModel->selection();
    bool clearFlag{true};
    if (this->sender() == this->horizontalHeader()) {
        const int section = this->horizontalHeader()->logicalIndexAt(pos);
        // 判断是否为列选中且列选中区域为当前右键对应的列
        for (const auto &selection: selections) {
            clearFlag = (selection.left() > section || selection.right() < section);
            clearFlag |= (selection.height() != this->model()->rowCount());
        }
        if (clearFlag) {
            selectionModel->clear();
            const QItemSelection selection(m_model->index(0, section),
                                           m_model->index(m_model->rowCount({}) - 1, section));
            selectionModel->select(selection, QItemSelectionModel::Select);
        }
    } else if (this->sender() == this->verticalHeader()) {
        const int section = this->verticalHeader()->logicalIndexAt(pos);
        // 判断是否为列选中且列选中区域为当前右键对应的列
        for (const auto &selection: selections) {
            clearFlag = (selection.top() > section || selection.bottom() < section);
            clearFlag |= (selection.width() != this->model()->columnCount());
        }
        if (clearFlag) {
            selectionModel->clear();
            const QItemSelection selection(m_model->index(section, 0),
                                           m_model->index(section, m_model->columnCount({}) - 1));
            selectionModel->select(selection, QItemSelectionModel::Select);
        }
    }
    if (m_menu) {
        m_menu->setContextObject(this->sender());
        m_menu->execMenu(QCursor::pos());
    }
}

void TableView::onMenuTriggered(QObject *contextObject, Table::TypeFlag type)
{
    const auto *selectionModel = this->selectionModel();
    if (!selectionModel->hasSelection()) {
        return;
    }
    auto selectionItem = selectionModel->selection();
    switch (type) {
        case Table::TypeFlag::Cut:
        case Table::TypeFlag::Copy: {
            if (selectionItem.size() > 1) {
                QMessageBox::warning(this, "警告", "无法对多重区域执行此操作！");
                return;
            }
            const auto range = QVariant::fromValue(selectionItem.first());
            CopyData::instance().setData(type, m_model->data(selectionItem, Qt::UserRole), range, this);
            break;
        }
        case Table::TypeFlag::Insert:
        case Table::TypeFlag::Remove:
            // 判断选中数据是否有效
            if (contextObject == this->horizontalHeader() || contextObject == this->verticalHeader()) {
                if (selectionItem.size() > 1) {
                    const int count = contextObject == this->horizontalHeader() ? selectionItem.first().height()
                                          : selectionItem.first().width();
                    for (const auto &selection: selectionItem) {
                        const int c = contextObject == this->horizontalHeader() ? selection.height()
                                            : selection.width();
                        if (count != c) {
                            QMessageBox::warning(this, "警告",
                                                 "当前选定区域同时包含整行（或整列）单元格及单元格区域时，此命令无效。"
                                                 "要么选定整行或整列，要么选定单元格区域。");
                            return;
                        }
                    }
                }
            }
        case Table::TypeFlag::Paste:
            if (m_commands.contains(type)) {
                m_commands.value(type)->cmd(contextObject, selectionItem);
            }
            break;
        case Table::TypeFlag::Clear:
        default:
            break;
    }
}
