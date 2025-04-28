/**
  ********************************************************************************
  * @file           : title_widget.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TITLE_WIDGET_H
#define TITLE_WIDGET_H

#include <QWidget>


class TitleWidget : public QWidget {
Q_OBJECT
public:
    explicit TitleWidget(QWidget *parent = nullptr);

    void initSearchLine1();

protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif //TITLE_WIDGET_H
