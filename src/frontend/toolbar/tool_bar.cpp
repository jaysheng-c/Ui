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
    this->setStyleSheet("QToolBar QToolButton {"
                        " border-radius: 5px;"
                        " padding: 3px;"
                        " color: rgb(209, 224, 228);"
                        " font: 10pt;"
                        "}"
                        "QToolBar QToolButton:hover {"
                        " background-color: rgb(65, 66, 69);"
                        "}");
    this->setMinimumHeight(50);
    this->addAction("菜单");
}

void ToolBar::paintEvent(QPaintEvent *event)
{
    QToolBar::paintEvent(event);
    auto rect = this->rect();
    rect.adjust(1, 0, -1, 0);
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(60, 63, 65)));
    painter.drawRect(rect);
}
