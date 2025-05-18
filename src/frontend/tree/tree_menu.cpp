/**
  ********************************************************************************
  * @file           : tree_menu.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#include "tree_menu.h"
#include "tree_view.h"

TreeMenu::TreeMenu(TreeView *parent)
    : QMenu(parent), m_treeView(parent)
{
}
