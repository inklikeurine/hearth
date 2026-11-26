// SPDX-License-Identifier: GPL-3.0-or-later
#include "keymap.h"
#include "keyrules.h"

#include <QSignalSpy>
#include <QTest>

using namespace Qt::StringLiterals;

namespace {

// Linux's numbers for some keys.
enum Code : uint32_t {
    CodeEsc = 1,
    CodeBackspace = 14,
    CodeTab = 15,
    CodeQ = 16,
    CodeEnter = 28,
    CodeCtrl = 29,
    CodeA = 30,
    CodeShift = 42,
    CodeSpace = 57,
    CodeF5 = 63,
    CodeKeypad4 = 75,
    CodeUp = 103,
    CodeLeft = 105,
    CodeRight = 106,
    CodeDown = 108,
    CodeVolumeUp = 115,
    CodeBack = 158,
    CodeOk = 352,
    CodeSelect = 353,
};

// Modifiers, as a US keymap numbers them.
constexpr uint32_t shiftBit = 1;
constexpr uint32_t capsBit = 2;
constexpr uint32_t ctrlBit = 4;
constexpr uint32_t altBit = 8;

} // namespace

class TestKeyRules : public QObject {
    Q_OBJECT

private:
    Keymap m_keymap;
    KeyRules *m_rules = nullptr;

    void press(uint32_t code, uint32_t modifiers = 0)
    {
        m_keymap.setModifiers(modifiers, 0, 0, 0);
        m_rules->press(m_keymap.key(code));
    }
    void tap(uint32_t code, uint32_t modifiers = 0)
    {
        press(code, modifiers);
        m_rules->release(code);
    }

private Q_SLOTS:
    void init()
    {
        QVERIFY(m_keymap.load(Keymap::us()));
        m_rules = new KeyRules(this);
    }
    void cleanup()
    {
        delete m_rules;
    }

    void namesKeysThroughTheKeymap()
    {
        QCOMPARE(m_keymap.key(CodeQ).text, u"q"_s);
        m_keymap.setModifiers(shiftBit, 0, 0, 0);
        QCOMPARE(m_keymap.key(CodeQ).text, u"Q"_s);
        m_keymap.setModifiers(0, 0, capsBit, 0);
        QCOMPARE(m_keymap.key(CodeQ).text, u"Q"_s);
        m_keymap.setModifiers(ctrlBit, 0, 0, 0);
        QCOMPARE(m_keymap.key(CodeQ).text, QString());
        QVERIFY(m_keymap.key(CodeQ).shortcut);
        m_keymap.setModifiers(altBit, 0, 0, 0);
        QVERIFY(m_keymap.key(CodeQ).shortcut);
        m_keymap.setModifiers(0, 0, 0, 0);
        // These come as control characters, which aren't text.
        for (const uint32_t code : { CodeEnter, CodeTab, CodeEsc, CodeBackspace, CodeUp }) {
            QCOMPARE(m_keymap.key(code).text, QString());
        }
        QVERIFY(m_keymap.key(CodeUp).symbol != 0);
    }

    // A keymap of a test tool or an odd device: one key, named by a number.
    void readsTinyKeymaps()
    {
        const QByteArray tiny = "xkb_keymap {\n"
                                "xkb_keycodes \"(unnamed)\" { minimum = 8; maximum = 10; <K1> = 9; "
                                "<K2> = 10; };\n"
                                "xkb_types \"(unnamed)\" { include \"complete\" };\n"
                                "xkb_compatibility \"(unnamed)\" { include \"complete\" };\n"
                                "xkb_symbols \"(unnamed)\" { key <K1> {[0xff53]}; "
                                "key <K2> {[U20AC]}; };\n"
                                "};\n";
        QVERIFY(m_keymap.load(tiny));
        QCOMPARE(m_keymap.key(2).text, u"€"_s);
        QSignalSpy moved(m_rules, &KeyRules::moved);
        tap(1);
        QCOMPARE(moved.count(), 1);
        QCOMPARE(moved.at(0).at(0).value<Direction>(), Direction::Right);
    }

    void surviveANonsenseKeymap()
    {
        QVERIFY(!m_keymap.load("not a keymap"));
        QCOMPARE(m_keymap.key(CodeQ).text, QString());
        m_keymap.setModifiers(1, 0, 0, 0);
        tap(CodeQ);
    }

    void arrowsMove_data()
    {
        QTest::addColumn<uint>("code");
        QTest::addColumn<Direction>("direction");
        QTest::newRow("up") << uint(CodeUp) << Direction::Up;
        QTest::newRow("down") << uint(CodeDown) << Direction::Down;
        QTest::newRow("left") << uint(CodeLeft) << Direction::Left;
        QTest::newRow("right") << uint(CodeRight) << Direction::Right;
        QTest::newRow("keypad") << uint(CodeKeypad4) << Direction::Left;
    }
    void arrowsMove()
    {
        QFETCH(uint, code);
        QFETCH(Direction, direction);
        QSignalSpy moved(m_rules, &KeyRules::moved);
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        tap(code);
        QCOMPARE(moved.count(), 1);
        QCOMPARE(moved.at(0).at(0).value<Direction>(), direction);
        QCOMPARE(passedOn.count(), 0);
    }

    void whateverTypesGoesToTheApp()
    {
        QSignalSpy typed(m_rules, &KeyRules::typedThrough);
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        tap(CodeQ);
        tap(CodeA, shiftBit);
        tap(CodeSpace);
        QCOMPARE(typed.count(), 3);
        QCOMPARE(typed.at(0).at(0).toString(), u"q"_s);
        QCOMPARE(typed.at(1).at(0).toString(), u"A"_s);
        QCOMPARE(typed.at(2).at(0).toString(), u" "_s);
        QCOMPARE(passedOn.count(), 6);
        QCOMPARE(passedOn.at(2), QVariantList({ uint(CodeA), true }));
        QCOMPARE(passedOn.at(3), QVariantList({ uint(CodeA), false }));
    }

    void enterIsSelectUnlessSomeoneIsTyping()
    {
        QSignalSpy selected(m_rules, &KeyRules::selected);
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        tap(CodeEnter);
        QCOMPARE(selected.count(), 1);
        tap(CodeQ);
        tap(CodeEnter);
        QCOMPARE(selected.count(), 1);
        // To the app, down and up.
        QCOMPARE(passedOn.count(), 4);
        QCOMPARE(passedOn.at(2), QVariantList({ uint(CodeEnter), true }));
        QCOMPARE(passedOn.at(3), QVariantList({ uint(CodeEnter), false }));
        tap(CodeEnter);
        QCOMPARE(passedOn.count(), 6);
        tap(CodeLeft);
        tap(CodeEnter);
        QCOMPARE(selected.count(), 2);
        // Another text box starts over.
        tap(CodeQ);
        m_rules->reset();
        tap(CodeEnter);
        QCOMPARE(selected.count(), 3);
        // So does a click on a key.
        tap(CodeQ);
        m_rules->pressedOnScreen();
        tap(CodeEnter);
        QCOMPARE(selected.count(), 4);
    }

    void aRemotesOkIsAlwaysSelect()
    {
        QSignalSpy selected(m_rules, &KeyRules::selected);
        tap(CodeQ);
        tap(CodeOk);
        tap(CodeQ);
        tap(CodeSelect);
        QCOMPARE(selected.count(), 2);
    }

    void volumeKeysAreNotTyping()
    {
        QSignalSpy selected(m_rules, &KeyRules::selected);
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        tap(CodeVolumeUp);
        QCOMPARE(passedOn.count(), 2);
        tap(CodeEnter);
        QCOMPARE(selected.count(), 1);
    }

    void theRestGoesToTheApp()
    {
        QSignalSpy typed(m_rules, &KeyRules::typedThrough);
        QSignalSpy moved(m_rules, &KeyRules::moved);
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        tap(CodeTab);
        tap(CodeF5);
        tap(CodeA, ctrlBit);
        tap(CodeLeft, altBit);
        QCOMPARE(passedOn.count(), 8);
        QCOMPARE(typed.count(), 0);
        QCOMPARE(moved.count(), 0);
        // Not Shift and Ctrl themselves: they go with the next key.
        tap(CodeShift);
        tap(CodeCtrl);
        QCOMPARE(passedOn.count(), 8);
    }

    void nothingStaysPressedInTheApp()
    {
        QSignalSpy passedOn(m_rules, &KeyRules::passedOn);
        press(CodeTab);
        QCOMPARE(passedOn.count(), 1);
        m_rules->reset();
        QCOMPARE(passedOn.count(), 2);
        QCOMPARE(passedOn.at(1), QVariantList({ uint(CodeTab), false }));
        m_rules->release(CodeTab);
        QCOMPARE(passedOn.count(), 2);
        // Nor does the app hear of a key coming up that it never saw go down.
        m_rules->release(CodeQ);
        QCOMPARE(passedOn.count(), 2);
    }

    // In a session labwc takes these before the keyboard does.
    void backAndBackspaceAnyway()
    {
        QSignalSpy closed(m_rules, &KeyRules::closed);
        QSignalSpy backspace(m_rules, &KeyRules::backspace);
        tap(CodeEsc);
        tap(CodeBack);
        tap(CodeBackspace);
        QCOMPARE(closed.count(), 2);
        QCOMPARE(backspace.count(), 1);
    }

    void aHeldArrowRepeatsAtTheGivenRate()
    {
        m_rules->setRepeat(50, 100);
        QSignalSpy moved(m_rules, &KeyRules::moved);
        press(CodeRight);
        QCOMPARE(moved.count(), 1);
        QTest::qWait(40);
        QCOMPARE(moved.count(), 1);
        QTRY_VERIFY(moved.count() >= 4);
        m_rules->release(CodeRight);
        const auto count = moved.count();
        QTest::qWait(100);
        QCOMPARE(moved.count(), count);
    }

    void repeatingStops()
    {
        m_rules->setRepeat(100, 10);
        QSignalSpy moved(m_rules, &KeyRules::moved);
        // When the keyboard goes.
        press(CodeRight);
        m_rules->reset();
        QTest::qWait(50);
        QCOMPARE(moved.count(), 1);
        // When the compositor repeats nothing.
        m_rules->setRepeat(0, 10);
        press(CodeRight);
        QTest::qWait(50);
        QCOMPARE(moved.count(), 2);
        // And the last arrow pressed is the one that repeats.
        m_rules->setRepeat(100, 10);
        press(CodeRight);
        press(CodeDown);
        m_rules->release(CodeRight);
        QTRY_VERIFY(moved.count() >= 6);
        QCOMPARE(moved.constLast().at(0).value<Direction>(), Direction::Down);
        m_rules->release(CodeDown);
    }

    void canIgnoreDoublePresses()
    {
        QSignalSpy moved(m_rules, &KeyRules::moved);
        QSignalSpy selected(m_rules, &KeyRules::selected);
        QSignalSpy typed(m_rules, &KeyRules::typedThrough);
        tap(CodeRight);
        tap(CodeRight);
        QCOMPARE(moved.count(), 2);
        m_rules->setIgnoreDoublePresses(true);
        QTest::qWait(350);
        tap(CodeRight);
        tap(CodeRight);
        tap(CodeLeft);
        QCOMPARE(moved.count(), 4);
        tap(CodeOk);
        tap(CodeOk);
        QCOMPARE(selected.count(), 1);
        // Typing on a real keyboard isn't a button press.
        tap(CodeQ);
        tap(CodeQ);
        QCOMPARE(typed.count(), 2);
        QTest::qWait(350);
        tap(CodeOk);
        QCOMPARE(selected.count(), 2);
    }

    void aHeldSelectPressesOnce()
    {
        m_rules->setRepeat(100, 10);
        QSignalSpy selected(m_rules, &KeyRules::selected);
        press(CodeEnter);
        QTest::qWait(50);
        m_rules->release(CodeEnter);
        QCOMPARE(selected.count(), 1);
    }
};

QTEST_GUILESS_MAIN(TestKeyRules)
#include "tst_keyrules.moc"
