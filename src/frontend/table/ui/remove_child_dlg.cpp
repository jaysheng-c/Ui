/**
  ********************************************************************************
  * @file           : remove_child_dlg.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/08
  * @version        : 1.0
  ********************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_remove_child_dlg.h" resolved

#include "remove_child_dlg.h"
#include "ui_remove_child_dlg.h"
#include "table/table_global.h"

RemoveChildDlg::RemoveChildDlg(QWidget *parent) : QDialog(parent), ui(new Ui::RemoveChildDlg)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    this->setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    this->setWindowTitle("删除文档");
    ui->move_left_radio->setChecked(true);

    (void) connect(ui->ok_btn, &QPushButton::clicked, this, [this]() {
        if (ui->move_left_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::MoveLeft));
        } else if (ui->move_up_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::MoveUp));
        } else if (ui->remove_row_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::RemoveRow));
        } else {
            done(static_cast<int>(Table::TypeFlag::RemoveColumn));
        }
    });
    (void) connect(ui->cancel_btn, &QPushButton::clicked, this, [this]() {
        done(static_cast<int>(Table::TypeFlag::None));
    });
}

RemoveChildDlg::~RemoveChildDlg()
{
    delete ui;
}
