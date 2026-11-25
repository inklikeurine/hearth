// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QQuickWindow>
#include <QtQml/qqmlregistration.h>

// Puts a window above every app, full-screen ones too (wlr-layer-shell).
class LayerSurface : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    // Before the window is first shown. Outside a Wayland session it stays an
    // ordinary window. Only one that takes the keyboard gets any keys.
    Q_INVOKABLE static void setUp(QQuickWindow *window, const QString &name, bool takesKeyboard);
};
