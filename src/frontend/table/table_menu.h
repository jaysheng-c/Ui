/**
  ********************************************************************************
  * @file           : table_menu.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/22
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TABLE_MENU_H
#define TABLE_MENU_H

#include <QMenu>
#include "table_global.h"

class TableView;


class TableMenu : public QMenu {
Q_OBJECT
signals:
    void triggered(QObject *contextObject, Table::TypeFlag type);

public:
    explicit TableMenu(TableView *table);
    void execMenu(const QPoint &pos);
    void setContextObject(QObject *object) { m_contextObject = object; }
private:
    struct Item;
    void createAction(const Item &item, QMenu *parent);
    void createItems(const Item &item, QMenu *parent);

    void onTriggered();


    QMap<Table::Type, QAction*> m_actions;
    TableView *m_tableView;
    QObject *m_contextObject;
};


#endif //TABLE_MENU_H
