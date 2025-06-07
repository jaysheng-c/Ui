/**
  ********************************************************************************
  * @file           : definer.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef DEFINER_H
#define DEFINER_H

#if defined(__cplusplus) && __cplusplus >= 201703L
#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#else
#define NODISCARD
#define MAYBE_UNUSED
#endif


#define MIN_ROWS 100
#define MIN_COLUMNS 20

#endif //DEFINER_H
