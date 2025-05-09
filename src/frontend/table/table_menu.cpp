/**
  ********************************************************************************
  * @file           : table_menu.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/22
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_menu.h"
#include "table_view.h"

struct TableMenu::Item {
    enum {Action, Menu};
    int style = Action;
    Table::Type type = Table::TypeFlag::None;
    QString text;
    QIcon icon;
    QVector<Item> items;
};

TableMenu::TableMenu(TableView *table)
    : QMenu(table), m_tableView(table), m_contextObject(nullptr)
{
    this->setStyleSheet(
            "QMenu { width: 200px; background-color: #FFFFFF; color: #000000; }"
            "QMenu::separator { height: 1px; border-top: 1px solid #E6E6E6; margin: 3px 0px 3px 30px; }"
            "QMenu::item { height: 25px; padding-left: 30px; }"
            "QMenu::item:selected { background-color: #EBEBEB }"
//            "QMenu::icon { width: 100px; height: 100px; }"
    );

    createAction({Item::Action, Table::TypeFlag::Cut, "剪切"}, this);
    createAction({Item::Action, Table::TypeFlag::Copy, "复制"}, this);
    this->addSeparator();
    createAction({Item::Action, Table::TypeFlag::Paste, "粘贴"}, this);
    this->addSeparator();
    createAction({Item::Action, Table::TypeFlag::Insert, "插入"}, this);
    createAction({Item::Action, Table::TypeFlag::Remove, "删除"}, this);
    createAction({Item::Action, Table::TypeFlag::Clear, "清除内容"}, this);

}

void TableMenu::execMenu(const QPoint &pos)
{
    this->exec(pos);
}

void TableMenu::createAction(const TableMenu::Item &item, QMenu *parent)
{
    if (item.style != Item::Action) {
        return;
    }
    auto *action = new QAction(item.text, parent);
    action->setProperty("type", QVariant::fromValue(item.type));
//    if (!item.icon.isNull()) {
//        MenuInit::setActionIconStyle(action, item.icon, {42, 42});
//    }
    if (parent) {
        parent->addAction(action);
    }
    m_actions[item.type] = action;
    (void) connect(action, &QAction::triggered, this, &TableMenu::onTriggered);
}

void TableMenu::createItems(const TableMenu::Item &item, QMenu *parent)
{
    if (item.style == Item::Action) {
        createAction(item, parent);
        return;
    }
    auto *menu = new QMenu(item.text, parent);
//    if (!item.icon.isNull()) {
//        MenuInit::setMenuIconStyle(menu, item.icon, {42, 42});
//    }
    if (parent) {
        parent->addMenu(menu);
    }
    for (const auto &child : item.items) {
        // 递归调用
        createItems(child, menu);
    }
}

void TableMenu::onTriggered()
{
    const auto type = this->sender()->property("type").value<Table::TypeFlag>();
    emit triggered(m_contextObject, type);
    m_contextObject = nullptr;
}
