// SPDX-License-Identifier: GPL-3.0-or-later
#include <KLocalizedQmlContext>

#include <QQmlEngine>
#include <QtQuickTest/quicktest.h>

// Sets the engine up as the keyboard does.
class Setup : public QObject {
    Q_OBJECT

public Q_SLOTS:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        KLocalization::setupLocalizedContext(engine);
    }
};

QUICK_TEST_MAIN_WITH_SETUP(keyboard, Setup)
#include "main.moc"
