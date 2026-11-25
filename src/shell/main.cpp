// SPDX-License-Identifier: GPL-3.0-or-later
#include "iconprovider.h"
#include "labwcconfig.h"
#include "shellsettings.h"
#include "version.h"

#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    // hearth-shell --write-labwc FOLDER, before there's a screen to draw on.
    if (argc == 3 && qstrcmp(argv[1], "--write-labwc") == 0) {
        const QCoreApplication app(argc, argv);
        return LabwcConfig::write(ShellSettings(), QFile::decodeName(argv[2])) ? 0 : 1;
    }

    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("hearth-shell"));
    QGuiApplication::setApplicationVersion(hearthVersion());
    // Its windows' app ID, which AppSwitcher knows the home screen by.
    QGuiApplication::setDesktopFileName(QStringLiteral("hearth-shell"));

    KLocalizedString::setApplicationDomain(TRANSLATION_DOMAIN);

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.addImageProvider(QStringLiteral("icon"), new IconProvider);
    // hearth-session starts it with --session; without, it's a window on a desktop.
    engine.setInitialProperties({ { QStringLiteral("inSession"),
        QGuiApplication::arguments().contains(QStringLiteral("--session")) } });
    engine.loadFromModule("Hearth.Shell", "Main");
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    return QGuiApplication::exec();
}
