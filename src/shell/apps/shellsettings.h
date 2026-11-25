// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QSettings>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

// This profile's home screen settings. Each profile is its own Unix user, so
// each has its own file: ~/.config/hearth/shell.ini.
class ShellSettings : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY changed)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY changed)
    Q_PROPERTY(QUrl background READ background WRITE setBackground NOTIFY changed)
    Q_PROPERTY(QStringList appOrder READ appOrder WRITE setAppOrder NOTIFY changed)

public:
    explicit ShellSettings(QObject *parent = nullptr);

    [[nodiscard]] int columns() const;
    void setColumns(int columns);
    [[nodiscard]] int rows() const;
    void setRows(int rows);
    // A still picture, or empty for the theme's plain background.
    [[nodiscard]] QUrl background() const;
    void setBackground(const QUrl &background);
    // The apps' IDs in the order the profile arranged them.
    [[nodiscard]] QStringList appOrder() const;
    void setAppOrder(const QStringList &order);

Q_SIGNALS:
    void changed();

private:
    void save(const QString &key, const QVariant &value);

    QSettings m_settings;
};
