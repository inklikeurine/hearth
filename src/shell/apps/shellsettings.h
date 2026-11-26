// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QSettings>
#include <QUrl>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

// This profile's home screen settings. Each profile is its own Unix user, so
// each has its own file: ~/.config/hearth/shell.ini.
class ShellSettings : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(int columns READ columns WRITE setColumns NOTIFY changed)
    Q_PROPERTY(int rows READ rows WRITE setRows NOTIFY changed)
    Q_PROPERTY(QUrl background READ background WRITE setBackground NOTIFY changed)
    Q_PROPERTY(QString theme READ theme WRITE setTheme NOTIFY changed)
    Q_PROPERTY(int textPercent READ textPercent WRITE setTextPercent NOTIFY changed)
    Q_PROPERTY(bool reducedMotion READ reducedMotion WRITE setReducedMotion NOTIFY changed)
    Q_PROPERTY(
        bool reduceTransparency READ reduceTransparency WRITE setReduceTransparency NOTIFY changed)
    Q_PROPERTY(
        int screensaverMinutes READ screensaverMinutes WRITE setScreensaverMinutes NOTIFY changed)
    // Buttons that mean Back, Home and Menu besides the usual ones, by their
    // names in xkbcommon: { back: [names], home: [names], menu: [names] }.
    Q_PROPERTY(QVariantMap extraKeys READ extraKeys WRITE setExtraKeys NOTIFY changed)
    Q_PROPERTY(RepeatSpeed repeatSpeed READ repeatSpeed WRITE setRepeatSpeed NOTIFY changed)
    Q_PROPERTY(bool ignoreDoublePresses READ ignoreDoublePresses WRITE setIgnoreDoublePresses NOTIFY
            changed)
    Q_PROPERTY(QStringList appOrder READ appOrder WRITE setAppOrder NOTIFY changed)

public:
    enum RepeatSpeed { Slow, Normal, Fast };
    Q_ENUM(RepeatSpeed)

    explicit ShellSettings(QObject *parent = nullptr);

    [[nodiscard]] int columns() const;
    void setColumns(int columns);
    [[nodiscard]] int rows() const;
    void setRows(int rows);
    // A still picture, or empty for the theme's plain background.
    [[nodiscard]] QUrl background() const;
    void setBackground(const QUrl &background);
    // A built-in theme's name, as ThemeTokens has it.
    [[nodiscard]] QString theme() const;
    void setTheme(const QString &theme);
    [[nodiscard]] int textPercent() const;
    void setTextPercent(int percent);
    [[nodiscard]] bool reducedMotion() const;
    void setReducedMotion(bool reduced);
    [[nodiscard]] bool reduceTransparency() const;
    void setReduceTransparency(bool reduce);
    // 0 is off.
    [[nodiscard]] int screensaverMinutes() const;
    void setScreensaverMinutes(int minutes);
    [[nodiscard]] QVariantMap extraKeys() const;
    void setExtraKeys(const QVariantMap &keys);
    [[nodiscard]] RepeatSpeed repeatSpeed() const;
    void setRepeatSpeed(RepeatSpeed speed);
    [[nodiscard]] bool ignoreDoublePresses() const;
    void setIgnoreDoublePresses(bool ignore);
    // Pictures there are to choose from: the ones Hearth ships, and the profile's.
    Q_INVOKABLE [[nodiscard]] QStringList backgroundChoices() const;
    // The apps' IDs in the order the profile arranged them.
    [[nodiscard]] QStringList appOrder() const;
    void setAppOrder(const QStringList &order);

Q_SIGNALS:
    void changed();

private:
    void save(const QString &key, const QVariant &value);

    QSettings m_settings;
};
