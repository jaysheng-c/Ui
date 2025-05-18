/**
  ********************************************************************************
  * @file           : tree_view.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#include "tree_view.h"

#include <QTimer>

#include "tree_model.h"
#include "tree_menu.h"

TreeView::TreeView(QWidget *parent)
    : QTreeView(parent), m_model(nullptr), m_menu(nullptr)
{
    m_model = new TreeModel(this);
    m_model->resetData("");
    QTreeView::setModel(m_model);
}
