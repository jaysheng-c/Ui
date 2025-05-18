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

    QTimer::singleShot(2000, [this]() { m_model->resetData(""); });

    // auto *model = new QStandardItemModel();
    //
    // // 创建根项
    // auto *rootItem = model->invisibleRootItem();
    //
    // // 添加一级节点
    // auto *item1 = new QStandardItem("一级节点1");
    // auto *item2 = new QStandardItem("一级节点2");
    // rootItem->appendRow(item1);
    // rootItem->appendRow(item2);
    //
    // // 添加二级节点
    // auto *subItem1 = new QStandardItem("二级节点1");
    // auto *subItem2 = new QStandardItem("二级节点2");
    // item1->appendRow(subItem1);
    // item1->appendRow(subItem2);
    //
    // // 设置模型到视图;
    // QTreeView::setModel(model);

}
