// SPDX-License-Identifier: GPL-3.0-or-later
#include "applauncher.h"
#include "menucontributions.h"

#include <QSignalSpy>
#include <QTest>

using namespace Qt::StringLiterals;

// docs/menu-api.md, line by line.
class TestMenuApi : public QObject {
    Q_OBJECT

private:
    // Single quotes: moc can't read strings full of double ones.
    static QByteArray json(QByteArray text)
    {
        return text.replace('\'', '"');
    }
    static QByteArray menu(const QByteArray &items)
    {
        return json("{'version':1,'items':[" + items + "]}");
    }

private Q_SLOTS:
    void readsTheExample()
    {
        const auto items = MenuContributions::parse(
            menu("{'id':'next','type':'action','label':'Next episode'},"
                 "{'id':'subtitles','type':'toggle','label':'Subtitles','on':true},"
                 "{'id':'audio','type':'choice','label':'Audio',"
                 "'options':['English','French'],'current':1,'enabled':false}"));
        QVERIFY(items);
        QCOMPARE(items->size(), 3);
        QCOMPARE(items->at(0).toMap().value(u"label"_s), u"Next episode"_s);
        QCOMPARE(items->at(0).toMap().value(u"enabled"_s), true);
        QCOMPARE(items->at(1).toMap().value(u"on"_s), true);
        QCOMPARE(items->at(2).toMap().value(u"current"_s), 1);
        QCOMPARE(items->at(2).toMap().value(u"enabled"_s), false);
    }

    void refusesAWholeMenu_data()
    {
        QTest::addColumn<QByteArray>("menu");
        QTest::newRow("nothing") << QByteArray();
        QTest::newRow("not JSON") << QByteArray("hello");
        QTest::newRow("a list") << json("[{'id':'a'}]");
        QTest::newRow("no version") << json("{'items':[]}");
        QTest::newRow("a later version") << json("{'version':2,'items':[]}");
        QTest::newRow("no items") << json("{'version':1}");
        QByteArray many;
        for (int i = 0; i < 13; ++i) {
            many += (i ? "," : "") + QByteArray("{'id':'") + QByteArray::number(i)
                + "','type':'action','label':'x'}";
        }
        QTest::newRow("too many items") << menu(many);
        QTest::newRow("too long") << menu(QByteArray(9000, ' '));
        QTest::newRow("very deep") << QByteArray(8000, '[');
    }

    void refusesAWholeMenu()
    {
        QFETCH(QByteArray, menu);
        QVERIFY(!MenuContributions::parse(menu));
    }

    void leavesOutBadItems_data()
    {
        QTest::addColumn<QByteArray>("item");
        QTest::newRow("unknown type") << QByteArray("{'id':'a','type':'html','label':'A'}");
        QTest::newRow("no ID") << QByteArray("{'type':'action','label':'A'}");
        QTest::newRow("long ID") << "{'id':'" + QByteArray(41, 'i')
                + "','type':'action','label':'A'}";
        QTest::newRow("no label") << QByteArray("{'id':'a','type':'action'}");
        QTest::newRow("empty label") << QByteArray("{'id':'a','type':'action','label':'  '}");
        QTest::newRow("toggle with no state")
            << QByteArray("{'id':'a','type':'toggle','label':'A'}");
        QTest::newRow("choice with no options")
            << QByteArray("{'id':'a','type':'choice','label':'A','options':[],'current':0}");
        QTest::newRow("choice off the end")
            << QByteArray("{'id':'a','type':'choice','label':'A','options':['x'],'current':1}");
        QTest::newRow("same ID") << QByteArray("{'id':'good','type':'action','label':'A'}");
        QTest::newRow("not an object") << QByteArray("'a'");
        QByteArray options("'x'");
        for (int i = 0; i < 20; ++i) {
            options += ",'x'";
        }
        QTest::newRow("too many options")
            << "{'id':'a','type':'choice','label':'A','current':0,'options':[" + options + "]}";
    }

    void leavesOutBadItems()
    {
        QFETCH(QByteArray, item);
        const auto items = MenuContributions::parse(
            menu("{'id':'good','type':'action','label':'Good'}," + item));
        QVERIFY(items);
        QCOMPARE(items->size(), 1);
        QCOMPARE(items->at(0).toMap().value(u"label"_s), u"Good"_s);
    }

    void labelsAreOneShortLine()
    {
        const auto items = MenuContributions::parse(
            menu("{'id':'a','type':'action','label':'One\\ntwo " + QByteArray(100, 'x') + "'}"));
        QVERIFY(items);
        const QString label = items->at(0).toMap().value(u"label"_s).toString();
        QCOMPARE(label.size(), 40);
        QVERIFY(label.startsWith(u"One two x"));
    }

    void onlyARunningAppCanAdd()
    {
        MenuContributions menuItems;
        QCOMPARE(menuItems.set({ }, u":1.1"_s, menu("")), MenuContributions::Refused);
        QCOMPARE(menuItems.set(u"app"_s, u":1.1"_s, menu("")), MenuContributions::Accepted);
        QCOMPARE(menuItems.appId(), u"app"_s);
    }

    void aBadMenuKeepsTheOneBefore()
    {
        MenuContributions menuItems;
        const QByteArray good = menu("{'id':'a','type':'action','label':'A'}");
        menuItems.set(u"app"_s, u":1.1"_s, good);
        QCOMPARE(menuItems.set(u"app"_s, u":1.1"_s, "bad"), MenuContributions::Refused);
        QCOMPARE(menuItems.items().size(), 1);
    }

    void tooManyMenusAreRefused()
    {
        MenuContributions menuItems;
        for (int i = 0; i < 10; ++i) {
            QCOMPARE(menuItems.set(u"app"_s, u":1.1"_s, menu("")), MenuContributions::Accepted);
        }
        QCOMPARE(menuItems.set(u"app"_s, u":1.1"_s, menu("")), MenuContributions::TooOften);
    }

    void itemsGoWithTheirOwnProgramOnly()
    {
        MenuContributions menuItems;
        menuItems.set(u"app"_s, u":1.1"_s, menu("{'id':'a','type':'action','label':'A'}"));
        menuItems.clear(u":1.2"_s);
        QCOMPARE(menuItems.items().size(), 1);
        menuItems.clear(u":1.1"_s);
        QCOMPARE(menuItems.items().size(), 0);
        QCOMPARE(menuItems.appId(), QString());
    }

    void choosingOnlyAsks()
    {
        MenuContributions menuItems;
        menuItems.set(
            u"app"_s, u":1.1"_s, menu("{'id':'a','type':'toggle','label':'A','on':false}"));
        QSignalSpy chosen(&menuItems, &MenuContributions::chosen);
        menuItems.choose(u"a"_s, true);
        QCOMPARE(chosen.at(0), (QVariantList { u":1.1"_s, u"a"_s, true }));
        QCOMPARE(menuItems.items().at(0).toMap().value(u"on"_s), false);
    }

    void aProgramBelongsToTheAppThatStartedIt()
    {
        AppLauncher launcher;
        launcher.start({ u"one"_s, u"One"_s, { }, { u"sleep"_s, u"60"_s }, { } });
        launcher.start({ u"two"_s, u"Two"_s, { }, { u"sh"_s, u"-c"_s, u"sleep 61; true"_s }, { } });
        // The shell's own programs belong to no app.
        QCOMPARE(launcher.appOfProcess(QCoreApplication::applicationPid()), QString());
        QCOMPARE(launcher.appOfProcess(0), QString());
        QCOMPARE(launcher.appOfProcess(-1), QString());
        QProcess find;
        QByteArray pid;
        // "sleep 61" was started by sh, not by the launcher.
        const auto found = [&find, &pid] {
            find.start(u"pgrep"_s, { u"-f"_s, u"-x"_s, u"sleep 61"_s, u"-n"_s });
            find.waitForFinished();
            pid = find.readAllStandardOutput().trimmed();
            return !pid.isEmpty();
        };
        QTRY_VERIFY(found());
        QCOMPARE(launcher.appOfProcess(pid.toInt()), u"two"_s);
    }
};

QTEST_GUILESS_MAIN(TestMenuApi)
#include "tst_menuapi.moc"
