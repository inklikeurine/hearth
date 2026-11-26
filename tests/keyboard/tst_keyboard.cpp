// SPDX-License-Identifier: GPL-3.0-or-later
#include "commandserver.h"
#include "fakelink.h"
#include "keyboard.h"
#include "keyboardsettings.h"
#include "shelllink.h"

#include <QDir>
#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <memory>

using namespace Qt::StringLiterals;

namespace {

// Linux's numbers for some keys.
enum Code : uint32_t {
    CodeQ = 16,
    CodeTab = 15,
    CodeEnter = 28,
    CodeRight = 106,
    CodeDown = 108,
};
constexpr uint32_t email = 6;
constexpr uint32_t password = 8;
constexpr uint32_t terminal = 13;

} // namespace

// The whole keyboard but its window, with no compositor.
class TestKeyboard : public QObject {
    Q_OBJECT

private:
    struct Parts {
        FakeLink link;
        KeyboardSettings settings;
        ShellLink shell { CommandServer::socketPath() };
        Keyboard keyboard { &link, &settings, &shell };
    };
    QTemporaryDir m_dir;
    std::unique_ptr<Parts> m;

    void showIt(uint32_t purpose = 0, uint32_t hints = 0)
    {
        m->link.focusBox(purpose, hints);
        QTRY_VERIFY(m->keyboard.isShown());
    }
    int keyWith(const QString &text)
    {
        const KeyboardModel *keys = m->keyboard.keys();
        for (int i = 0; i < keys->rowCount(); ++i) {
            if (keys->index(i).data(KeyboardModel::Text).toString() == text) {
                return i;
            }
        }
        return -1;
    }
    QString highlighted()
    {
        const KeyboardModel *keys = m->keyboard.keys();
        return keys->index(keys->currentIndex()).data(KeyboardModel::Text).toString();
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Once: QSettings looks at it only the first time.
        qputenv("XDG_CONFIG_HOME", QFile::encodeName(m_dir.path()));
        qputenv("XDG_RUNTIME_DIR", QFile::encodeName(m_dir.path()));
        qputenv("WAYLAND_DISPLAY", "wayland-test");
    }
    void init()
    {
        QDir(m_dir.filePath(u"hearth"_s)).removeRecursively();
        m = std::make_unique<Parts>();
    }
    void cleanup()
    {
        m.reset();
    }

    void showsForATextBoxAndGoesWithIt()
    {
        QVERIFY(!m->keyboard.isShown());
        showIt();
        QVERIFY(m->link.grabbed);
        m->link.leaveBox();
        QTRY_VERIFY(!m->keyboard.isShown());
        QVERIFY(!m->link.grabbed);
    }

    void staysAwayFromAWindowWithNoTextBox()
    {
        Q_EMIT m->link.activated();
        Q_EMIT m->link.done();
        Q_EMIT m->link.cursorChanged(false);
        QTest::qWait(100);
        QVERIFY(!m->keyboard.isShown());
        QVERIFY(!m->link.grabbed);
        // A box in it takes focus.
        Q_EMIT m->link.cursorChanged(true);
        QTRY_VERIFY(m->keyboard.isShown());
    }

    // A focused button in a Qt window, as Hearth's failure screen has.
    void staysAwayFromADescriptionWithNoCursor()
    {
        Q_EMIT m->link.activated();
        Q_EMIT m->link.done();
        Q_EMIT m->link.cursorChanged(false);
        Q_EMIT m->link.contentType(7, 0);
        Q_EMIT m->link.done();
        Q_EMIT m->link.cursorChanged(false);
        QTest::qWait(100);
        QVERIFY(!m->keyboard.isShown());
        QVERIFY(!m->link.grabbed);
    }

    void staysClosedUntilAnotherBoxTakesFocus()
    {
        showIt();
        m->keyboard.close();
        QVERIFY(!m->keyboard.isShown());
        QVERIFY(!m->link.grabbed);
        // The same box changes: someone types with a real keyboard.
        Q_EMIT m->link.surroundingText("a", 1, 1);
        Q_EMIT m->link.contentType(0, 0);
        Q_EMIT m->link.done();
        Q_EMIT m->link.cursorChanged(true);
        QTest::qWait(100);
        QVERIFY(!m->keyboard.isShown());

        m->link.leaveBox();
        m->link.focusBox();
        QTRY_VERIFY(m->keyboard.isShown());
    }

    // The window mustn't close and open again when focus moves between boxes.
    void movingBetweenBoxesKeepsItShown()
    {
        showIt();
        QSignalSpy shown(&m->keyboard, &Keyboard::shownChanged);
        m->link.leaveBox();
        m->link.focusBox(email);
        QTRY_VERIFY(keyWith(u".com"_s) >= 0);
        QTest::qWait(100);
        QCOMPARE(shown.count(), 0);
        QVERIFY(m->link.grabbed);
    }

    void typesWithTheRemote()
    {
        showIt();
        m->link.tap(CodeEnter);
        m->link.tap(CodeRight);
        m->link.tap(CodeEnter);
        QCOMPARE(m->link.asked, QStringList({ u"type q"_s, u"type w"_s }));
    }

    void typesWithTheKeyboard()
    {
        showIt();
        m->link.tap(CodeQ);
        m->link.tap(CodeEnter);
        m->link.tap(CodeTab);
        QCOMPARE(m->link.asked,
            QStringList({ u"key 16 1"_s, u"key 16 0"_s, u"key 28 1"_s, u"key 28 0"_s, u"key 15 1"_s,
                u"key 15 0"_s }));
        QCOMPARE(highlighted(), u"q"_s);
    }

    void typesWithThePointer()
    {
        showIt();
        m->keyboard.keys()->pressKey(keyWith(u"w"_s));
        m->keyboard.keys()->pressKey(m->keyboard.keys()->keys().find(KeyAction::Enter));
        QCOMPARE(m->link.asked, QStringList({ u"type w"_s, u"enter"_s }));
    }

    // The app's word on what was typed can come later than the next key.
    void backspaceAfterTypingThrough()
    {
        showIt();
        Q_EMIT m->link.surroundingText("", 0, 0);
        Q_EMIT m->link.done();
        m->link.tap(26); // [, which a US keyboard has
        m->keyboard.backspace();
        QCOMPARE(m->link.asked, QStringList({ u"key 26 1"_s, u"key 26 0"_s, u"delete 1"_s }));
    }

    void backspaceDeletesOneCharacter()
    {
        showIt();
        m->keyboard.keys()->pressKey(m->keyboard.keys()->keys().find(KeyAction::Backspace));
        const QByteArray text = u"aé"_s.toUtf8();
        Q_EMIT m->link.surroundingText(text, text.size(), text.size());
        Q_EMIT m->link.done();
        m->keyboard.backspace();
        // Before the app has said anything about the last one.
        m->keyboard.backspace();
        QCOMPARE(m->link.asked, QStringList({ u"delete 1"_s, u"delete 2"_s, u"delete 1"_s }));
    }

    void backspaceDeletesWhatIsSelected()
    {
        showIt();
        Q_EMIT m->link.surroundingText("hello", 5, 0);
        Q_EMIT m->link.done();
        m->keyboard.backspace();
        QCOMPARE(m->link.asked, QStringList({ u"type  "_s, u"delete 1"_s }));
    }

    void backspaceInATerminalIsText()
    {
        showIt(terminal);
        m->keyboard.backspace();
        QCOMPARE(m->link.asked, QStringList({ u"type \x7f"_s }));
    }

    void backspaceWorksWhileItIsClosed()
    {
        showIt();
        m->keyboard.close();
        m->keyboard.backspace();
        QCOMPARE(m->link.asked, QStringList({ u"delete 1"_s }));
    }

    void doesNothingWithNoTextBox()
    {
        m->keyboard.backspace();
        m->keyboard.close();
        m->link.tap(CodeQ);
        QCOMPARE(m->link.asked, QStringList());
    }

    void aPasswordBoxSaysSo()
    {
        showIt();
        QVERIFY(!m->keyboard.isSensitive());
        QSignalSpy changed(&m->keyboard, &Keyboard::boxChanged);
        m->link.leaveBox();
        m->link.focusBox(password, 0xc0);
        QTRY_VERIFY(changed.count() > 0);
        QVERIFY(m->keyboard.isSensitive());
    }

    void eachBoxStartsAfresh()
    {
        showIt();
        m->keyboard.keys()->pressKey(m->keyboard.keys()->keys().find(KeyAction::Symbols));
        QVERIFY(keyWith(u"q"_s) < 0);
        m->link.leaveBox();
        QTRY_VERIFY(!m->keyboard.isShown());
        showIt();
        QCOMPARE(highlighted(), u"q"_s);
        QCOMPARE(m->keyboard.keys()->shift(), KeyboardModel::Off);
    }

    // Select on the Close key: the app never saw it go down, so it mustn't see it
    // come up.
    void keepsHoldOfAKeyThatIsStillDown()
    {
        showIt();
        m->link.press(CodeEnter);
        m->keyboard.close();
        QVERIFY(!m->keyboard.isShown());
        QVERIFY(m->link.grabbed);
        m->link.release(CodeEnter);
        QVERIFY(!m->link.grabbed);
    }

    void butNotForever()
    {
        showIt();
        m->link.press(CodeEnter);
        m->keyboard.close();
        QTRY_VERIFY_WITH_TIMEOUT(!m->link.grabbed, 4000);
    }

    void keysHeldWhileItGoesDoNothing()
    {
        showIt();
        m->link.press(CodeDown);
        m->keyboard.close();
        m->link.press(CodeQ);
        m->link.release(CodeQ);
        m->link.release(CodeDown);
        QCOMPARE(m->link.asked, QStringList());
        QVERIFY(!m->link.grabbed);
    }

    void changesLayoutWhileShown()
    {
        showIt();
        QCOMPARE(highlighted(), u"q"_s);
        QFile file(m_dir.filePath(u"hearth/keyboard.ini"_s));
        QVERIFY(QDir(m_dir.path()).mkpath(u"hearth"_s));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write("layout=alphabet\n");
        file.close();
        QTRY_COMPARE(highlighted(), u"a"_s);
    }

    void tellsTheShell()
    {
        CommandServer shell;
        QVERIFY(shell.listen());
        m->shell.start();
        showIt();
        QTRY_VERIFY(shell.keyboardShown());
        QTRY_VERIFY(shell.textBoxFocused());
        shell.hideKeyboard();
        QTRY_VERIFY(!m->keyboard.isShown());
        QTRY_VERIFY(!shell.keyboardShown());
        // The box is still there, so Backspace still deletes.
        QVERIFY(shell.textBoxFocused());
        shell.sendBackspace();
        QTRY_COMPARE(m->link.asked, QStringList({ u"delete 1"_s }));
        m->link.leaveBox();
        QTRY_VERIFY(!shell.textBoxFocused());
        // Goes before the shell does.
        showIt();
        QTRY_VERIFY(shell.textBoxFocused());
        m.reset();
        QTRY_VERIFY(!shell.textBoxFocused());
    }

    // Nothing typed may ever leave for the shell, whichever way it was typed.
    void whatIsTypedNeverReachesTheShell()
    {
        QLocalServer shell;
        QVERIFY(shell.listen(CommandServer::socketPath()));
        m->shell.start();
        QTRY_VERIFY(shell.hasPendingConnections());
        const std::unique_ptr<QLocalSocket> socket(shell.nextPendingConnection());
        showIt(password, 0xc0);
        m->link.tap(CodeQ);
        m->link.tap(CodeRight);
        m->link.tap(CodeEnter);
        m->keyboard.keys()->pressKey(keyWith(u"w"_s));
        m->keyboard.backspace();
        m->keyboard.close();
        m->link.leaveBox();
        QByteArray sent;
        const auto read = [&sent, &socket] { return sent += socket->readAll(); };
        QTRY_COMPARE(read().count("textbox 0\n"), 2);
        QCOMPARE(sent, "keyboard\nshown 0\ntextbox 0\nshown 1\ntextbox 1\nshown 0\ntextbox 0\n");
    }
};

QTEST_GUILESS_MAIN(TestKeyboard)
#include "tst_keyboard.moc"
