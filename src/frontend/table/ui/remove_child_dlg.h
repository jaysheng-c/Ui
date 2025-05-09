/**
  ********************************************************************************
  * @file           : remove_child_dlg.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/08
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef REMOVE_CHILD_DLG_H
#define REMOVE_CHILD_DLG_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class RemoveChildDlg; }
QT_END_NAMESPACE

class RemoveChildDlg final : public QDialog {
Q_OBJECT

public:
    explicit RemoveChildDlg(QWidget *parent = nullptr);
    ~RemoveChildDlg() override;

private:
    Ui::RemoveChildDlg *ui;
};


#endif //REMOVE_CHILD_DLG_H
