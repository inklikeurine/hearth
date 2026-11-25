// SPDX-License-Identifier: GPL-3.0-or-later
#include "applauncher.h"

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <csignal>

using namespace Qt::StringLiterals;

class TestLauncher : public QObject {
    Q_OBJECT

private:
    static DesktopEntry app(const QStringList &command)
    {
        return { u"app"_s, u"App"_s, { }, command, { } };
    }

private Q_SLOTS:
    void saysWhenAnAppCantStart()
    {
        AppLauncher launcher;
        QSignalSpy failed(&launcher, &AppLauncher::failed);
        launcher.start(app({ u"/nowhere/at/all"_s }));
        QTRY_COMPARE(failed.count(), 1);
        QVERIFY(launcher.running().isEmpty());
        launcher.start(app({ u"/nowhere/at/all"_s }));
        QTRY_COMPARE(failed.count(), 2);
    }

    void startsAndFinishes()
    {
        AppLauncher launcher;
        QSignalSpy started(&launcher, &AppLauncher::started);
        QSignalSpy finished(&launcher, &AppLauncher::finished);
        launcher.start(app({ u"true"_s }));
        QTRY_COMPARE(finished.count(), 1);
        QCOMPARE(started.count(), 1);
        QCOMPARE(finished.at(0), (QVariantList { u"app"_s, false, false }));
    }

    void tellsErrorsFromCrashes()
    {
        AppLauncher launcher;
        QSignalSpy finished(&launcher, &AppLauncher::finished);
        launcher.start(app({ u"false"_s }));
        QTRY_COMPARE(finished.count(), 1);
        QCOMPARE(finished.at(0), (QVariantList { u"app"_s, true, false }));
        launcher.start(app({ u"sh"_s, u"-c"_s, u"kill -KILL $$"_s }));
        QTRY_COMPARE(finished.count(), 2);
        QCOMPARE(finished.at(1), (QVariantList { u"app"_s, true, true }));
    }

    void aRunningAppIsntStartedTwice()
    {
        AppLauncher launcher;
        QSignalSpy started(&launcher, &AppLauncher::started);
        launcher.start(app({ u"sleep"_s, u"60"_s }));
        QTRY_COMPARE(started.count(), 1);
        launcher.start(app({ u"sleep"_s, u"60"_s }));
        QTest::qWait(100);
        QCOMPARE(started.count(), 1);
    }

    // Many apps are a script that starts the real program.
    void endingAnAppEndsWhatItStarted()
    {
        QTemporaryDir dir;
        const QString file = dir.filePath(u"pid"_s);
        AppLauncher launcher;
        launcher.start(app({ u"sh"_s, u"-c"_s, u"sleep 60 & echo $! > %1; wait"_s.arg(file) }));
        QByteArray pid;
        const auto readPid = [&pid, &file] {
            QFile written(file);
            pid = written.open(QIODevice::ReadOnly) ? written.readAll().trimmed() : QByteArray();
            return !pid.isEmpty();
        };
        QTRY_VERIFY(readPid());
        QCOMPARE(::kill(pid.toInt(), 0), 0);
        launcher.terminateAll();
        QTRY_VERIFY(launcher.running().isEmpty());
        // Gone, or dead and waiting for init to clear it away.
        QTRY_VERIFY(::kill(pid.toInt(), 0) != 0 || isZombie(pid));
    }

private:
    static bool isZombie(const QByteArray &pid)
    {
        QFile stat(u"/proc/%1/stat"_s.arg(QString::fromLatin1(pid)));
        return stat.open(QIODevice::ReadOnly) && stat.readAll().contains(") Z ");
    }
};

QTEST_MAIN(TestLauncher)
#include "tst_launcher.moc"
