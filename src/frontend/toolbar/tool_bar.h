/**
  ********************************************************************************
  * @file           : tool_bar.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/12
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TOOL_BAR_H
#define TOOL_BAR_H

#include <QToolBar>

class ToolBar : public QToolBar {
Q_OBJECT
signals:
    void triggered(bool checked, int type);
public:
    explicit ToolBar(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
};


#endif //TOOL_BAR_H
