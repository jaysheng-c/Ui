/**
  ********************************************************************************
  * @file           : paste_cmd.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef PASTE_CMD_H
#define PASTE_CMD_H

#include "table_cmd.h"
#include "global/macro.h"

namespace { struct Range; }


class PasteCmd : public TableCmd {
public:
    using DataPair = QPair<QVector<Range>, QVariantList>;
    explicit PasteCmd(TableView *table);
    void cmd(QObject *contextObject, const QItemSelection &selectionItem) override;

protected:
    NODISCARD DataPair data(const QVector<Range> &origin, int role) const;
};


#endif //PASTE_CMD_H
