/**
  ********************************************************************************
  * @file           : copy_data.cpp
  * @author         : jaysheng
  * @brief          : None
  * @attention      : None
  * @date           : 2025/04/25
  * @version        : 1.0
  ********************************************************************************
  */

#include "copy_data.h"

#include <QApplication>
#include <QClipboard>

#include "table/table_view.h"
#include "table/table_global.h"

#ifdef Q_OS_WIN
#include <windows.h>

HHOOK gWindowsHook {};

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && wParam == WM_KEYDOWN) {
        const auto *p = reinterpret_cast<KBDLLHOOKSTRUCT *>(lParam);
        const auto hwnd = GetForegroundWindow();  // 获取当前活动窗口
        if (const auto focusWnd = GetFocus(); focusWnd != hwnd && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
            // // 外部操作
            // if (p->vkCode == static_cast<int>('V')) {
            //     qDebug() << "OutApp Key down:" << static_cast<char>(p->vkCode);
            // } else if (p->vkCode == static_cast<int>('C')) {
            //     qDebug() << "OutApp Key down:" << static_cast<char>(p->vkCode);
            // }
            if (p->vkCode == static_cast<int>('C') || p->vkCode == static_cast<int>('X')) {
                CopyData::instance().clear();
            }
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
#endif

CopyData &CopyData::instance()
{
    static CopyData inst;
    return inst;
}

void CopyData::setData(const Table::TypeFlag opt, const Type copyTyp, const QVariant &data, const QVariant &range,
                       TableView *table)
{
    m_opt = opt;
    m_copyType = copyTyp;
    m_data = data;
    m_range = range;
    m_table = table;
    if (m_table) {
        const auto varList = data.value<QVector<QVariant>>();
        const auto selection = range.value<QItemSelectionRange>();
        if (varList.isEmpty() || selection.isEmpty()) {
            return;
        }
        // 将数据同步到系统剪切板
        const auto *model = m_table->model();
        const int col = selection.left();
        const int row = selection.top();
        QStringList rowStrs(selection.height(), {});
        QStringList colStrs(selection.width(), {});
        for (int r {0}; r < selection.height(); ++r) {
            for (int c {0}; c < selection.width(); ++c) {
                colStrs[c] = model->index(row + r, col + c).data(Qt::DisplayRole).toString();
            }
            rowStrs[r] = colStrs.join('\t');
        }
        QApplication::clipboard()->setText(rowStrs.join('\n') + '\n');
    }
}

void CopyData::clear()
{
    m_data.clear();
    m_range.clear();
}

CopyData::CopyData() : m_opt(Table::TypeFlag::None)
{
#ifdef Q_OS_WIN
    gWindowsHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
    if (!gWindowsHook) {
        qInfo() << "Unable to set hook.";
    }
#endif
}

CopyData::~CopyData()
{
#ifdef Q_OS_WIN
    UnhookWindowsHookEx(gWindowsHook);
#endif
}