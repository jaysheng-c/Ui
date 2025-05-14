/**
  ********************************************************************************
  * @file           : clear_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/14
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef CLEAR_CMD_H
#define CLEAR_CMD_H

#include "table_cmd.h"

class ClearCmd final : public TableCmd {
public:
    explicit ClearCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selection) override;
};



#endif //CLEAR_CMD_H
