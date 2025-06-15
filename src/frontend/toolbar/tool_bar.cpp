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
#include <QPainter>

ToolBar::ToolBar(QWidget *parent)
    : QToolBar(parent)
{
    this->setObjectName("ToolBar");
    this->setFixedWidth(40);

    this->addAction("file");
}

void ToolBar::paintEvent(QPaintEvent *event)
{
    QToolBar::paintEvent(event);
    // auto rect = this->rect();
    // rect.adjust(0, 0, -1, 0);
    // QPainter painter(this);
    // painter.setPen(Qt::NoPen);
    // painter.setBrush(QBrush(QColor(43, 45, 48)));
    // painter.drawRect(rect);
}
