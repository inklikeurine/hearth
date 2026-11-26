// SPDX-License-Identifier: GPL-3.0-or-later
#include "textbox.h"

#include <QTest>

using namespace Qt::StringLiterals;
using Change = TextBox::Change;

namespace {

constexpr uint32_t normal = 0;
constexpr uint32_t email = 6;
constexpr uint32_t password = 8;

// As labwc sends a text box: first that there is one, then what it's like, each
// with where its cursor is.
void focusBox(TextBox &box, uint32_t purpose = normal)
{
    box.activate();
    QCOMPARE(box.done(), Change::None);
    QCOMPARE(box.cursor(false), Change::None);
    box.contentType(0, purpose);
    QCOMPARE(box.done(), Change::None);
    QCOMPARE(box.cursor(true), Change::Focused);
}

} // namespace

class TestTextBox : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void kinds_data()
    {
        QTest::addColumn<uint>("hints");
        QTest::addColumn<uint>("purpose");
        QTest::addColumn<FieldKind>("kind");
        QTest::addColumn<bool>("sensitive");
        QTest::newRow("normal") << 0U << 0U << FieldKind::Text << false;
        QTest::newRow("alpha") << 0U << 1U << FieldKind::Text << false;
        QTest::newRow("digits") << 0U << 2U << FieldKind::Digits << false;
        QTest::newRow("number") << 0U << 3U << FieldKind::Number << false;
        QTest::newRow("phone") << 0U << 4U << FieldKind::Phone << false;
        QTest::newRow("url") << 0U << 5U << FieldKind::Url << false;
        QTest::newRow("email") << 0U << 6U << FieldKind::Email << false;
        QTest::newRow("password") << 0xc0U << 8U << FieldKind::Password << true;
        QTest::newRow("password, no hints") << 0U << 8U << FieldKind::Password << true;
        QTest::newRow("PIN") << 0U << 9U << FieldKind::Digits << true;
        QTest::newRow("hidden text") << 0x40U << 0U << FieldKind::Password << true;
        QTest::newRow("sensitive") << 0x80U << 0U << FieldKind::Text << true;
        QTest::newRow("date") << 0U << 10U << FieldKind::Text << false;
        QTest::newRow("from a newer protocol") << 0U << 99U << FieldKind::Text << false;
    }
    void kinds()
    {
        QFETCH(uint, hints);
        QFETCH(uint, purpose);
        QFETCH(FieldKind, kind);
        QFETCH(bool, sensitive);
        TextBox box;
        box.activate();
        box.contentType(hints, purpose);
        QCOMPARE(box.done(), Change::Focused);
        QCOMPARE(box.cursor(true), Change::None);
        QCOMPARE(box.content().kind(), kind);
        QCOMPARE(box.content().isSensitive(), sensitive);
    }

    void aBoxThatDescribesItselfCounts()
    {
        TextBox box;
        QVERIFY(!box.isFocused());
        focusBox(box);
        QVERIFY(box.isFocused());
        box.deactivate();
        QCOMPARE(box.done(), Change::Unfocused);
        QVERIFY(!box.isFocused());
    }

    // Qt does this for windows with no text box.
    void switchedOnWithNothingDescribedDoesNot()
    {
        TextBox box;
        box.activate();
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.cursor(false), Change::None);
        QCOMPARE(box.done(), Change::None);
        QVERIFY(!box.isFocused());
        box.deactivate();
        QCOMPARE(box.done(), Change::None);
    }

    // And then, when a text box in that window takes focus, only its cursor shows.
    void aCursorIsADescriptionToo()
    {
        TextBox box;
        box.activate();
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.cursor(false), Change::None);
        QCOMPARE(box.cursor(true), Change::Focused);
        QVERIFY(box.isFocused());
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.cursor(true), Change::None);
        // Focus left the box; Qt never says it's switched off.
        QCOMPARE(box.cursor(false), Change::Unfocused);
        QCOMPARE(box.cursor(true), Change::Focused);
    }

    void aDescribedBoxGoesWithItsCursorToo()
    {
        TextBox box;
        focusBox(box);
        QCOMPARE(box.cursor(true), Change::None);
        QCOMPARE(box.cursor(false), Change::Unfocused);
    }

    // Qt describes a focused button as it does an empty text box: Hearth's own
    // failure screen does this.
    void aDescriptionWithNoCursorDoesNot()
    {
        TextBox box;
        box.activate();
        box.contentType(7, normal);
        QCOMPARE(box.done(), Change::Focused);
        QCOMPARE(box.cursor(false), Change::Unfocused);
        box.contentType(7, normal);
        QCOMPARE(box.done(), Change::None);
        QVERIFY(!box.isFocused());
    }

    // As Firefox does when a page focuses a box itself.
    void textIsEnoughBeforeTheCursorShows()
    {
        TextBox box;
        box.activate();
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.cursor(false), Change::None);
        box.surroundingText("", 0, 0);
        box.contentType(0, normal);
        QCOMPARE(box.done(), Change::Focused);
        QCOMPARE(box.cursor(false), Change::None);
        QCOMPARE(box.cursor(true), Change::None);
        // From now on the cursor decides.
        QCOMPARE(box.cursor(false), Change::Unfocused);
        box.surroundingText("", 0, 0);
        QCOMPARE(box.done(), Change::None);
        QVERIFY(!box.isFocused());
    }

    // A compositor that says nothing about cursors.
    void aDescriptionIsEnoughWithNoWordOnCursors()
    {
        TextBox box;
        box.activate();
        box.contentType(0, email);
        QCOMPARE(box.done(), Change::Focused);
        box.contentType(0, email);
        QCOMPARE(box.done(), Change::None);
        QVERIFY(box.isFocused());
    }

    void theNextBoxIsAnother()
    {
        TextBox box;
        focusBox(box, email);
        QCOMPARE(box.content().kind(), FieldKind::Email);
        box.deactivate();
        QCOMPARE(box.done(), Change::Unfocused);
        QVERIFY(!box.isFocused());
        focusBox(box);
        // Nothing left over from the last one.
        QCOMPARE(box.content().kind(), FieldKind::Text);
    }

    // Some apps move focus without a word between.
    void switchedOnAgainIsAnotherBox()
    {
        TextBox box;
        focusBox(box);
        box.activate();
        box.contentType(0, normal);
        QCOMPARE(box.done(), Change::Focused);
        QCOMPARE(box.cursor(true), Change::None);
    }

    void anotherKindIsAnotherBox()
    {
        TextBox box;
        focusBox(box);
        box.contentType(0, normal);
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.cursor(true), Change::None);
        box.contentType(0xc0, password);
        QCOMPARE(box.done(), Change::Focused);
        QCOMPARE(box.content().kind(), FieldKind::Password);
    }

    void countsTheBytesToDelete()
    {
        TextBox box;
        focusBox(box);
        const QByteArray text = u"aé€😀z"_s.toUtf8();
        box.surroundingText(text, text.size() - 1, text.size() - 1);
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.takeBefore(), 4);
        QCOMPARE(box.takeBefore(), 3);
        QCOMPARE(box.takeBefore(), 2);
        QCOMPARE(box.takeBefore(), 1);
        // Nothing known before that.
        QCOMPARE(box.takeBefore(), 1);
        box.typed(u"né"_s);
        QCOMPARE(box.takeBefore(), 2);
        QCOMPARE(box.takeBefore(), 1);
    }

    void keepsToTheAppsWord()
    {
        TextBox box;
        focusBox(box);
        box.typed(u"é"_s);
        box.surroundingText("ab", 2, 2);
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.takeBefore(), 1);
        // A cursor past the end, or text cut in the middle of a character.
        box.surroundingText("ab", 99, 99);
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.takeBefore(), 1);
        box.surroundingText("\xa9z", 1, 1);
        QCOMPARE(box.done(), Change::None);
        QCOMPARE(box.takeBefore(), 1);
    }

    void knowsWhatIsSelected()
    {
        TextBox box;
        focusBox(box);
        box.surroundingText("hello", 5, 0);
        QCOMPARE(box.done(), Change::None);
        QVERIFY(box.hasSelection());
        box.typed(u"x"_s);
        QVERIFY(!box.hasSelection());
    }
};

QTEST_GUILESS_MAIN(TestTextBox)
#include "tst_textbox.moc"
