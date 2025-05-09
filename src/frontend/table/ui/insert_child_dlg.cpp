/**
  ********************************************************************************
  * @file           : insert_child_dlg.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/08
  * @version        : 1.0
  ********************************************************************************
  */

// You may need to build the project (run Qt uic code generator) to get "ui_insert_child_dlg.h" resolved

#include "insert_child_dlg.h"
#include "ui_insert_child_dlg.h"
#include "table/table_global.h"


InsertChildDlg::InsertChildDlg(QWidget *parent) : QDialog(parent), ui(new Ui::InsertChildDlg)
{
    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowMinimizeButtonHint, false);
    this->setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    this->setWindowTitle("插入");
    ui->move_right_radio->setChecked(true);

    (void) connect(ui->ok_btn, &QPushButton::clicked, this, [this]() {
        if (ui->move_right_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::MoveRight));
        } else if (ui->move_down_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::MoveDown));
        } else if (ui->insert_row_radio->isChecked()) {
            done(static_cast<int>(Table::TypeFlag::InsertRow));
        } else {
            done(static_cast<int>(Table::TypeFlag::InsertColumn));
        }
    });
    (void) connect(ui->cancel_btn, &QPushButton::clicked, this, [this]() {
        done(static_cast<int>(Table::TypeFlag::None));
    });
}

InsertChildDlg::~InsertChildDlg()
{
    delete ui;
}
