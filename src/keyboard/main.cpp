// SPDX-License-Identifier: GPL-3.0-or-later
//
// hearth-keyboard: the on-screen keyboard. hearth-session starts it next to the
// shell. It ends by itself only when it can't be the session's keyboard, with
// status 0 so that it isn't started again.
#include "keyboard.h"
#include "keyboardsettings.h"
#include "shelllink.h"
#include "waylandlink.h"

#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>

#include <cstdio>

namespace {

void sayWhy(const QString &reason)
{
    std::fprintf(stderr, "hearth-keyboard: no on-screen keyboard: %s\n", qPrintable(reason));
}

} // namespace

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("hearth-keyboard"));
    QGuiApplication::setDesktopFileName(QStringLiteral("hearth-keyboard"));
    // It's hidden most of the time.
    QGuiApplication::setQuitOnLastWindowClosed(false);
    KLocalizedString::setApplicationDomain(TRANSLATION_DOMAIN);

    WaylandLink link;
    KeyboardSettings settings;
    ShellLink shell(ShellLink::socketPath());
    // Listening before the link starts: a text box may have focus already.
    Keyboard keyboard(&link, &settings, &shell);
    const QString problem = link.start();
    if (!problem.isEmpty()) {
        sayWhy(problem);
        return 0;
    }
    QObject::connect(&link, &CompositorLink::lost, &app, [] {
        sayWhy(QStringLiteral("the compositor took it away"));
        QCoreApplication::exit(0);
    });

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.setInitialProperties({ { QStringLiteral("keyboard"), QVariant::fromValue(&keyboard) } });
    engine.loadFromModule("Hearth.Keyboard", "KeyboardWindow");
    auto *window = engine.rootObjects().isEmpty()
        ? nullptr
        : qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
    if (!window) {
        return 1;
    }
    WaylandLink::makeStrip(window);
    shell.start();
    return QGuiApplication::exec();
}
