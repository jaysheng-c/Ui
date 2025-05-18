/**
  ********************************************************************************
  * @file           : tree_view.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TREE_VIEW_H
#define TREE_VIEW_H

#include <QPointer>
#include <QTreeView>
#include "frontend/global/global.h"

class TreeModel;
class TreeMenu;

class UI_EXPORT TreeView : public QTreeView {
Q_OBJECT
public:
    explicit TreeView(QWidget *parent = nullptr);


private:
    QPointer<TreeModel> m_model;
    QPointer<TreeMenu> m_menu;
};



#endif //TREE_VIEW_H
