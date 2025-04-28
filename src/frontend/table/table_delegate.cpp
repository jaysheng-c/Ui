/**
  ********************************************************************************
  * @file           : table_delegate.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_delegate.h"
#include <QLineEdit>

#include "data/table_data.h"
#include "table_view.h"

TableDelegate::TableDelegate(TableView *parent)
    : QStyledItemDelegate(parent)
{

}

QWidget *TableDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::createEditor(parent, option, index);
}

void TableDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (auto edit = qobject_cast<QLineEdit *>(editor)) {
        auto data = index.data(Qt::UserRole).value<TableData>();
        edit->setFont(data.data(TableData::Font).value<QFont>());
        edit->setAlignment(data.data(TableData::Align).value<Qt::Alignment>());
        edit->setStyleSheet(QString("color: %1;").arg(data.data(TableData::Foreground).value<QColor>().name()));
        edit->setText(data.data(TableData::Display).toString());
    }
    return QStyledItemDelegate::setEditorData(editor, index);
}

void TableDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    return QStyledItemDelegate::setModelData(editor, model, index);
}

void TableDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QStyledItemDelegate::paint(painter, option, index);
}

