/**
  ********************************************************************************
  * @file           : insert_row_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef INSERT_ROW_CMD_H
#define INSERT_ROW_CMD_H

#include "table_cmd.h"

class InsertRowCmd final : public TableCmd {
public:
    explicit InsertRowCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //INSERT_ROW_CMD_H
