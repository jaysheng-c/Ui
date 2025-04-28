/**
  ********************************************************************************
  * @file           : transformer.h
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/10
  * @version        : 1.0
  ********************************************************************************
  */

#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include <QVariant>

#ifdef USE_BOOST
#include <boost/lexical_cast.hpp>
#endif

template<class F, class T>
class Transformer {
public:
    T operator()(const F &v) const
    {
#ifdef USE_BOOST
        return boost::leical_cast<T>(v);
#endif
        if (std::is_same<T, F>::value) {
            return v;
        }
        return {};
    }
};

template<class T>
class Transformer<QVariant, T> {
public:
    T operator()(const QVariant &v) const
    {
        if (std::is_same<T, int>::value || std::is_same<T, short>::value) {
            return v.toInt();
        } else if (std::is_same<T, uint>::value || std::is_same<T, ushort>::value) {
            return v.toUInt();
        } else if (std::is_same<T, qlonglong>::value || std::is_same<T, long>::value) {
            return v.toLongLong();
        } else if (std::is_same<T, qulonglong>::value || std::is_same<T, ulong>::value) {
            return v.toULongLong();
        } else if (std::is_same<T, float>::value) {
            return v.toFloat();
        } else if (std::is_same<T, double>::value) {
            return v.toDouble();
        } else if (std::is_same<T, QVariant>::value) {
            return v;
        }
        return v.value<T>();
    }
};

template<>
class Transformer<QString, QString> {
public:
    QString operator()(const QString &v) const
    {
        return v;
    }
};

template<class F>
class Transformer<F, QString> {
public:
    QString operator()(const F &v) const
    {
        if (std::is_arithmetic_v<F>) {
            return QString::number(v);
        }
        throw std::runtime_error("unsupported type" + std::string(typeid(F).name()));
    }
};

template<class T>
class Transformer<QString, T> {
public:
    T operator()(const QString &v) const
    {
        if (std::is_same<T, short>::value) {
            return v.toShort();
        } else if (std::is_same<T, ushort>::value) {
            return v.toUShort();
        } else if (std::is_same<T, int>::value) {
            return v.toInt();
        } else if (std::is_same<T, uint>::value) {
            return v.toUInt();
        } else if (std::is_same<T, long>::value) {
            return v.toLong();
        } else if (std::is_same<T, ulong>::value) {
            return v.toULong();
        } else if (std::is_same<T, qlonglong>::value) {
            return v.toLongLong();
        } else if (std::is_same<T, qulonglong>::value) {
            return v.toULongLong();
        } else if (std::is_same<T, float>::value) {
            return v.toFloat();
        } else if (std::is_same<T, double>::value) {
            return v.toDouble();
        } else if (std::is_same<T, bool>::value) {
            return v.toLower() == "true";
        }
        throw std::runtime_error("unsupported type" + std::string(typeid(T).name()));
    }
};


#endif //TRANSFORMER_H
