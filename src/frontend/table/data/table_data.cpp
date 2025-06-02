/**
  ********************************************************************************
  * @file           : table_data.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/13
  * @version        : 1.0
  ********************************************************************************
  */

#include "table_data.h"
#include <QVariant>
#include <QDebug>
#include <QColor>
#include <QFontComboBox>
#include <QBuffer>

namespace {

constexpr int BOLD = 0;
constexpr int ITALIC = 1;
constexpr int UNDERLINE = 2;


quint16 fontFamilyIndex(const QString &text)
{
    static QFontComboBox gFontComboBox;
    const int index = gFontComboBox.findText(text);
    if (index != -1 && index < gFontComboBox.count()) {
        return index;
    }
    return 0;
}

QString fontFamilyString(const int index)
{
    static QFontComboBox gFontComboBox;
    if (index < 0 || index >= gFontComboBox.count()) {
        return gFontComboBox.itemText(0);
    }
    return gFontComboBox.itemText(index);
}

QVector<bool> fontType(const int type)
{
    switch (type) {
        case TableData::Bold:                                           return {true, false, false};
        case TableData::Italic:                                         return {false, true, false};
        case TableData::Underline:                                      return {false, false, true};
        case TableData::Bold | TableData::Italic:                       return {true, true, false};
        case TableData::Bold | TableData::Underline:                    return {true, false, true};
        case TableData::Italic | TableData::Underline:                  return {false, true, true};
        case TableData::Bold |TableData::Italic | TableData::Underline: return {true, true, true};
        case TableData::None:
        default:                                                        return {false, false, false};
    }
}

TableData::FontType fontType(const QVector<bool> &types)
{
    Q_ASSERT(types.size() == 3);
    int type = TableData::FontType::None;
    type |= types.at(BOLD) ? TableData::Bold : TableData::None;
    type |= types.at(ITALIC) ? TableData::Italic : TableData::None;
    type |= types.at(UNDERLINE) ? TableData::Underline : TableData::None;
    return static_cast<TableData::FontType>(type);
}

int toQtAlign(const int align)
{
    switch (align) {
        case TableData::Left:                           return Qt::AlignLeft;
        case TableData::Right:                          return Qt::AlignRight;
        case TableData::HCenter:                        return Qt::AlignHCenter;
        case TableData::Top:                            return Qt::AlignTop;
        case TableData::Top | TableData::Left:          return Qt::AlignTop | Qt::AlignLeft;
        case TableData::Top | TableData::Right:         return Qt::AlignTop | Qt::AlignRight;
        case TableData::Top | TableData::HCenter:       return Qt::AlignTop | Qt::AlignHCenter;
        case TableData::Bottom:                         return Qt::AlignBottom;
        case TableData::Bottom | TableData::Left:       return Qt::AlignBottom | Qt::AlignLeft;
        case TableData::Bottom | TableData::Right:      return Qt::AlignBottom | Qt::AlignRight;
        case TableData::Bottom | TableData::HCenter:    return Qt::AlignBottom | Qt::AlignHCenter;
        case TableData::VCenter:                        return Qt::AlignVCenter;
        case TableData::VCenter | TableData::Left:      return Qt::AlignVCenter | Qt::AlignLeft;
        case TableData::VCenter | TableData::Right:     return Qt::AlignVCenter | Qt::AlignRight;
        case TableData::Center:
        default:
            break;
    }
    return Qt::AlignCenter;

}

int fromQtAlign(const int align)
{
    switch (align) {
        case Qt::AlignLeft:                         return TableData::Left;
        case Qt::AlignRight:                        return TableData::Right;
        case Qt::AlignHCenter:                      return TableData::HCenter;
        case Qt::AlignTop:                          return TableData::Top;
        case Qt::AlignTop | Qt::AlignLeft:          return TableData::Top | TableData::Left;
        case Qt::AlignTop | Qt::AlignRight:         return TableData::Top | TableData::Right;
        case Qt::AlignTop | Qt::AlignHCenter:       return TableData::Top | TableData::HCenter;
        case Qt::AlignBottom:                       return TableData::Bottom;
        case Qt::AlignBottom | Qt::AlignLeft:       return TableData::Bottom | TableData::Left;
        case Qt::AlignBottom | Qt::AlignRight:      return TableData::Bottom | TableData::Right;
        case Qt::AlignBottom | Qt::AlignHCenter:    return TableData::Bottom | TableData::HCenter;
        case Qt::AlignVCenter:                      return TableData::VCenter;
        case Qt::AlignVCenter | Qt::AlignLeft:      return TableData::VCenter | TableData::Left;
        case Qt::AlignVCenter | Qt::AlignRight:     return TableData::VCenter | TableData::Right;
        case Qt::AlignCenter:
        default:
            break;
    }
    return TableData::Center;
}

}

TableData::TableData(const QString &value)
        : m_value(value), m_display(value), m_align(VCenter | Left), m_foreground(4278190080), m_background(0),
          m_family(0), m_fontType(None), m_fontSize(11)
{

}

TableData::TableData(const TableData &data)
{
    m_value = data.m_value;
    m_display = data.m_display;
    m_align = data.m_align;
    m_foreground = data.m_foreground;
    m_background = data.m_background;
    m_family = data.m_family;
    m_fontType = data.m_fontType;
    m_fontSize = data.m_fontSize;
}

TableData::TableData(TableData &&data) noexcept
{
    m_value = std::move(data.m_value);
    m_display = std::move(data.m_display);
    m_align = data.m_align;
    m_foreground = data.m_foreground;
    m_background = data.m_background;
    m_family = data.m_family;
    m_fontType = data.m_fontType;
    m_fontSize = data.m_fontSize;
}

QVariant TableData::data(const TableData::Type type) const
{
    switch (type) {
        case Real:          return m_value;
        case Display:       return m_display;
        case Align:         return toQtAlign(m_align);
        case Foreground:    return QColor(m_foreground);
        case Background: {
            QColor color;
            color.setRgba(m_background);
            return QBrush(color);
        }
        case Font: {
            QFont font;
            font.setFamily(fontFamilyString(m_family));
            const auto list = fontType(m_fontType);
            font.setBold(list.at(0));
            font.setItalic(list.at(1));
            font.setUnderline(list.at(2));
            font.setPointSizeF(m_fontSize);
            return font;
        }
    }
    return {};
}

bool TableData::setData(const TableData::Type type, const QVariant &data)
{
    switch (type) {
        case Real:          m_value = data.toString();
        case Display:       m_display = data.toString(); break;
        case Align:         m_align = fromQtAlign(data.toInt()); break;
        case Foreground:    m_foreground = data.value<QColor>().rgb(); break;
        case Background:    m_background = data.value<QBrush>().color().rgba(); break;
        case Font: {
            const auto font = data.value<QFont>();
            m_family = fontFamilyIndex(font.family());
            m_fontType = fontType(QVector<bool>() << font.bold() << font.italic() << font.underline());
            m_fontSize = static_cast<float>(font.pointSizeF());
            break;
        }
        default:
            return false;
    }
    return true;
}

bool TableData::setData(const TableData::Type type, QVariant &&data)
{
    switch (type) {
        case Real:          m_value = data.toString(); break;
        case Display:       m_display = data.toString(); break;
        case Align:         m_align = fromQtAlign(data.toInt()); break;
        case Foreground:    m_foreground = data.value<QColor>().rgb(); break;
        case Background:    m_background = data.value<QBrush>().color().rgba(); break;
        case Font: {
            const auto font = data.value<QFont>();
            m_family = fontFamilyIndex(font.family());
            m_fontType = fontType(QVector<bool>() << font.bold() << font.italic() << font.underline());
            m_fontSize = static_cast<float>(font.pointSizeF());
            break;
        }
        default:
            return false;
    }
    return true;
}

void TableData::reset()
{
    m_value = "";
    m_display = "";
    m_align = TableData::VCenter | TableData::Left;
    m_foreground = QColor(Qt::black).rgb();
    m_background = QColor(Qt::transparent).rgba();
    m_family = fontFamilyIndex("Arial");
    m_fontType = TableData::None;
    m_fontSize = 11;
}

QByteArray TableData::serializeData(bool compress, int compressionLevel) const
{
    QByteArray serializedData;
    QBuffer buffer(&serializedData);
    buffer.open(QBuffer::WriteOnly);
    QDataStream out(&buffer);
    out << *this;
    buffer.close();
    if (compress) {
        return qCompress(serializedData, compressionLevel);
    }
    return serializedData;
}

bool TableData::deserializeData(const QByteArray &data, const bool decompress)
{
    auto decompressedData = decompress ? qUncompress(data) : data;
    QBuffer buffer(&decompressedData);
    if (buffer.open(QBuffer::ReadOnly)) {
        QDataStream in(&buffer);
        in >> *this;
        buffer.close();
        return true;
    }
    return false;
}

bool TableData::equal(const TableData &other, const QVector<Type> &types) const
{
    if (types.isEmpty()) {
        return *this == other;
    }
    bool res = true;
    for (const auto type: types) {
        res &= equal(other, type);
    }
    return res;
}

bool TableData::equal(const TableData &other, const TableData::Type type) const
{
    switch (type) {
        case Real:          return m_value == other.m_value;
        case Display:       return m_display == other.m_display;
        case Align:         return m_align == other.m_align;
        case Foreground:    return m_foreground == other.m_foreground;
        case Background:    return m_background == other.m_background;
        case Font:
            return m_family == other.m_family && m_fontType == other.m_fontType
                && qFuzzyCompare(m_fontSize, other.m_fontSize);
    }
    return false;
}

bool TableData::operator==(const TableData &other) const
{
    return m_value == other.m_value && m_display == other.m_display && m_align == other.m_align &&
           m_foreground == other.m_foreground && m_background == other.m_background && m_family == other.m_family &&
           m_fontType == other.m_fontType && qFuzzyCompare(m_fontSize, other.m_fontSize);
}

bool TableData::operator!=(const TableData &other) const
{
    return !(*this == other);
}

TableData &TableData::operator=(const TableData &data)
{
    if (this != &data) {
        m_value = data.m_value;
        m_display = data.m_display;
        m_align = data.m_align;
        m_foreground = data.m_foreground;
        m_background = data.m_background;
        m_family = data.m_family;
        m_fontType = data.m_fontType;
        m_fontSize = data.m_fontSize;
    }
    return *this;
}

TableData &TableData::operator=(TableData &&data) noexcept
{
    if (this != &data) {
        m_value = std::move(data.m_value);
        m_display = std::move(data.m_display);
        m_align = data.m_align;
        m_foreground = data.m_foreground;
        m_background = data.m_background;
        m_family = data.m_family;
        m_fontType = data.m_fontType;
        m_fontSize = data.m_fontSize;
    }
    return *this;
}

bool TableData::operator<(const TableData &other) const
{
    return qHash(*this) < qHash(other);
}

QDebug operator<<(QDebug debug, const TableData &obj)
{
    QColor background;
    background.setRgba(obj.m_background);
    debug.nospace() << "{\n [Real: " << obj.m_value << "]\n" << " [Display: " << obj.m_display << "]\n"
                    << " [Align: " << static_cast<Qt::AlignmentFlag>(toQtAlign(obj.m_align)) << "]\n"
                    << " [Foreground: " << QColor(obj.m_foreground).name() << "]\n"
                    << " [Background: " << QBrush(background) << "]\n"
                    << " [Font(family:" << fontFamilyString(obj.m_family)
                    << ", \"bold, italic, underline\": " << fontType(obj.m_fontType) << ", size:" << obj.m_fontSize
                    << ")]\n}";
    return debug.space();
}

QDebug operator<<(QDebug debug, const TableData *obj)
{
    if (obj == nullptr) {
        return debug.nospace() << "0x0";
    } else {
        QColor background;
        background.setRgba(obj->m_background);
        debug.nospace() << "{\n [Real: " << obj->m_value << "]\n" << " [Display: " << obj->m_display << "]\n"
                        << " [Align: " << static_cast<Qt::AlignmentFlag>(toQtAlign(obj->m_align)) << "]\n"
                        << " [Foreground: " << QColor(obj->m_foreground).name() << "]\n"
                        << " [Background: " << QBrush(background) << "]\n"
                        << " [Font(family:" << fontFamilyString(obj->m_family)
                        << ", \"bold, italic, underline\": " << fontType(obj->m_fontType) << ", size:"
                        << obj->m_fontSize << ")]\n}";
    }
    return debug.space();
}

QDataStream &operator<<(QDataStream &out, const TableData &obj)
{
    out << obj.m_value << obj.m_display << obj.m_align << obj.m_foreground
        << obj.m_background << obj.m_family << obj.m_fontType << obj.m_fontSize;
    return out;
}

QDataStream &operator>>(QDataStream & in, TableData & obj)
{
    float fontSize;
    quint8 align, type;
    QRgb foreground, background;
    quint16 family;
    in >> obj.m_value >> obj.m_display
       >> align >> foreground >> background
       >> family >> type >> fontSize;

    obj.m_align = align;
    obj.m_foreground = foreground;
    obj.m_background = background;
    obj.m_family = family;
    obj.m_fontType = type;
    obj.m_fontSize = fontSize;
    return in;
}

size_t qHash(const TableData &obj)
{
    return qHash(&obj);
}

size_t qHash(const TableData *obj)
{
    auto hash = qHash(obj->m_display + "dmsas" + obj->m_value);
    hash ^= static_cast<quint64>(obj->m_align) * 31;
    hash ^= static_cast<quint64>(obj->m_foreground) * 37;
    hash ^= static_cast<quint64>(obj->m_background) * 41;
    hash ^= static_cast<quint64>(obj->m_family) * 43;
    hash ^= static_cast<quint64>(obj->m_fontType) * 47;
    hash ^= static_cast<quint64>(obj->m_fontSize) * 53;
    return hash;
}
