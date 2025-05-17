/**
  ********************************************************************************
  * @file           : table_view.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QPointer>
#include <QTableView>
#include "frontend/global/global.h"

namespace Table {
enum class TypeFlag;
}

class QUndoStack;
class TableMenu;
class TableModel;
class TableCmd;

class UI_EXPORT TableView : public QTableView {
Q_OBJECT
public:
    static QUndoStack &undoStack() { return s_stack; }

    explicit TableView(QWidget *parent = nullptr);
    void setCmd(Table::TypeFlag type, const std::shared_ptr<TableCmd> &cmd);
    void setTableMenu(TableMenu *menu) noexcept;
    void setTableModel(TableModel *model);
    NODISCARD QPointer<TableModel> tableModel() const { return m_model; }

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;


protected slots:
    void onExecMenu(const QPoint &pos) const;
    void onMenuTriggered(QObject *contextObject, Table::TypeFlag type);

private:
    static QUndoStack s_stack;
    QPointer<TableMenu> m_menu;
    QPointer<TableModel> m_model;

    QHash<Table::TypeFlag, std::shared_ptr<TableCmd>> m_commands;
};


#endif //TABLE_VIEW_H
