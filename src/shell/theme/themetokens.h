// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QColor>
#include <QObject>
#include <QStringList>
#include <QtQml/qqmlregistration.h>

class QJsonObject;

// The colours every screen uses.
struct ThemeColors {
    Q_GADGET
    QML_VALUE_TYPE(themeColors)
    Q_PROPERTY(QColor background MEMBER background)
    Q_PROPERTY(QColor surface MEMBER surface)
    Q_PROPERTY(QColor text MEMBER text)
    Q_PROPERTY(QColor textDim MEMBER textDim)
    Q_PROPERTY(QColor accent MEMBER accent)
    Q_PROPERTY(QColor textOnAccent MEMBER textOnAccent)
    Q_PROPERTY(QColor focusRing MEMBER focusRing)
    Q_PROPERTY(QColor outline MEMBER outline)

public:
    QColor background;
    QColor surface;
    QColor text;
    QColor textDim;
    QColor accent;
    QColor textOnAccent;
    QColor focusRing;
    QColor outline;
};

// Sizes in pixels. A theme file gives them for a 1920x1080 screen.
struct ThemeSizes {
    Q_GADGET
    QML_VALUE_TYPE(themeSizes)
    Q_PROPERTY(int textSmall MEMBER textSmall)
    Q_PROPERTY(int textBody MEMBER textBody)
    Q_PROPERTY(int textTitle MEMBER textTitle)
    Q_PROPERTY(int safeVertical MEMBER safeVertical)
    Q_PROPERTY(int safeSide MEMBER safeSide)
    Q_PROPERTY(int gap MEMBER gap)
    Q_PROPERTY(int radius MEMBER radius)
    Q_PROPERTY(int focusRing MEMBER focusRing)
    Q_PROPERTY(int outline MEMBER outline)

public:
    int textSmall = 0;
    int textBody = 0;
    int textTitle = 0;
    int safeVertical = 0;
    int safeSide = 0;
    int gap = 0;
    int radius = 0;
    int focusRing = 0;
    int outline = 0;
};

// How things move.
struct ThemeMotion {
    Q_GADGET
    QML_VALUE_TYPE(themeMotion)
    Q_PROPERTY(int duration MEMBER duration)
    Q_PROPERTY(qreal focusScale MEMBER focusScale)
    Q_PROPERTY(QList<qreal> easing MEMBER easing)

public:
    int duration = 0;
    qreal focusScale = 1.0;
    // Material 3's standard curve, as QML's Easing.BezierSpline wants it.
    QList<qreal> easing { 0.2, 0.0, 0.0, 1.0, 1.0, 1.0 };
};

// One theme, ready for screens to use. Reads a theme file, which is data only:
// every value is checked against the tokens above, anything else is ignored, and
// a theme whose text can't be read is refused. A theme lists only what it
// changes; the dark theme fills in the rest.
class ThemeTokens : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(ThemeColors colors READ colors NOTIFY changed)
    Q_PROPERTY(ThemeSizes sizes READ sizes NOTIFY changed)
    Q_PROPERTY(ThemeMotion motion READ motion NOTIFY changed)
    Q_PROPERTY(QString fontFamily READ fontFamily NOTIFY changed)
    Q_PROPERTY(qreal textScale READ textScale WRITE setTextScale NOTIFY changed)
    Q_PROPERTY(bool reducedMotion READ reducedMotion WRITE setReducedMotion NOTIFY changed)
    Q_PROPERTY(int screenHeight READ screenHeight WRITE setScreenHeight NOTIFY changed)

public:
    explicit ThemeTokens(QObject *parent = nullptr);

    // The theme in use. Screens get it unless something above them sets another.
    static ThemeTokens *live();

    // Loads a theme file. If it's refused, nothing changes and it returns false.
    Q_INVOKABLE bool loadFile(const QString &path);
    Q_INVOKABLE bool loadBuiltin(const QString &name);
    // What the last load ignored, corrected or refused.
    [[nodiscard]] QStringList problems() const
    {
        return m_problems;
    }

    [[nodiscard]] ThemeColors colors() const
    {
        return m_colors;
    }
    [[nodiscard]] ThemeSizes sizes() const;
    [[nodiscard]] ThemeMotion motion() const;
    [[nodiscard]] QString fontFamily() const
    {
        return m_fontFamily;
    }

    // The user's settings, kept when the theme changes.
    [[nodiscard]] qreal textScale() const
    {
        return m_textScale;
    }
    void setTextScale(qreal scale);
    [[nodiscard]] bool reducedMotion() const
    {
        return m_reducedMotion;
    }
    void setReducedMotion(bool reduced);
    [[nodiscard]] int screenHeight() const
    {
        return m_screenHeight;
    }
    void setScreenHeight(int height);

    // The built-in themes' names, as loadBuiltin() takes them.
    Q_INVOKABLE static QStringList builtinThemes();
    // WCAG 2.2's contrast ratio, from 1 to 21.
    static qreal contrast(const QColor &a, const QColor &b);

Q_SIGNALS:
    void changed();

private:
    bool loadJson(const QByteArray &json);
    void readInto(const QJsonObject &theme, ThemeColors &colors, ThemeSizes &sizes,
        ThemeMotion &motion, QString &fontFamily);
    bool readable(const ThemeColors &colors, const ThemeSizes &sizes);

    ThemeColors m_colors;
    ThemeSizes m_sizes;
    ThemeMotion m_motion;
    QString m_fontFamily;
    qreal m_textScale = 1.0;
    bool m_reducedMotion = false;
    int m_screenHeight = 1080;
    QStringList m_problems;
};
