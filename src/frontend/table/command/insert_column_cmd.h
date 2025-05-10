/**
  ********************************************************************************
  * @file           : insert_column_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef INSERT_COLUMN_CMD_H
#define INSERT_COLUMN_CMD_H

#include "table_cmd.h"

class InsertColumnCmd final : public TableCmd {
public:
  explicit InsertColumnCmd(TableView *table);
  void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //INSERT_COLUMN_CMD_H
