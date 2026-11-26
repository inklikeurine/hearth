// SPDX-License-Identifier: GPL-3.0-or-later
#include "themetokens.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMetaProperty>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

class TestTheme : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;

    // Writes a theme file. The tests write its JSON with single quotes.
    QString file(QByteArray json)
    {
        json.replace('\'', '"');
        const QString path = m_dir.filePath(u"theme.json"_s);
        QFile out(path);
        if (!out.open(QIODevice::WriteOnly) || out.write(json) != json.size()) {
            qFatal("can't write the test theme");
        }
        return path;
    }

private Q_SLOTS:
    void contrastMatchesWcag()
    {
        QCOMPARE(qRound(ThemeTokens::contrast(Qt::white, Qt::black)), 21);
        QCOMPARE(ThemeTokens::contrast(Qt::red, Qt::red), 1.0);
        // #767676 is the well-known lightest grey that passes on white.
        QVERIFY(ThemeTokens::contrast(QColor(u"#767676"_s), Qt::white) >= 4.5);
        QVERIFY(ThemeTokens::contrast(QColor(u"#777777"_s), Qt::white) < 4.5);
    }

    void builtinThemesLoadCleanly_data()
    {
        QTest::addColumn<QString>("name");
        const QStringList names = ThemeTokens::builtinThemes();
        for (const QString &name : names) {
            QTest::newRow(qPrintable(name)) << name;
        }
    }

    // Loading refuses anything below WCAG AA, so a clean load is the contrast check.
    void builtinThemesLoadCleanly()
    {
        QFETCH(QString, name);
        ThemeTokens tokens;
        QVERIFY2(tokens.loadBuiltin(name), qPrintable(tokens.problems().join(u"; "_s)));
        QCOMPARE(tokens.problems(), QStringList());
    }

    void darkSetsEveryToken()
    {
        QFile dark(u":/hearth/themes/dark.json"_s);
        QVERIFY(dark.open(QIODevice::ReadOnly));
        const QJsonObject theme = QJsonDocument::fromJson(dark.readAll()).object();
        const auto missing = [](const QMetaObject &meta, const QJsonObject &group) {
            QStringList names;
            for (int i = 0; i < meta.propertyCount(); ++i) {
                const QString name = QString::fromLatin1(meta.property(i).name());
                if (name != "easing"_L1 && !group.contains(name)) {
                    names << name;
                }
            }
            return names;
        };
        QCOMPARE(
            missing(ThemeColors::staticMetaObject, theme["colors"_L1].toObject()), QStringList());
        QCOMPARE(
            missing(ThemeSizes::staticMetaObject, theme["sizes"_L1].toObject()), QStringList());
        QCOMPARE(
            missing(ThemeMotion::staticMetaObject, theme["motion"_L1].toObject()), QStringList());
        QCOMPARE(
            missing(ThemeGlass::staticMetaObject, theme["glass"_L1].toObject()), QStringList());
    }

    void fontIsInstalled()
    {
        ThemeTokens tokens;
        QCOMPARE(tokens.fontFamily(), u"Atkinson Hyperlegible Next"_s);
    }

    void missingTokensComeFromDark()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.loadBuiltin(u"high-contrast"_s));
        QVERIFY(tokens.loadFile(file("{'colors': {'accent': '#ffffff'}}")));
        QCOMPARE(tokens.colors().accent, QColor(Qt::white));
        QCOMPARE(tokens.colors().background, ThemeTokens().colors().background);
        QCOMPARE(tokens.sizes().outline, 0);
    }

    void badValuesAreIgnored_data()
    {
        QTest::addColumn<QByteArray>("json");
        QTest::newRow("unknown token") << QByteArray("{'colors': {'onclick': '#ffffff'}}");
        QTest::newRow("script") << QByteArray("{'colors': {'text': 'Qt.quit();'}}");
        QTest::newRow("see-through colour") << QByteArray("{'colors': {'text': '#00ffffff'}}");
        QTest::newRow("trailing newline") << QByteArray("{'colors': {'text': '#ffffff\\n'}}");
        QTest::newRow("colour as number") << QByteArray("{'colors': {'text': 7}}");
        QTest::newRow("size as text") << QByteArray("{'sizes': {'gap': '24'}}");
        QTest::newRow("easing") << QByteArray("{'motion': {'easing': [9]}}");
        QTest::newRow("font not installed") << QByteArray("{'font': 'No Such Font'}");
        QTest::newRow("group isn't an object") << QByteArray("{'colors': []}");
    }

    void badValuesAreIgnored()
    {
        QFETCH(QByteArray, json);
        const ThemeTokens dark;
        ThemeTokens tokens;
        QVERIFY(tokens.loadFile(file(json)));
        QCOMPARE(tokens.problems().size(), 1);
        QCOMPARE(tokens.colors().text, dark.colors().text);
        QCOMPARE(tokens.sizes().gap, dark.sizes().gap);
        QCOMPARE(tokens.motion().easing, dark.motion().easing);
        QCOMPARE(tokens.fontFamily(), dark.fontFamily());
    }

    void outOfRangeSizesAreCorrected()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.loadFile(
            file("{'sizes': {'textSmall': 2, 'textBody': 9000}, 'motion': {'duration': -5}}")));
        QCOMPARE(tokens.sizes().textSmall, 24);
        QCOMPARE(tokens.sizes().textBody, 64);
        QCOMPARE(tokens.motion().duration, 0);
        QCOMPARE(tokens.problems().size(), 3);
    }

    void glassStaysReadable()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.loadFile(file("{'glass': {'opacity': 0.1, 'blur': 500}}")));
        QCOMPARE(tokens.glass().opacity, 0.6);
        QCOMPARE(tokens.glass().blur, 64);
        QCOMPARE(tokens.problems().size(), 2);
    }

    void reduceTransparencyMakesPanelsSolid()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.glass().opacity < 1.0);
        tokens.setScreenHeight(2160);
        QCOMPARE(tokens.glass().blur, 96);
        QSignalSpy changed(&tokens, &ThemeTokens::changed);
        tokens.setReduceTransparency(true);
        QCOMPARE(changed.count(), 1);
        QCOMPARE(tokens.glass().opacity, 1.0);
        QCOMPARE(tokens.glass().blur, 0);
        // A user setting: a new theme keeps it.
        QVERIFY(tokens.loadBuiltin(u"dark"_s));
        QCOMPARE(tokens.glass().opacity, 1.0);
    }

    void highContrastIsSolid()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.loadBuiltin(u"high-contrast"_s));
        QCOMPARE(tokens.glass().opacity, 1.0);
        QCOMPARE(tokens.glass().blur, 0);
    }

    void refusedThemesChangeNothing_data()
    {
        QTest::addColumn<QByteArray>("json");
        QTest::newRow("not JSON") << QByteArray("import QtQuick");
        QTest::newRow("empty") << QByteArray();
        QTest::newRow("too big") << QByteArray(70000, ' ');
        QTest::newRow("unreadable text") << QByteArray("{'colors': {'text': '#111111'}}");
        QTest::newRow("invisible ring") << QByteArray("{'colors': {'focusRing': '#101217'}}");
        QTest::newRow("invisible outline")
            << QByteArray("{'colors': {'outline': '#101217'}, 'sizes': {'outline': 3}}");
    }

    void refusedThemesChangeNothing()
    {
        QFETCH(QByteArray, json);
        ThemeTokens tokens;
        QVERIFY(tokens.loadBuiltin(u"high-contrast"_s));
        QSignalSpy changed(&tokens, &ThemeTokens::changed);
        QVERIFY(!tokens.loadFile(file(json)));
        QVERIFY(!tokens.problems().isEmpty());
        QCOMPARE(changed.count(), 0);
        QCOMPARE(tokens.colors().accent, QColor(u"#ffff00"_s));
    }

    void onlyBuiltinNamesLoad()
    {
        ThemeTokens tokens;
        QVERIFY(!tokens.loadBuiltin(u"../../etc/passwd"_s));
    }

    void settingsScaleAndSurviveAThemeChange()
    {
        ThemeTokens tokens;
        tokens.setScreenHeight(2160);
        tokens.setTextScale(1.5);
        tokens.setReducedMotion(true);
        QCOMPARE(tokens.sizes().textBody, 96);
        QCOMPARE(tokens.sizes().gap, 48);
        QVERIFY(tokens.loadBuiltin(u"high-contrast"_s));
        QCOMPARE(tokens.sizes().textBody, 96);
        QCOMPARE(tokens.motion().focusScale, 1.0);
        tokens.setTextScale(40);
        QCOMPARE(tokens.textScale(), 1.5);
    }

    void thinLinesNeverVanish()
    {
        ThemeTokens tokens;
        QVERIFY(tokens.loadBuiltin(u"high-contrast"_s));
        tokens.setScreenHeight(90);
        QCOMPARE(tokens.sizes().outline, 1);
    }
};

QTEST_MAIN(TestTheme)
#include "tst_theme.moc"
