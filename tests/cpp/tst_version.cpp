// SPDX-License-Identifier: GPL-3.0-or-later
#include "version.h"

#include <QTest>

class TestVersion : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void matchesTheBuild()
    {
        QCOMPARE(hearthVersion(), QStringLiteral(EXPECTED_VERSION));
    }
};

QTEST_GUILESS_MAIN(TestVersion)
#include "tst_version.moc"
