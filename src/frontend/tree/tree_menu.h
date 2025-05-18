/**
  ********************************************************************************
  * @file           : tree_menu.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TREE_MENU_H
#define TREE_MENU_H

#include <QMenu>

class TreeView;

class TreeMenu : public QMenu {
Q_OBJECT
public:
    explicit TreeMenu(TreeView *parent);

private:
    TreeView *m_treeView;
};



#endif //TREE_MENU_H
