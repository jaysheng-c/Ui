/**
  ********************************************************************************
  * @file           : move_right_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/14
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MOVE_RIGHT_CMD_H
#define MOVE_RIGHT_CMD_H

#include "table_cmd.h"

class MoveRightCmd final : public TableCmd {
public:
    explicit MoveRightCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //MOVE_RIGHT_CMD_H
