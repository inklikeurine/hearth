// SPDX-License-Identifier: GPL-3.0-or-later
#include "iconprovider.h"
#include "version.h"

#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("hearth-shell"));
    QGuiApplication::setApplicationVersion(hearthVersion());

    KLocalizedString::setApplicationDomain(TRANSLATION_DOMAIN);

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);
    engine.addImageProvider(QStringLiteral("icon"), new IconProvider);
    engine.loadFromModule("Hearth.Shell", "Main");
    if (engine.rootObjects().isEmpty()) {
        return 1;
    }
    return QGuiApplication::exec();
}
