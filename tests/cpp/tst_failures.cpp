// SPDX-License-Identifier: GPL-3.0-or-later
#include "failures.h"
#include "qrcode.h"

#include <QFile>
#include <QImage>
#include <QPainter>
#include <QRegularExpression>
#include <QSet>
#include <QTest>

using namespace Qt::StringLiterals;

class TestFailures : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void codesAreReadableAndUsedOnce()
    {
        static const QRegularExpression format(u"^HEARTH-[A-Z]+-[0-9]{3}$"_s);
        QSet<QString> seen;
        for (const QStringList codes = Failures::codes(); const QString &code : codes) {
            QVERIFY2(format.match(code).hasMatch(), qPrintable(code));
            QVERIFY2(!seen.contains(code), qPrintable(code));
            seen << code;
        }
    }

    void everyCodeSaysWhatHappenedAndWhatToDo()
    {
        for (const QStringList codes = Failures::codes(); const QString &code : codes) {
            const FailureText text = Failures::describe(code, u"The app"_s);
            QCOMPARE(text.code, code);
            // One sentence.
            QCOMPARE(text.what.count(u'.'), 1);
            QVERIFY2(text.what.endsWith(u'.'), qPrintable(text.what));
            QVERIFY(!text.what.contains(u'%'));
            QVERIFY(!text.next.isEmpty());
            QVERIFY(!text.next.contains(u'%'));
        }
        QCOMPARE(
            Failures::describe(u"HEARTH-APP-001"_s, u"Test"_s).what, u"Test couldn't start."_s);
    }

    void anAppsNameIsNeverReadAsAPattern()
    {
        const QString name = u"100%1 <b>Real</b> %2"_s;
        QCOMPARE(Failures::describe(u"HEARTH-APP-001"_s, name).what, name + u" couldn't start."_s);
    }

    void theHelpAddressHoldsTheCodeAndVersionOnly()
    {
        const QUrl help = Failures::describe(u"HEARTH-APP-001"_s, u"Secret name"_s).help;
        QCOMPARE(help.toString(),
            u"https://github.com/inklikeurine/hearth/wiki/HEARTH-APP-001?version=" EXPECTED_VERSION
            ""_s);
    }

    void anUnknownCodeStillSaysSomething()
    {
        const FailureText text = Failures::describe(u"HEARTH-NOPE-999"_s);
        QCOMPARE(text.code, u"HEARTH-SHELL-000"_s);
        QVERIFY(!text.what.isEmpty());
    }

    void theListInTheDocsIsTheSame()
    {
        QFile docs(QStringLiteral(ERROR_CODES_DOC));
        QVERIFY(docs.open(QIODevice::ReadOnly));
        static const QRegularExpression row(
            u"^\\| (HEARTH-[A-Z]+-[0-9]{3}) \\|"_s, QRegularExpression::MultilineOption);
        QStringList listed;
        auto rows = row.globalMatch(QString::fromUtf8(docs.readAll()));
        while (rows.hasNext()) {
            listed << rows.next().captured(1);
        }
        QCOMPARE(listed, Failures::codes());
    }

    void qrCodeIsBlackOnWhiteWithAnEdge()
    {
        QrCode code;
        code.setText(u"https://example.org/"_s);
        QVERIFY(code.modules() >= 21);
        code.setSize({ 400, 400 });
        QImage image(400, 400, QImage::Format_RGB32);
        image.fill(Qt::red);
        QPainter painter(&image);
        code.paint(&painter);
        painter.end();
        const int module = 400 / (code.modules() + 8);
        const int corner = (400 - module * code.modules()) / 2;
        QCOMPARE(image.pixelColor(0, 0), QColor(Qt::white));
        QCOMPARE(image.pixelColor(corner - 1, corner - 1), QColor(Qt::white));
        // The square in a QR code's corner starts with a dark line.
        QCOMPARE(image.pixelColor(corner + 1, corner + 1), QColor(Qt::black));
        QCOMPARE(image.pixelColor(corner + 6 * module + 1, corner + 1), QColor(Qt::black));
        QCOMPARE(image.pixelColor(corner + module + 1, corner + module + 1), QColor(Qt::white));
    }

    void qrCodeSaysWhenTheTextIsTooLong()
    {
        QrCode code;
        code.setText(QString(10'000, u'x'));
        QCOMPARE(code.modules(), 0);
    }
};

QTEST_MAIN(TestFailures)
#include "tst_failures.moc"
