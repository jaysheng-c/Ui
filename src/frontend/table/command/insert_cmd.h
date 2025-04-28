/**
  ********************************************************************************
  * @file           : insert_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef INSERT_CMD_H
#define INSERT_CMD_H

#include "table_cmd.h"

class InsertCmd : public TableCmd {
public:
    explicit InsertCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};


#endif //INSERT_CMD_H
