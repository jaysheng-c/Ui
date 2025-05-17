/**
  ********************************************************************************
  * @file           : main_window.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#include "main_window.h"
#include "toolbar/tool_bar.h"
#include "title/title_widget.h"
#include "table/table_view.h"

#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setMenuWidget(new TitleWidget(this));
    this->addToolBar(Qt::TopToolBarArea, new ToolBar(this));

    qDebug() << qApp->metaObject()->className();
}