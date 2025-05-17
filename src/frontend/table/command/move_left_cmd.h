/**
  ********************************************************************************
  * @file           : move_left_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/17
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MOVE_LEFT_CMD_H
#define MOVE_LEFT_CMD_H

#include "table_cmd.h"

class MoveLeftCmd final : public TableCmd {
public:
    explicit MoveLeftCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //MOVE_LEFT_CMD_H
