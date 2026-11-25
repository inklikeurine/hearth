// SPDX-License-Identifier: GPL-3.0-or-later
//
// hearth-gallery [THEME] [PICTURE]: shows every control. With a picture's file
// name, saves the window there and quits.
#include "themetokens.h"

#include <QGuiApplication>
#include <QImage>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QTimer>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    const QStringList arguments = QGuiApplication::arguments();
    if (arguments.size() > 1 && !ThemeTokens::live()->loadBuiltin(arguments.at(1))) {
        qCritical("%s", qPrintable(ThemeTokens::live()->problems().join(QLatin1Char('\n'))));
        return 1;
    }

    QQmlApplicationEngine engine;
    engine.loadFromModule("Hearth.Gallery", "Gallery");
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    if (arguments.size() > 2) {
        QTimer::singleShot(1000, &app, [&engine, &arguments] {
            auto *window = qobject_cast<QQuickWindow *>(engine.rootObjects().constFirst());
            const bool saved = window && window->grabWindow().save(arguments.at(2));
            QCoreApplication::exit(saved ? 0 : 1);
        });
    }
    return QGuiApplication::exec();
}
