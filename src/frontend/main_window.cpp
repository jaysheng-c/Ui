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

#include <QApplication>

#include "main_tool_layout_obj.h"
#include "table/table_view.h"
#include "title/title_widget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_mainToolLayoutObj(new MainToolLayoutObj(this))
{
    this->setMenuWidget(new TitleWidget(this));

    qDebug() << qApp->metaObject()->className();

    m_mainToolLayoutObj->init();
    this->addToolBar(Qt::LeftToolBarArea, m_mainToolLayoutObj->toolBar());
    this->setCentralWidget(new TableView(this));
}