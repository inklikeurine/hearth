// SPDX-License-Identifier: GPL-3.0-or-later
#include "idlewatcher.h"
#include "keycatcher.h"
#include "labwcconfig.h"
#include "pressfilter.h"

#include <QFile>
#include <QKeyEvent>
#include <QSignalSpy>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QTest>
#include <QWindow>
#include <QXmlStreamReader>

#include <xkbcommon/xkbcommon-keysyms.h>

using namespace Qt::StringLiterals;

// Counts the key presses that get through to a window.
class KeyWindow : public QWindow {
    Q_OBJECT

public:
    int presses = 0;

protected:
    void keyPressEvent(QKeyEvent *) override
    {
        ++presses;
    }
};

class TestButtons : public QObject {
    Q_OBJECT

private:
    static void press(QWindow &window, int key, quint32 symbol, bool repeat = false)
    {
        QKeyEvent event(
            QEvent::KeyPress, key, Qt::NoModifier, 0, symbol, 0, { }, repeat);
        QCoreApplication::sendEvent(&window, &event);
    }

private Q_SLOTS:
    void init()
    {
        ShellSettings settings;
        settings.setExtraKeys({ });
        settings.setRepeatSpeed(ShellSettings::Normal);
    }

    void theShippedConfigIsReadable()
    {
        const ShellSettings settings;
        QXmlStreamReader xml(LabwcConfig::make(settings));
        while (!xml.atEnd()) {
            xml.readNext();
        }
        QVERIFY2(!xml.hasError(), qPrintable(xml.errorString()));
    }

    void addsTheProfilesButtons()
    {
        ShellSettings settings;
        settings.setExtraKeys({ { u"back"_s, QStringList { u"F5"_s, u"XF86Reload"_s } } });
        const QByteArray config = LabwcConfig::make(settings);
        QVERIFY(config.contains("<keybind key=\"F5\" onRelease=\"yes\"><action name=\"Execute\" "
                                "command=\"hearth-send back\" /></keybind>"));
        QVERIFY(config.contains("key=\"XF86Reload\""));
        // The usual ones stay.
        QVERIFY(config.contains("key=\"XF86Back\""));
    }

    // Someone could put anything in the settings file.
    void neverWritesAnythingButKeyNames()
    {
        ShellSettings settings;
        settings.setExtraKeys({ { u"back"_s,
            QStringList { u"F5\"/><action name=\"Execute\" command=\"evil"_s, u"Return"_s, u"a"_s,
                u"NoSuchKey"_s } } });
        const QByteArray config = LabwcConfig::make(settings);
        QVERIFY(!config.contains("evil"));
        QVERIFY(!config.contains("key=\"Return\""));
        QVERIFY(!config.contains("key=\"a\""));
        QVERIFY(!config.contains("NoSuchKey"));
    }

    void repeatSpeeds()
    {
        ShellSettings settings;
        QVERIFY(LabwcConfig::make(settings).contains("<repeatDelay>600<"));
        settings.setRepeatSpeed(ShellSettings::Slow);
        const QByteArray slow = LabwcConfig::make(settings);
        QVERIFY(slow.contains("<repeatRate>8<"));
        QVERIFY(slow.contains("<repeatDelay>1200<"));
    }

    void writesAFile()
    {
        const QTemporaryDir dir;
        const ShellSettings settings;
        QVERIFY(LabwcConfig::write(settings, dir.filePath(u"new/folder"_s)));
        QVERIFY(QFile::exists(dir.filePath(u"new/folder/rc.xml"_s)));
        QVERIFY(!LabwcConfig::write(settings, u"/proc/nowhere"_s));
    }

    void whichButtonsMayBeUsed_data()
    {
        QTest::addColumn<QString>("key");
        QTest::addColumn<bool>("may");
        for (const char *key : { "F5", "XF86Back", "XF86Red", "Print", "Page_Up", "Insert" }) {
            QTest::newRow(key) << QString::fromLatin1(key) << true;
        }
        for (const char *key : { "a", "1", "space", "eacute", "Up", "Return", "KP_Enter", "Tab",
                 "Shift_L", "Control_R", "BackSpace", "Escape", "", "nonsense" }) {
            QTest::newRow(qPrintable(u"not "_s + QLatin1String(key)))
                << QString::fromLatin1(key) << false;
        }
    }

    void whichButtonsMayBeUsed()
    {
        QFETCH(QString, key);
        QFETCH(bool, may);
        QCOMPARE(LabwcConfig::mayBeUsed(key), may);
    }

    void catchesOneKey()
    {
        KeyWindow window;
        KeyCatcher catcher;
        QSignalSpy caught(&catcher, &KeyCatcher::caught);
        press(window, Qt::Key_F5, XKB_KEY_F5);
        QCOMPARE(window.presses, 1);
        catcher.setCatching(true);
        press(window, Qt::Key_F5, XKB_KEY_F5);
        QCOMPARE(caught.count(), 1);
        QCOMPARE(caught.at(0).at(0).toString(), u"F5"_s);
        // Nothing else saw it, and the next one is an ordinary press again.
        QCOMPARE(window.presses, 1);
        press(window, Qt::Key_F5, XKB_KEY_F5);
        QCOMPARE(window.presses, 2);
    }

    void refusesKeysThatAreNeeded()
    {
        KeyWindow window;
        KeyCatcher catcher;
        QSignalSpy caught(&catcher, &KeyCatcher::caught);
        QSignalSpy refused(&catcher, &KeyCatcher::refused);
        catcher.setCatching(true);
        press(window, Qt::Key_Return, XKB_KEY_Return);
        QCOMPARE(refused.count(), 1);
        QCOMPARE(caught.count(), 0);
        QCOMPARE(window.presses, 0);
        QVERIFY(!catcher.catching());
    }

    void dropsPressesThatComeTooSoon()
    {
        KeyWindow window;
        PressFilter filter;
        filter.setProperty("milliseconds", 100);
        press(window, Qt::Key_Right, XKB_KEY_Right);
        press(window, Qt::Key_Right, XKB_KEY_Right);
        QCOMPARE(window.presses, 2);

        filter.setActive(true);
        press(window, Qt::Key_Right, XKB_KEY_Right);
        press(window, Qt::Key_Right, XKB_KEY_Right);
        QCOMPARE(window.presses, 3);
        // Another button, and a held one, still count.
        press(window, Qt::Key_Left, XKB_KEY_Left);
        press(window, Qt::Key_Left, XKB_KEY_Left, true);
        QCOMPARE(window.presses, 5);
        QTest::qWait(150);
        press(window, Qt::Key_Right, XKB_KEY_Right);
        QCOMPARE(window.presses, 6);
    }

    void dropsCommandsThatComeTooSoon()
    {
        PressFilter filter;
        QVERIFY(filter.allows(u"back"_s) && filter.allows(u"back"_s));
        filter.setActive(true);
        QVERIFY(filter.allows(u"back"_s));
        QVERIFY(!filter.allows(u"back"_s));
        QVERIFY(filter.allows(u"home"_s));
        QVERIFY(filter.allows(u"volume-up"_s) && filter.allows(u"volume-up"_s));
        QVERIFY(filter.allows(u"backspace"_s) && filter.allows(u"backspace"_s));
    }

    void idleAfterAWhileAndNotBefore()
    {
        KeyWindow window;
        IdleWatcher watcher;
        watcher.setSeconds(1);
        for (int i = 0; i < 4; ++i) {
            QTest::qWait(400);
            QVERIFY(!watcher.idle());
            press(window, Qt::Key_Right, XKB_KEY_Right);
        }
        QTRY_VERIFY(watcher.idle());
        press(window, Qt::Key_Right, XKB_KEY_Right);
        QVERIFY(!watcher.idle());
        // A press labwc caught.
        QTRY_VERIFY(watcher.idle());
        watcher.activity();
        QVERIFY(!watcher.idle());
    }

    void neverIdleWhenOffOrNotRunning()
    {
        IdleWatcher off;
        off.setSeconds(0);
        IdleWatcher stopped;
        stopped.setSeconds(1);
        stopped.setRunning(false);
        QTest::qWait(1300);
        QVERIFY(!off.idle());
        QVERIFY(!stopped.idle());
    }
};

QTEST_MAIN(TestButtons)
#include "tst_buttons.moc"
