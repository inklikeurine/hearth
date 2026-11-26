// SPDX-License-Identifier: GPL-3.0-or-later
#include "keyboardsettings.h"

#include <QDir>
#include <QFile>
#include <QSettings>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

class TestSettings : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;

    [[nodiscard]] QString path(const QString &name = u"keyboard"_s) const
    {
        return m_dir.filePath(u"hearth/%1.ini"_s.arg(name));
    }
    void write(const QByteArray &text, const QString &name = u"keyboard"_s)
    {
        QVERIFY(QDir(m_dir.path()).mkpath(u"hearth"_s));
        QFile file(path(name));
        QVERIFY(file.open(QIODevice::WriteOnly));
        file.write(text);
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Once: QSettings looks at it only the first time.
        qputenv("XDG_CONFIG_HOME", QFile::encodeName(m_dir.path()));
    }
    void init()
    {
        QDir(m_dir.filePath(u"hearth"_s)).removeRecursively();
    }

    void reads_data()
    {
        QTest::addColumn<QByteArray>("text");
        QTest::addColumn<LayoutKind>("layout");
        QTest::newRow("qwerty") << QByteArray("layout=qwerty\n") << LayoutKind::Qwerty;
        QTest::newRow("alphabet") << QByteArray("layout=alphabet\n") << LayoutKind::Alphabet;
        QTest::newRow("row") << QByteArray("layout=row\n") << LayoutKind::Row;
        QTest::newRow("wheel") << QByteArray("layout=wheel\n") << LayoutKind::Wheel;
        QTest::newRow("under a heading")
            << QByteArray("[General]\nlayout=row\n") << LayoutKind::Row;
        QTest::newRow("unknown") << QByteArray("layout=dvorak\n") << LayoutKind::Qwerty;
        QTest::newRow("a list") << QByteArray("layout=row,wheel\n") << LayoutKind::Qwerty;
        QTest::newRow("nothing") << QByteArray("") << LayoutKind::Qwerty;
        QTest::newRow("rubbish") << QByteArray("\0\xff[[[=\n", 8) << LayoutKind::Qwerty;
    }
    void reads()
    {
        QFETCH(QByteArray, text);
        QFETCH(LayoutKind, layout);
        write(text);
        const KeyboardSettings settings;
        QCOMPARE(settings.layout(), layout);
    }

    void noFile()
    {
        const KeyboardSettings settings;
        QCOMPARE(settings.layout(), LayoutKind::Qwerty);
        // Reading it doesn't make one.
        QVERIFY(!QFile::exists(path()));
    }

    void picksUpEveryChange()
    {
        KeyboardSettings settings;
        QSignalSpy changed(&settings, &KeyboardSettings::layoutChanged);
        // The folder didn't exist.
        write("layout=row\n");
        QTRY_COMPARE(settings.layout(), LayoutKind::Row);
        // Written in place.
        write("layout=alphabet\n");
        QTRY_COMPARE(settings.layout(), LayoutKind::Alphabet);
        // Replaced by a new file, as QSettings does.
        {
            QSettings file(path(), QSettings::IniFormat);
            file.setValue(u"layout"_s, u"wheel"_s);
        }
        QTRY_COMPARE(settings.layout(), LayoutKind::Wheel);
        QVERIFY(QFile::remove(path()));
        QTRY_COMPARE(settings.layout(), LayoutKind::Qwerty);
        write("layout=row\n");
        QTRY_COMPARE(settings.layout(), LayoutKind::Row);
        QVERIFY(changed.count() >= 5);
    }

    // What the shell's settings say, with ShellSettings's defaults and limits.
    void looksAsTheShellDoes()
    {
        KeyboardSettings settings;
        QCOMPARE(settings.theme(), u"dark"_s);
        QCOMPARE(settings.textScale(), 1.0);
        QVERIFY(!settings.reducedMotion());
        QVERIFY(!settings.ignoresDoublePresses());
        QSignalSpy look(&settings, &KeyboardSettings::lookChanged);
        QSignalSpy presses(&settings, &KeyboardSettings::ignoreDoublePressesChanged);
        QSignalSpy layout(&settings, &KeyboardSettings::layoutChanged);
        {
            // As the shell writes it.
            QSettings shell(path(u"shell"_s), QSettings::IniFormat);
            shell.setValue(u"look/theme"_s, u"high-contrast"_s);
            shell.setValue(u"look/textPercent"_s, 130);
            shell.setValue(u"look/reducedMotion"_s, true);
            shell.setValue(u"keys/ignoreDoublePresses"_s, true);
        }
        QTRY_COMPARE(settings.theme(), u"high-contrast"_s);
        QCOMPARE(settings.textScale(), 1.3);
        QVERIFY(settings.reducedMotion());
        QVERIFY(settings.ignoresDoublePresses());
        QVERIFY(look.count() >= 1);
        QVERIFY(presses.count() >= 1);
        QCOMPARE(layout.count(), 0);
        write("[look]\ntextPercent=900\n", u"shell"_s);
        QTRY_COMPARE(settings.textScale(), 1.5);
        QCOMPARE(settings.theme(), u"dark"_s);
    }
};

QTEST_GUILESS_MAIN(TestSettings)
#include "tst_settings.moc"
