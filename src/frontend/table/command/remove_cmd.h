/**
  ********************************************************************************
  * @file           : remove_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/23
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef REMOVE_CMD_H
#define REMOVE_CMD_H

#include "table_cmd.h"

class RemoveCmd : public TableCmd {
public:
    explicit RemoveCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};


#endif //REMOVE_CMD_H
