// SPDX-License-Identifier: GPL-3.0-or-later
#include "appswitcher.h"

#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <algorithm>

using namespace Qt::StringLiterals;

// Windows as labwc would report them, with nothing on screen.
class PretendWindows : public Windows {
    Q_OBJECT

public:
    QList<WindowInfo> list;
    // Windows that ask about unsaved work instead of closing.
    QSet<int> stubborn;
    QList<int> madeFullscreen;
    // labwc takes a moment to do what it's asked: nothing changes until it has.
    bool slow = false;

    [[nodiscard]] QList<WindowInfo> windows() const override
    {
        return list;
    }
    void bringToFront(int id) override
    {
        if (slow) {
            return;
        }
        for (WindowInfo &window : list) {
            window.active = window.id == id;
            window.minimized = window.minimized && !window.active;
        }
        Q_EMIT changed();
    }
    void minimize(int id) override
    {
        for (WindowInfo &window : list) {
            if (window.id == id) {
                window.minimized = true;
                window.active = false;
            }
        }
        Q_EMIT changed();
    }
    void makeFullscreen(int id) override
    {
        madeFullscreen << id;
    }
    void close(int id) override
    {
        if (!stubborn.contains(id)) {
            remove(id);
        }
    }

    void add(int id, const QString &appId, bool hasParent = false)
    {
        for (WindowInfo &window : list) {
            window.active = false;
        }
        WindowInfo window;
        window.id = id;
        window.appId = appId;
        window.title = u"Title of "_s + appId;
        window.active = true;
        window.hasParent = hasParent;
        window.fullscreen = appId == u"hearth-shell"_s;
        list << window;
        Q_EMIT changed();
    }
    void remove(int id)
    {
        list.removeIf([id](const WindowInfo &window) { return window.id == id; });
        Q_EMIT changed();
    }
    [[nodiscard]] WindowInfo find(int id) const
    {
        const auto found = std::find_if(
            list.begin(), list.end(), [id](const WindowInfo &window) { return window.id == id; });
        return found == list.end() ? WindowInfo() : *found;
    }
};

class TestSwitcher : public QObject {
    Q_OBJECT

private:
    enum App { Long, Other, Missing, Failing, Crashing };

    QTemporaryDir m_dir;
    AppList m_apps;

    void write(const QString &name, const QByteArray &exec, const QByteArray &more = { })
    {
        QFile file(m_dir.filePath(name));
        if (!file.open(QIODevice::WriteOnly)
            || file.write("[Desktop Entry]\nType=Application\nName=" + name.toUtf8()
                   + "\nExec=" + exec + "\n" + more)
                < 0) {
            qFatal("can't write the test's .desktop file");
        }
    }

private Q_SLOTS:
    void initTestCase()
    {
        write(u"1long.desktop"_s, "sleep 60", "StartupWMClass=LongClass\n");
        write(u"2other.desktop"_s, "sleep 61");
        write(u"3missing.desktop"_s, "/nowhere/at/all");
        write(u"4failing.desktop"_s, "false");
        write(u"5crashing.desktop"_s, "sh -c \"sleep 0.3; kill -KILL \\\\$\\\\$\"");
        m_apps.setFolders({ m_dir.path() });
        m_apps.componentComplete();
        QCOMPARE(m_apps.count(), 5);
    }

    void init()
    {
        m_windows = std::make_unique<PretendWindows>();
        m_launcher = std::make_unique<AppLauncher>();
        m_switcher = std::make_unique<AppSwitcher>();
        m_switcher->setProperty("apps", QVariant::fromValue(&m_apps));
        m_switcher->setProperty("closeWait", 200);
        m_switcher->setLauncher(m_launcher.get());
        m_switcher->setWindows(m_windows.get());
        m_windows->add(1, u"hearth-shell"_s);
    }

    void cleanup()
    {
        m_switcher.reset();
        m_launcher.reset();
        m_windows.reset();
    }

    void startsAtHome()
    {
        QVERIFY(m_switcher->homeInFront());
        QCOMPARE(m_switcher->runningId(), QString());
    }

    void opensAnApp()
    {
        m_switcher->open(Long);
        QCOMPARE(m_switcher->openingName(), u"1long.desktop"_s);
        QCOMPARE(m_switcher->runningId(), u"1long"_s);
        // By its window's name, whatever its case.
        m_windows->add(2, u"longclass"_s);
        QCOMPARE(m_switcher->openingName(), QString());
        QCOMPARE(m_switcher->runningId(), u"1long"_s);
        QVERIFY(!m_switcher->homeInFront());
    }

    void aWindowWithAnUnknownNameIsTheAppThatWasOpened()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"org.example.Surprise"_s);
        QCOMPARE(m_switcher->runningId(), u"1long"_s);
        QCOMPARE(m_switcher->runningName(), u"1long.desktop"_s);
    }

    void homeHidesTheAppAndChoosingItReturnsToIt()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        QSignalSpy homeShown(m_switcher.get(), &AppSwitcher::homeShown);
        m_switcher->goHome();
        QVERIFY(m_switcher->homeInFront());
        QCOMPARE(homeShown.count(), 1);
        QVERIFY(m_windows->find(2).minimized);
        QCOMPARE(m_switcher->runningId(), u"1long"_s);

        m_switcher->open(Long);
        QVERIFY(!m_switcher->homeInFront());
        QVERIFY(!m_windows->find(2).minimized);
        QCOMPARE(m_launcher->running(), QStringList { u"1long"_s });
    }

    void anotherAppWaitsForTheFirstToClose()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_switcher->goHome();
        m_switcher->open(Other);
        QCOMPARE(m_switcher->openingName(), u"2other.desktop"_s);
        // The first has no window any more, but its program takes a while.
        QCOMPARE(m_windows->list.size(), 1);
        QCOMPARE(m_launcher->running(), QStringList { u"1long"_s });
        QTRY_COMPARE(m_launcher->running(), QStringList { u"2other"_s });
        m_windows->add(3, u"2other"_s);
        QCOMPARE(m_switcher->runningId(), u"2other"_s);
        QCOMPARE(m_switcher->openingName(), QString());
    }

    void anAppWithUnsavedWorkIsNeverEnded()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->stubborn << 2;
        QSignalSpy stillOpen(m_switcher.get(), &AppSwitcher::stillOpen);
        m_switcher->open(Other);
        QTRY_COMPARE(stillOpen.count(), 1);
        QCOMPARE(stillOpen.at(0).at(0).toString(), u"1long.desktop"_s);
        QTest::qWait(600);
        QCOMPARE(m_launcher->running(), QStringList { u"1long"_s });

        m_switcher->returnToApp();
        QVERIFY(!m_switcher->homeInFront());
        QCOMPARE(m_switcher->openingName(), QString());
    }

    void forceCloseEndsItAndOpensTheNext()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->stubborn << 2;
        QSignalSpy stillOpen(m_switcher.get(), &AppSwitcher::stillOpen);
        m_switcher->open(Other);
        QTRY_COMPARE(stillOpen.count(), 1);
        m_switcher->forceClose();
        // As labwc would when the program goes.
        QTRY_VERIFY(!m_launcher->running().contains(u"1long"_s));
        m_windows->remove(2);
        QTRY_COMPARE(m_launcher->running(), QStringList { u"2other"_s });
    }

    void closingLeavesTheAppsDialogsToThePerson()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->add(3, u"1long"_s, true);
        m_windows->stubborn << 2;
        m_switcher->closeApp();
        QCOMPARE(m_windows->find(3).id, 3);
    }

    void homeComesBackWhenTheAppCloses()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_switcher->closeApp();
        QTRY_VERIFY(m_launcher->running().isEmpty());
        QVERIFY(m_switcher->homeInFront());
        QCOMPARE(m_switcher->runningId(), QString());
    }

    void waitsForLabwcToShowHome()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->slow = true;
        m_windows->remove(2);
        QTRY_VERIFY(m_launcher->running().isEmpty());
        QVERIFY(!m_switcher->homeInFront());
        m_windows->slow = false;
        m_windows->bringToFront(1);
        QVERIFY(m_switcher->homeInFront());
    }

    void aProgramLeftBehindIsEnded()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->remove(2);
        QTRY_VERIFY(m_launcher->running().isEmpty());
    }

    void onlyWindowsWithNoParentAreMadeFullscreen()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        m_windows->add(3, u"1long"_s, true);
        QVERIFY(m_windows->madeFullscreen.contains(2));
        QVERIFY(!m_windows->madeFullscreen.contains(1));
        QVERIFY(!m_windows->madeFullscreen.contains(3));
    }

    // Otherwise the two would take turns for ever.
    void anAppThatLeavesFullscreenIsLetBe()
    {
        m_switcher->open(Long);
        m_windows->add(2, u"1long"_s);
        for (int i = 0; i < 20; ++i) {
            Q_EMIT m_windows->changed();
        }
        QCOMPARE(m_windows->madeFullscreen.count(2), 3);
    }

    void saysWhenAnAppCantStart()
    {
        QSignalSpy failed(m_switcher.get(), &AppSwitcher::couldNotStart);
        m_switcher->open(Missing);
        QTRY_COMPARE(failed.count(), 1);
        QCOMPARE(failed.at(0).at(1).toString(), u"3missing.desktop"_s);
        QCOMPARE(m_switcher->openingName(), QString());
    }

    void saysWhenAnAppEndsBeforeItShows()
    {
        QSignalSpy ended(m_switcher.get(), &AppSwitcher::endedEarly);
        m_switcher->open(Failing);
        QTRY_COMPARE(ended.count(), 1);
        QCOMPARE(ended.at(0).at(1).toString(), u"4failing.desktop"_s);
    }

    void saysWhenAnAppCrashes()
    {
        QSignalSpy ended(m_switcher.get(), &AppSwitcher::endedEarly);
        QSignalSpy crashed(m_switcher.get(), &AppSwitcher::crashed);
        m_switcher->open(Crashing);
        m_windows->add(2, u"5crashing"_s);
        QTRY_COMPARE(crashed.count(), 1);
        m_windows->remove(2);
        QCOMPARE(crashed.at(0).at(1).toString(), u"5crashing.desktop"_s);
        QCOMPARE(ended.count(), 0);
        QVERIFY(m_switcher->homeInFront());
    }

    void closingAnAppIsntAFailure()
    {
        QSignalSpy ended(m_switcher.get(), &AppSwitcher::endedEarly);
        QSignalSpy crashed(m_switcher.get(), &AppSwitcher::crashed);
        m_switcher->open(Long);
        m_switcher->closeApp();
        QTRY_VERIFY(m_launcher->running().isEmpty());
        QCOMPARE(ended.count(), 0);
        QCOMPARE(crashed.count(), 0);
    }

    // As after the shell restarts: the app is there, but this shell didn't start it.
    void knowsAnAppItDidntStart()
    {
        m_windows->add(2, u"2other"_s);
        QCOMPARE(m_switcher->runningId(), u"2other"_s);
        QCOMPARE(m_switcher->runningName(), u"2other.desktop"_s);
        m_switcher->goHome();
        m_switcher->open(Other);
        QVERIFY(!m_switcher->homeInFront());
        QVERIFY(m_launcher->running().isEmpty());
    }

    void wrongIndexesDoNothing()
    {
        m_switcher->open(-1);
        m_switcher->open(99);
        QCOMPARE(m_switcher->runningId(), QString());
    }

private:
    std::unique_ptr<PretendWindows> m_windows;
    std::unique_ptr<AppLauncher> m_launcher;
    std::unique_ptr<AppSwitcher> m_switcher;
};

QTEST_MAIN(TestSwitcher)
#include "tst_switcher.moc"
