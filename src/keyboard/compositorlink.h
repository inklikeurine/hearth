// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "keymap.h"

#include <QByteArray>
#include <QObject>
#include <QString>

// What the keyboard needs from the compositor. WaylandLink is the real one; tests
// use their own.
class CompositorLink : public QObject {
    Q_OBJECT

public:
    using QObject::QObject;

    // Puts text into the text box.
    virtual void type(const QString &text) = 0;
    // Deletes this many bytes of text before the cursor.
    virtual void deleteBefore(int bytes) = 0;
    virtual void pressEnter() = 0;
    // A grabbed key, for the app after all.
    virtual void passOn(uint32_t code, bool pressed) = 0;
    // While on, keys come here instead of the app.
    virtual void setGrab(bool on) = 0;

Q_SIGNALS:
    // The text box in focus: changes piece by piece, then done().
    void activated();
    void deactivated();
    void surroundingText(const QByteArray &text, qsizetype cursor, qsizetype anchor);
    void contentType(uint32_t hints, uint32_t purpose);
    void done();
    // Whether the app shows a cursor. labwc says so after each done().
    void cursorChanged(bool present);

    void keyPressed(const GrabbedKey &key);
    void keyReleased(uint32_t code);
    void repeatChanged(int rate, int delay);

    // The compositor gave the job to another keyboard, or took it away.
    void lost();
};
