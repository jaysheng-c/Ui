/**
  ********************************************************************************
  * @file           : move_bottom_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/05/17
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef MOVE_DOWN_CMD_H
#define MOVE_DOWN_CMD_H

#include "table_cmd.h"

class MoveDownCmd final : public TableCmd {
public:
  explicit MoveDownCmd(TableView *table);
  void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;
};



#endif //MOVE_DOWN_CMD_H
