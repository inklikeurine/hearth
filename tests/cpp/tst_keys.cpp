// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyrouter.h"

#include <QMetaMethod>
#include <QSignalSpy>
#include <QTest>

using namespace Qt::StringLiterals;

// What each key means, for everything that can be showing.
class TestKeys : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void meaning_data()
    {
        QTest::addColumn<QString>("key");
        QTest::addColumn<QStringList>("showing");
        QTest::addColumn<QByteArray>("meaning");

        const QStringList home { u"homeInFront"_s };
        const QStringList app;
        const QStringList typing { u"keyboardShown"_s, u"textBoxFocused"_s };
        const QStringList hidden { u"textBoxFocused"_s };
        const QStringList menu { u"menuOpen"_s };
        const QStringList menuTyping { u"menuOpen"_s, u"textBoxFocused"_s, u"keyboardShown"_s };
        const QStringList screen { u"screenOpen"_s };
        const QStringList screenHome { u"screenOpen"_s, u"homeInFront"_s };
        const QStringList screenMenu { u"screenOpen"_s, u"menuOpen"_s };

        QTest::newRow("home at home") << u"home"_s << home << QByteArray("goHome");
        QTest::newRow("home in an app") << u"home"_s << app << QByteArray("goHome");
        QTest::newRow("home in the menu") << u"home"_s << menu << QByteArray("goHome");
        QTest::newRow("menu at home") << u"menu"_s << home << QByteArray("openMenu");
        QTest::newRow("menu in an app") << u"menu"_s << app << QByteArray("openMenu");
        QTest::newRow("menu in the menu") << u"menu"_s << menu << QByteArray("closeMenu");
        QTest::newRow("back at home") << u"back"_s << home << QByteArray("homeBack");
        QTest::newRow("back in an app") << u"back"_s << app << QByteArray("appBack");
        QTest::newRow("back while typing") << u"back"_s << typing << QByteArray("closeKeyboard");
        QTest::newRow("back, keyboard closed") << u"back"_s << hidden << QByteArray("appBack");
        QTest::newRow("back in the menu") << u"back"_s << menuTyping << QByteArray("closeMenu");
        QTest::newRow("back on a screen") << u"back"_s << screen << QByteArray("screenBack");
        QTest::newRow("back, screen at home")
            << u"back"_s << screenHome << QByteArray("screenBack");
        QTest::newRow("back, menu over a screen")
            << u"back"_s << screenMenu << QByteArray("closeMenu");
        QTest::newRow("backspace at home") << u"backspace"_s << home << QByteArray("homeBack");
        QTest::newRow("backspace in an app") << u"backspace"_s << app << QByteArray("appBack");
        QTest::newRow("backspace while typing")
            << u"backspace"_s << typing << QByteArray("deleteBackward");
        QTest::newRow("backspace, keyboard closed")
            << u"backspace"_s << hidden << QByteArray("deleteBackward");
        QTest::newRow("backspace in the menu")
            << u"backspace"_s << menuTyping << QByteArray("closeMenu");
        QTest::newRow("backspace on a screen")
            << u"backspace"_s << screen << QByteArray("screenBack");
        QTest::newRow("something unknown") << u"reboot"_s << home << QByteArray();
    }

    void meaning()
    {
        QFETCH(QString, key);
        QFETCH(QStringList, showing);
        QFETCH(QByteArray, meaning);

        KeyRouter router;
        router.setProperty("homeInFront", false);
        for (const QString &what : std::as_const(showing)) {
            QVERIFY(router.setProperty(qPrintable(what), true));
        }
        QList<QByteArray> seen;
        const QMetaObject *meta = router.metaObject();
        std::vector<std::unique_ptr<QSignalSpy>> spies;
        for (int i = meta->methodOffset(); i < meta->methodCount(); ++i) {
            const QMetaMethod method = meta->method(i);
            if (method.methodType() == QMetaMethod::Signal) {
                spies.push_back(std::make_unique<QSignalSpy>(&router, method));
            }
        }
        router.handle(key);
        for (const auto &spy : spies) {
            for (int i = 0; i < spy->count(); ++i) {
                seen << spy->signal().left(spy->signal().indexOf('('));
            }
        }
        QCOMPARE(seen, meaning.isEmpty() ? QList<QByteArray>() : QList<QByteArray> { meaning });
    }
};

QTEST_MAIN(TestKeys)
#include "tst_keys.moc"
