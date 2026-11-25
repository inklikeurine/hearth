// SPDX-License-Identifier: GPL-3.0-or-later
#include "applauncher.h"

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

class TestLauncher : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;
    AppList m_apps;

    void write(const QString &name, const QByteArray &exec)
    {
        QFile file(m_dir.filePath(name));
        if (!file.open(QIODevice::WriteOnly)
            || file.write("[Desktop Entry]\nType=Application\nName=" + name.toUtf8()
                   + "\nExec=" + exec + "\n")
                < 0) {
            qFatal("can't write the test's .desktop file");
        }
    }

private Q_SLOTS:
    void initTestCase()
    {
        write(u"1missing.desktop"_s, "/nowhere/at/all");
        write(u"2quick.desktop"_s, "true");
        write(u"3long.desktop"_s, "sleep 60");
        m_apps.setFolders({ m_dir.path() });
        m_apps.componentComplete();
        QCOMPARE(m_apps.count(), 3);
    }

    void saysWhenAnAppCantStart()
    {
        AppLauncher launcher;
        QSignalSpy failed(&launcher, &AppLauncher::failed);
        launcher.launch(&m_apps, 0);
        QTRY_COMPARE(failed.count(), 1);
        launcher.launch(&m_apps, 0);
        QTRY_COMPARE(failed.count(), 2);
    }

    void startsAndFinishes()
    {
        AppLauncher launcher;
        QSignalSpy started(&launcher, &AppLauncher::started);
        QSignalSpy finished(&launcher, &AppLauncher::finished);
        launcher.launch(&m_apps, 1);
        QTRY_COMPARE(finished.count(), 1);
        QCOMPARE(started.count(), 1);
    }

    void aRunningAppIsntStartedTwice()
    {
        AppLauncher launcher;
        QSignalSpy started(&launcher, &AppLauncher::started);
        launcher.launch(&m_apps, 2);
        QTRY_COMPARE(started.count(), 1);
        launcher.launch(&m_apps, 2);
        QTest::qWait(100);
        QCOMPARE(started.count(), 1);
    }

    void wrongIndexesDoNothing()
    {
        AppLauncher launcher;
        launcher.launch(&m_apps, -1);
        launcher.launch(&m_apps, 3);
        launcher.launch(nullptr, 0);
    }
};

QTEST_MAIN(TestLauncher)
#include "tst_launcher.moc"
