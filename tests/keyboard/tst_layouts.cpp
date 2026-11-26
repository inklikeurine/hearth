// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardmodel.h"
#include "layout.h"

#include <QSet>
#include <QSignalSpy>
#include <QTest>

#include <utility>

using namespace Qt::StringLiterals;

namespace {

const QHash<QString, LayoutKind> &layouts()
{
    static const QHash<QString, LayoutKind> names {
        { u"qwerty"_s, LayoutKind::Qwerty },
        { u"alphabet"_s, LayoutKind::Alphabet },
        { u"row"_s, LayoutKind::Row },
        { u"wheel"_s, LayoutKind::Wheel },
    };
    return names;
}
const QHash<QString, FieldKind> &fields()
{
    static const QHash<QString, FieldKind> names {
        { u"text"_s, FieldKind::Text },
        { u"email"_s, FieldKind::Email },
        { u"url"_s, FieldKind::Url },
        { u"password"_s, FieldKind::Password },
        { u"number"_s, FieldKind::Number },
        { u"digits"_s, FieldKind::Digits },
        { u"phone"_s, FieldKind::Phone },
    };
    return names;
}
const QHash<QString, Page> &pages()
{
    static const QHash<QString, Page> names {
        { u"letters"_s, Page::Letters },
        { u"symbols"_s, Page::Symbols },
        { u"pad"_s, Page::Pad },
    };
    return names;
}
constexpr Direction directions[]
    = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };

// The keys the arrows reach from one key.
QSet<int> reached(const Layout &layout, int from)
{
    using Spot = std::pair<int, qreal>;
    const Spot start { from, layout.keys().at(from).place.center().x() };
    QSet<Spot> seen { start };
    QList<Spot> next { start };
    QSet<int> keys;
    while (!next.isEmpty()) {
        const Spot spot = next.takeLast();
        keys << spot.first;
        for (const Direction direction : directions) {
            qreal column = spot.second;
            const int to = layout.move(spot.first, direction, column);
            const Spot there { to, column };
            if (to >= 0 && to < layout.keys().size() && !seen.contains(there)) {
                seen << there;
                next << there;
            }
        }
    }
    return keys;
}

QString typedBy(const Layout &layout)
{
    QString all;
    for (const Key &key : layout.keys()) {
        all += key.text;
    }
    return all;
}

int keyWith(const Layout &layout, const QString &text)
{
    for (int i = 0; i < layout.keys().size(); ++i) {
        if (layout.keys().at(i).text == text) {
            return i;
        }
    }
    return -1;
}

int keyWith(const KeyboardModel &model, const QString &text)
{
    for (int i = 0; i < model.rowCount(); ++i) {
        if (model.index(i).data(KeyboardModel::Text).toString() == text) {
            return i;
        }
    }
    return -1;
}

} // namespace

class TestLayouts : public QObject {
    Q_OBJECT

    static void everyLayout()
    {
        QTest::addColumn<QString>("layout");
        QTest::addColumn<QString>("field");
        QTest::addColumn<QString>("page");
        for (const auto &[layout, layoutKind] : layouts().asKeyValueRange()) {
            for (const auto &[field, fieldKind] : fields().asKeyValueRange()) {
                for (const auto &[page, pageKind] : pages().asKeyValueRange()) {
                    QTest::addRow(
                        "%s %s %s", qPrintable(layout), qPrintable(field), qPrintable(page))
                        << layout << field << page;
                }
            }
        }
    }
    static Layout fetched()
    {
        QFETCH(QString, layout);
        QFETCH(QString, field);
        QFETCH(QString, page);
        return { layouts().value(layout), fields().value(field), pages().value(page) };
    }

private Q_SLOTS:
    void arrowsReachEveryKeyFromEveryKey_data()
    {
        everyLayout();
    }
    void arrowsReachEveryKeyFromEveryKey()
    {
        const Layout layout = fetched();
        QVERIFY(layout.keys().size() > 10);
        for (int from = 0; from < layout.keys().size(); ++from) {
            const QSet<int> keys = reached(layout, from);
            for (int key = 0; key < layout.keys().size(); ++key) {
                QVERIFY2(keys.contains(key),
                    qPrintable(u"from %1 (%2), never reaches %3 (%4)"_s.arg(from)
                            .arg(layout.keys().at(from).text)
                            .arg(key)
                            .arg(layout.keys().at(key).text)));
            }
        }
    }

    void keysHaveRoom_data()
    {
        everyLayout();
    }
    void keysHaveRoom()
    {
        const Layout layout = fetched();
        const QRectF all(QPointF(), layout.size());
        for (int i = 0; i < layout.keys().size(); ++i) {
            const QRectF place = layout.keys().at(i).place;
            QVERIFY(all.contains(place));
            for (int j = 0; j < i; ++j) {
                QVERIFY(!place.intersects(layout.keys().at(j).place));
            }
        }
    }

    void everyLayoutCanDoEverything_data()
    {
        everyLayout();
    }
    void everyLayoutCanDoEverything()
    {
        const Layout layout = fetched();
        QFETCH(QString, page);
        QVERIFY(layout.find(KeyAction::Backspace) >= 0);
        QVERIFY(layout.find(KeyAction::Enter) >= 0);
        QVERIFY(layout.find(KeyAction::Close) >= 0);
        QCOMPARE(layout.find(KeyAction::Space) >= 0, page != u"pad"_s);
        QCOMPARE(layout.find(KeyAction::Shift) >= 0, page == u"letters"_s);
        QCOMPARE(layout.find(KeyAction::Symbols) >= 0, page == u"letters"_s);
        QCOMPARE(layout.find(KeyAction::Letters) >= 0, page != u"letters"_s);
    }

    void everyCharacterIsSomewhere_data()
    {
        everyLayout();
    }
    void everyCharacterIsSomewhere()
    {
        const Layout layout = fetched();
        QFETCH(QString, page);
        const QString typed = typedBy(layout);
        for (char16_t character = u'!'; character <= u'~'; ++character) {
            const bool letter = QChar(character).isLetter();
            const bool digit = QChar(character).isDigit();
            const bool wanted = page == u"letters"_s ? letter && QChar(character).isLower()
                : page == u"symbols"_s               ? !letter
                                                     : digit;
            if (wanted) {
                QVERIFY2(typed.contains(QChar(character)), qPrintable(QString(QChar(character))));
            }
        }
    }

    void eachKindOfFieldHasItsKeys_data()
    {
        QTest::addColumn<QString>("field");
        QTest::addColumn<QStringList>("wanted");
        QTest::addColumn<QStringList>("unwanted");
        QTest::newRow("text") << u"text"_s << QStringList { u"?"_s, u"'"_s }
                              << QStringList { u".com"_s };
        QTest::newRow("email") << u"email"_s << QStringList { u"@"_s, u".com"_s, u"_"_s }
                               << QStringList { u"'"_s };
        QTest::newRow("url") << u"url"_s << QStringList { u"/"_s, u".com"_s, u":"_s }
                             << QStringList { u"'"_s };
        QTest::newRow("password") << u"password"_s << QStringList { u"!"_s }
                                  << QStringList { u".com"_s };
        QTest::newRow("number") << u"number"_s << QStringList { u"."_s, u"-"_s }
                                << QStringList { u"a"_s, u"*"_s };
        QTest::newRow("digits") << u"digits"_s << QStringList { u"0"_s }
                                << QStringList { u"a"_s, u"."_s, u"-"_s };
        QTest::newRow("phone") << u"phone"_s << QStringList { u"+"_s, u"*"_s, u"#"_s }
                               << QStringList { u"a"_s };
    }
    void eachKindOfFieldHasItsKeys()
    {
        QFETCH(QString, field);
        QFETCH(QStringList, wanted);
        QFETCH(QStringList, unwanted);
        const FieldKind kind = fields().value(field);
        for (const LayoutKind layoutKind : layouts()) {
            const Layout layout(layoutKind, kind, Layout::firstPage(kind));
            for (const QString &text : wanted) {
                QVERIFY2(keyWith(layout, text) >= 0, qPrintable(text));
            }
            for (const QString &text : unwanted) {
                QVERIFY2(keyWith(layout, text) < 0, qPrintable(text));
            }
        }
    }

    void startsOnTheFirstLetterOrDigit()
    {
        const auto first = [](LayoutKind kind, FieldKind field) {
            const Layout layout(kind, field, Layout::firstPage(field));
            return layout.keys().at(layout.firstKey()).text;
        };
        QCOMPARE(first(LayoutKind::Qwerty, FieldKind::Text), u"q"_s);
        QCOMPARE(first(LayoutKind::Alphabet, FieldKind::Text), u"a"_s);
        QCOMPARE(first(LayoutKind::Row, FieldKind::Text), u"a"_s);
        QCOMPARE(first(LayoutKind::Wheel, FieldKind::Text), u"a"_s);
        QCOMPARE(first(LayoutKind::Qwerty, FieldKind::Phone), u"1"_s);
    }

    void gridsStopAtTheEdges()
    {
        const Layout layout(LayoutKind::Qwerty, FieldKind::Text, Page::Letters);
        const int q = keyWith(layout, u"q"_s);
        qreal column = layout.keys().at(q).place.center().x();
        QCOMPARE(layout.move(q, Direction::Left, column), q);
        const int one = layout.move(q, Direction::Up, column);
        QCOMPARE(layout.keys().at(one).text, u"1"_s);
        QCOMPARE(layout.move(one, Direction::Up, column), one);
    }

    void upAndDownComeBackOverAWideKey()
    {
        const Layout layout(LayoutKind::Qwerty, FieldKind::Text, Page::Letters);
        const int c = keyWith(layout, u"c"_s);
        qreal column = layout.keys().at(c).place.center().x();
        const int space = layout.move(c, Direction::Down, column);
        QCOMPARE(layout.keys().at(space).action, KeyAction::Space);
        QCOMPARE(layout.move(space, Direction::Up, column), c);
    }

    void theRowGoesRound()
    {
        const Layout layout(LayoutKind::Row, FieldKind::Text, Page::Letters);
        qreal column = 0;
        const auto last = int(layout.keys().size()) - 1;
        QCOMPARE(layout.move(0, Direction::Left, column), last);
        QCOMPARE(layout.move(last, Direction::Right, column), 0);
        // Space is one step from the first letter.
        const int space = layout.move(layout.firstKey(), Direction::Left, column);
        QCOMPARE(layout.keys().at(space).action, KeyAction::Space);
    }

    void theWheelGoesRound()
    {
        const Layout layout(LayoutKind::Wheel, FieldKind::Text, Page::Letters);
        qreal column = 0;
        const int a = layout.firstKey();
        const int e = layout.move(a, Direction::Right, column);
        QCOMPARE(layout.keys().at(e).text, u"e"_s);
        QCOMPARE(layout.move(e, Direction::Left, column), a);
        const int b = layout.move(a, Direction::Down, column);
        QCOMPARE(layout.keys().at(b).text, u"b"_s);
        const int d = layout.move(a, Direction::Up, column);
        QCOMPARE(layout.keys().at(d).text, u"d"_s);
        const int space = layout.move(a, Direction::Left, column);
        QCOMPARE(layout.keys().at(space).action, KeyAction::Space);
    }

    void shiftIsForOneLetterThenLocks()
    {
        KeyboardModel model;
        model.open(LayoutKind::Qwerty, FieldKind::Email);
        QSignalSpy typed(&model, &KeyboardModel::typed);
        const int shift = model.keys().find(KeyAction::Shift);
        const int w = keyWith(model, u"w"_s);
        const int com = keyWith(model, u".com"_s);

        model.pressKey(shift);
        QCOMPARE(model.shift(), KeyboardModel::Once);
        QCOMPARE(model.index(w).data(KeyboardModel::Text).toString(), u"W"_s);
        QCOMPARE(model.index(com).data(KeyboardModel::Text).toString(), u".com"_s);
        model.pressKey(w);
        model.pressKey(w);
        QCOMPARE(typed.at(0).at(0).toString(), u"W"_s);
        QCOMPARE(typed.at(1).at(0).toString(), u"w"_s);

        model.pressKey(shift);
        model.pressKey(shift);
        QCOMPARE(model.shift(), KeyboardModel::Locked);
        model.pressKey(w);
        model.pressKey(w);
        QCOMPARE(typed.at(3).at(0).toString(), u"W"_s);
        model.pressKey(shift);
        QCOMPARE(model.shift(), KeyboardModel::Off);

        // Another text box starts without it.
        model.pressKey(shift);
        model.open(LayoutKind::Qwerty, FieldKind::Text);
        QCOMPARE(model.shift(), KeyboardModel::Off);
    }

    void symbolsAndBackWithTheSameKey()
    {
        for (const LayoutKind layout : layouts()) {
            KeyboardModel model;
            model.open(layout, FieldKind::Text);
            QVERIFY(keyWith(model, u"a"_s) >= 0);
            model.pressKey(model.keys().find(KeyAction::Symbols));
            QVERIFY(keyWith(model, u"a"_s) < 0);
            QVERIFY(keyWith(model, u"{"_s) >= 0);
            model.press();
            QVERIFY(keyWith(model, u"a"_s) >= 0);
        }
    }

    void aNumberPadCanShowTheLetters()
    {
        KeyboardModel model;
        model.open(LayoutKind::Qwerty, FieldKind::Digits);
        QVERIFY(keyWith(model, u"a"_s) < 0);
        model.pressKey(model.keys().find(KeyAction::Letters));
        QVERIFY(keyWith(model, u"a"_s) >= 0);
    }

    void pressingDoesWhatTheKeySays()
    {
        KeyboardModel model;
        model.open(LayoutKind::Qwerty, FieldKind::Text);
        QSignalSpy typed(&model, &KeyboardModel::typed);
        QSignalSpy backspace(&model, &KeyboardModel::backspace);
        QSignalSpy enter(&model, &KeyboardModel::enter);
        QSignalSpy close(&model, &KeyboardModel::close);
        model.press();
        QCOMPARE(typed.takeFirst().at(0).toString(), u"q"_s);
        model.move(Direction::Right);
        model.press();
        QCOMPARE(typed.takeFirst().at(0).toString(), u"w"_s);
        model.pressKey(model.keys().find(KeyAction::Space));
        QCOMPARE(typed.takeFirst().at(0).toString(), u" "_s);
        model.pressKey(model.keys().find(KeyAction::Backspace));
        QCOMPARE(backspace.count(), 1);
        model.pressKey(model.keys().find(KeyAction::Enter));
        QCOMPARE(enter.count(), 1);
        model.pressKey(model.keys().find(KeyAction::Close));
        QCOMPARE(close.count(), 1);
        QCOMPARE(typed.count(), 0);
        // Nothing there.
        model.pressKey(-1);
        model.pressKey(1000);
        QCOMPARE(typed.count(), 0);
    }
};

QTEST_GUILESS_MAIN(TestLayouts)
#include "tst_layouts.moc"
