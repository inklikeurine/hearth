// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QtQml/qqmlregistration.h>

// Takes the next key pressed on Hearth's own screens, for "press the button you
// want". Nothing else gets that key.
class KeyCatcher : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool catching READ catching WRITE setCatching NOTIFY catchingChanged)

public:
    explicit KeyCatcher(QObject *parent = nullptr);

    [[nodiscard]] bool catching() const
    {
        return m_catching;
    }
    void setCatching(bool catching);
    bool eventFilter(QObject *object, QEvent *event) override;

Q_SIGNALS:
    void catchingChanged();
    // The button's name in xkbcommon.
    void caught(const QString &key);
    // The button can't be given a meaning.
    void refused();

private:
    bool m_catching = false;
};
