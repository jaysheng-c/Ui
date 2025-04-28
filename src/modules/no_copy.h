/**
  ********************************************************************************
  * @file           : no_copy.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef NO_COPY_H
#define NO_COPY_H

#if defined(__cplusplus) && __cplusplus >= 201703L
#define NODISCARD [[nodiscard]]
#else
#define NODISCARD
#endif

class NoCopy {
public:
    NoCopy() = default;
    virtual ~NoCopy() = default;

    NoCopy(const NoCopy&) = delete; // 禁止拷贝构造
    NoCopy(const NoCopy&&) = delete; // 禁止移动构造
    NoCopy& operator=(const NoCopy&) = delete; // 禁止赋值操作
    NoCopy& operator=(const NoCopy&&) = delete; // 禁止移动操作
};

#endif //NO_COPY_H
