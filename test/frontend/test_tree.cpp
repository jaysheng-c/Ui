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
#include "frontend/tree/tree_model.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TreeView w;
    QFile file(":/file/tree_view.json");
    (void) file.open(QIODevice::ReadOnly);
    if (auto *model = qobject_cast<TreeModel*>(w.model())) {
        model->resetData(file.readAll());
    }
    w.show();
    return QApplication::exec();
}