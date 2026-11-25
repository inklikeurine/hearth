// SPDX-License-Identifier: GPL-3.0-or-later
#include "iconprovider.h"

#include <KLocalizedQmlContext>

#include <QQmlEngine>
#include <QtQuickTest>

// Sets the engine up as the shell does.
class Setup : public QObject {
    Q_OBJECT

public Q_SLOTS:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        KLocalization::setupLocalizedContext(engine);
        engine->addImageProvider(QStringLiteral("icon"), new IconProvider);
    }
};

QUICK_TEST_MAIN_WITH_SETUP(hearth, Setup)
#include "main.moc"
