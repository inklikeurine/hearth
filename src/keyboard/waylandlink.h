// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "compositorlink.h"

#include <QQuickWindow>

#include <memory>

// The keyboard's link to labwc: input-method v2 for the text box in focus, what's
// typed and the keys, and virtual-keyboard v1 for key presses.
class WaylandLink : public CompositorLink {
    Q_OBJECT

public:
    explicit WaylandLink(QObject *parent = nullptr);
    ~WaylandLink() override;

    // Takes the keyboard's job. Returns why it can't, or nothing.
    QString start();

    // Makes a window the strip along the bottom of the screen, above the apps.
    // Before it's first shown.
    static void makeStrip(QQuickWindow *window);

    void type(const QString &text) override;
    void deleteBefore(int bytes) override;
    void pressEnter() override;
    void passOn(uint32_t code, bool pressed) override;
    void setGrab(bool on) override;

private:
    class Private;
    std::unique_ptr<Private> d;
};
