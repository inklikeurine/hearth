// SPDX-License-Identifier: GPL-3.0-or-later
#include <QKeyEvent>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickWindow>
#include <QSignalSpy>
#include <QTest>

// A remote's button held down sends the same key again and again.
class TestHeldKeys : public QObject {
    Q_OBJECT

private Q_SLOTS:
    void heldSelectPressesOnce()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData("import Hearth.Controls\nButton { text: \"OK\" }", QUrl());
        QScopedPointer<QObject> object(component.create());
        auto *button = qobject_cast<QQuickItem *>(object.data());
        QVERIFY2(button, qPrintable(component.errorString()));

        QQuickWindow window;
        button->setParentItem(window.contentItem());
        window.show();
        QVERIFY(QTest::qWaitForWindowActive(&window));
        button->forceActiveFocus();
        QSignalSpy clicked(button, SIGNAL(clicked()));

        const auto press = [&window](bool repeat) {
            QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier, { }, repeat);
            QCoreApplication::sendEvent(&window, &event);
        };
        press(false);
        for (int i = 0; i < 20; ++i) {
            press(true);
        }
        QCOMPARE(clicked.count(), 1);
    }
};

QTEST_MAIN(TestHeldKeys)
#include "tst_heldkeys.moc"
