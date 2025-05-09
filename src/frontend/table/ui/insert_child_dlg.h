/**
  ********************************************************************************
  * @file           : insert_child_dlg.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/08
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef INSERT_CHILD_DLG_H
#define INSERT_CHILD_DLG_H

#include <QDialog>


QT_BEGIN_NAMESPACE
namespace Ui { class InsertChildDlg; }
QT_END_NAMESPACE

class InsertChildDlg final : public QDialog {
Q_OBJECT

public:
    explicit InsertChildDlg(QWidget *parent = nullptr);
    ~InsertChildDlg() override;

private:
    Ui::InsertChildDlg *ui;
};


#endif //INSERT_CHILD_DLG_H
