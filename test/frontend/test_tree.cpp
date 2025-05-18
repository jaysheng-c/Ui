/**
  ********************************************************************************
  * @file           : test_tree.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/18
  * @version        : 1.0
  ********************************************************************************
  */

#include <QApplication>
#include "frontend/tree/tree_view.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TreeView w;
    w.show();
    return QApplication::exec();
}