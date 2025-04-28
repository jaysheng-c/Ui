/**
  ********************************************************************************
  * @file           : table_delegate.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef UI_TABLE_DELEGATE_H
#define UI_TABLE_DELEGATE_H

#include <QStyledItemDelegate>
class TableView;

class TableDelegate : public QStyledItemDelegate {
Q_OBJECT
public:
    explicit TableDelegate(TableView *table = nullptr);

public: // override
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};


#endif //UI_TABLE_DELEGATE_H
