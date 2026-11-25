// SPDX-License-Identifier: GPL-3.0-or-later
#include "shellsettings.h"

#include <algorithm>

using namespace Qt::StringLiterals;

ShellSettings::ShellSettings(QObject *parent)
    : QObject(parent)
    , m_settings(QSettings::IniFormat, QSettings::UserScope, u"hearth"_s, u"shell"_s)
{
}

int ShellSettings::columns() const
{
    return std::clamp(m_settings.value(u"home/columns"_s, 5).toInt(), 3, 7);
}

void ShellSettings::setColumns(int columns)
{
    save(u"home/columns"_s, std::clamp(columns, 3, 7));
}

int ShellSettings::rows() const
{
    return std::clamp(m_settings.value(u"home/rows"_s, 1).toInt(), 1, 2);
}

void ShellSettings::setRows(int rows)
{
    save(u"home/rows"_s, std::clamp(rows, 1, 2));
}

QUrl ShellSettings::background() const
{
    const QString path = m_settings.value(u"home/background"_s).toString();
    return path.isEmpty() ? QUrl() : QUrl::fromLocalFile(path);
}

void ShellSettings::setBackground(const QUrl &background)
{
    save(u"home/background"_s, background.toLocalFile());
}

QStringList ShellSettings::appOrder() const
{
    return m_settings.value(u"home/order"_s).toStringList();
}

void ShellSettings::setAppOrder(const QStringList &order)
{
    save(u"home/order"_s, order);
}

void ShellSettings::save(const QString &key, const QVariant &value)
{
    if (m_settings.value(key) != value) {
        m_settings.setValue(key, value);
        Q_EMIT changed();
    }
}
