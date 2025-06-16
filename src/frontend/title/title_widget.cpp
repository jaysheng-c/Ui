/**
  ********************************************************************************
  * @file           : title_widget.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */

#include "title_widget.h"
#include <QHBoxLayout>

TitleWidget::TitleWidget(QWidget *parent)
    : QFrame(parent)
{
    this->setFixedHeight(40);
    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 3, 5, 3);
    layout->setSpacing(5);
}

void TitleWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    // auto rect = this->rect();
    // rect.adjust(0, 0, -1, 0);
    // QPainter painter(this);
    // painter.setPen(Qt::NoPen);
    // painter.setBrush(QBrush(QColor(60, 63, 65)));
    // painter.drawRect(rect);
}

