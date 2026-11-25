// SPDX-License-Identifier: GPL-3.0-or-later
#include "version.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("hearth-shell"));
    QGuiApplication::setApplicationVersion(hearthVersion());

    QQmlApplicationEngine engine;
    engine.loadFromModule("Hearth.Shell", "Main");
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    return QGuiApplication::exec();
}
