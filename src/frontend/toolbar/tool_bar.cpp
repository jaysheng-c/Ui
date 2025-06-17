/**
  ********************************************************************************
  * @file           : tool_bar.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */

#include "tool_bar.h"

#include <QLayout>
#include <QToolButton>

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
{
    this->setObjectName("ToolBar");
    this->layout()->setSpacing(8);
}

void ToolBar::paintEvent(QPaintEvent *event)
{
    QToolBar::paintEvent(event);
}
