// SPDX-License-Identifier: GPL-3.0-or-later
#include "applist.h"

#include <QAbstractItemModelTester>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

class TestApps : public QObject {
    Q_OBJECT

private:
    QScopedPointer<QTemporaryDir> m_dir;

    QString write(const QString &name, const QByteArray &lines)
    {
        const QString path = m_dir->filePath(name);
        QDir().mkpath(QFileInfo(path).path());
        QFile file(path);
        const QByteArray text = "[Desktop Entry]\n" + lines + '\n';
        if (!file.open(QIODevice::WriteOnly) || file.write(text) != text.size()) {
            qFatal("can't write the test's .desktop file");
        }
        return path;
    }

    static QByteArray app(const QByteArray &name, const QByteArray &more = { })
    {
        return "Type=Application\nName=" + name + "\nExec=prog\n" + more;
    }

    static QStringList names(const AppList &list)
    {
        QStringList out;
        for (const DesktopEntry &entry : list.apps()) {
            out << entry.name;
        }
        return out;
    }

private Q_SLOTS:
    void init()
    {
        m_dir.reset(new QTemporaryDir);
        ShellSettings().setAppOrder({ });
    }

    void splitsExecByTheSpec_data()
    {
        QTest::addColumn<QString>("exec");
        QTest::addColumn<QStringList>("expected");
        QTest::newRow("plain") << u"prog -a  b"_s << QStringList { u"prog"_s, u"-a"_s, u"b"_s };
        QTest::newRow("quoted") << u"prog \"two words\""_s
                                << QStringList { u"prog"_s, u"two words"_s };
        QTest::newRow("empty argument") << u"prog \"\""_s << QStringList { u"prog"_s, QString() };
        QTest::newRow("escapes in quotes")
            << u"sh -c \"a \\\"b\\\" \\$c\""_s << QStringList { u"sh"_s, u"-c"_s, u"a \"b\" $c"_s };
        QTest::newRow("shell characters are just text")
            << u"prog ;rm $(x)"_s << QStringList { u"prog"_s, u";rm"_s, u"$(x)"_s };
    }

    void splitsExecByTheSpec()
    {
        QFETCH(QString, exec);
        QFETCH(QStringList, expected);
        const auto split = DesktopEntry::splitExec(exec);
        QVERIFY(split);
        QCOMPARE(*split, expected);
    }

    void refusesBrokenExec()
    {
        QVERIFY(!DesktopEntry::splitExec(u"prog \"never closed"_s));
        QVERIFY(!DesktopEntry::splitExec(u"prog \"ends in \\"_s));
    }

    void fillsInFieldCodes()
    {
        const QString path = write(u"a.desktop"_s,
            "Type=Application\nName=My App\nIcon=pic\n"
            "Exec=prog %u %F --name=%c %i 100%% --open=%u");
        const auto entry = DesktopEntry::read(path, u"a"_s, u"en_US"_s);
        QVERIFY(entry);
        QCOMPARE(entry->command,
            QStringList(
                { u"prog"_s, u"--name=My App"_s, u"--icon"_s, u"pic"_s, u"100%"_s, u"--open="_s }));
    }

    void readsLocalizedNames()
    {
        const QString path
            = write(u"a.desktop"_s, app("Files", "Name[pt]=Ficheiros\nName[pt_BR]=Arquivos\n"));
        QVERIFY(DesktopEntry::read(path, u"a"_s, { }));
        QCOMPARE(DesktopEntry::read(path, u"a"_s, u"pt_BR"_s)->name, u"Arquivos"_s);
        QCOMPARE(DesktopEntry::read(path, u"a"_s, u"pt_PT"_s)->name, u"Ficheiros"_s);
        QCOMPARE(DesktopEntry::read(path, u"a"_s, u"de_DE"_s)->name, u"Files"_s);
    }

    void hidesWhatIsntForHearth_data()
    {
        QTest::addColumn<QByteArray>("lines");
        QTest::newRow("link") << QByteArray("Type=Link\nName=x\nExec=prog");
        QTest::newRow("no display") << app("x", "NoDisplay=true");
        QTest::newRow("hidden") << app("x", "Hidden=true");
        QTest::newRow("terminal") << app("x", "Terminal=true");
        QTest::newRow("another desktop's") << app("x", "OnlyShowIn=KDE;GNOME;");
        QTest::newRow("not in Hearth") << app("x", "NotShowIn=Hearth;");
        QTest::newRow("program missing") << app("x", "TryExec=/nowhere/prog");
        QTest::newRow("no name") << QByteArray("Type=Application\nExec=prog");
        QTest::newRow("no Exec") << QByteArray("Type=Application\nName=x");
        QTest::newRow("only a file code") << QByteArray("Type=Application\nName=x\nExec=%f");
    }

    void hidesWhatIsntForHearth()
    {
        QFETCH(QByteArray, lines);
        QVERIFY(!DesktopEntry::read(write(u"a.desktop"_s, lines), u"a"_s, { }));
    }

    void ignoresOtherGroups()
    {
        const QString path
            = write(u"a.desktop"_s, app("Right", "[Desktop Action new]\nName=Wrong\nExec=other"));
        const auto entry = DesktopEntry::read(path, u"a"_s, { });
        QVERIFY(entry);
        QCOMPARE(entry->name, u"Right"_s);
        QCOMPARE(entry->command, QStringList(u"prog"_s));
    }

    void listsByNameAndEarlierFoldersWin()
    {
        write(u"user/b.desktop"_s, app("Mine"));
        write(u"user/hidden.desktop"_s, app("x", "Hidden=true"));
        write(u"system/b.desktop"_s, app("Theirs"));
        write(u"system/hidden.desktop"_s, app("Should stay hidden"));
        write(u"system/kde/a.desktop"_s, app("Another"));
        AppList list;
        list.setFolders({ m_dir->filePath(u"user"_s), m_dir->filePath(u"system"_s) });
        list.componentComplete();
        QCOMPARE(names(list), QStringList({ u"Another"_s, u"Mine"_s }));
        QCOMPARE(list.apps().constFirst().id, u"kde-a"_s);
    }

    void keepsTheProfilesOrderAndAddsNewAppsAtTheEnd()
    {
        write(u"a.desktop"_s, app("A"));
        write(u"b.desktop"_s, app("B"));
        write(u"c.desktop"_s, app("C"));
        ShellSettings settings;
        {
            AppList list;
            const QAbstractItemModelTester tester(&list);
            list.setFolders({ m_dir->path() });
            list.setSettings(&settings);
            list.componentComplete();
            list.moveApp(2, 0);
            QCOMPARE(names(list), QStringList({ u"C"_s, u"A"_s, u"B"_s }));
            list.moveApp(0, 1);
            list.moveApp(5, 0);
            list.moveApp(0, -1);
            QCOMPARE(names(list), QStringList({ u"A"_s, u"C"_s, u"B"_s }));
        }
        write(u"0.desktop"_s, app("0 comes first by name"));
        AppList list;
        list.setFolders({ m_dir->path() });
        list.setSettings(&settings);
        list.componentComplete();
        QCOMPARE(names(list), QStringList({ u"A"_s, u"C"_s, u"B"_s, u"0 comes first by name"_s }));
    }

    void reinstalledAppsComeBackWhereTheyWere()
    {
        write(u"a.desktop"_s, app("A"));
        const QString b = write(u"b.desktop"_s, app("B"));
        write(u"c.desktop"_s, app("C"));
        ShellSettings settings;
        AppList list;
        list.setFolders({ m_dir->path() });
        list.setSettings(&settings);
        list.componentComplete();
        QVERIFY(QFile::remove(b));
        QTRY_COMPARE(names(list), QStringList({ u"A"_s, u"C"_s }));
        write(u"b.desktop"_s, app("B"));
        QTRY_COMPARE(names(list), QStringList({ u"A"_s, u"B"_s, u"C"_s }));
    }

    void settingsStayInRange()
    {
        ShellSettings settings;
        settings.setColumns(900);
        QCOMPARE(settings.columns(), 7);
        settings.setColumns(5);
        settings.setRows(0);
        QCOMPARE(settings.rows(), 1);
    }
};

QTEST_MAIN(TestApps)
#include "tst_apps.moc"
