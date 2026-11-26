// SPDX-License-Identifier: GPL-3.0-or-later
#include "commandserver.h"
#include "shelllink.h"

#include <QFile>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

#include <memory>

using namespace Qt::StringLiterals;

class TestShellLink : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;

private Q_SLOTS:
    void init()
    {
        qputenv("XDG_RUNTIME_DIR", QFile::encodeName(m_dir.path()));
        qputenv("WAYLAND_DISPLAY", "wayland-test");
    }

    void agreesWithTheShellAboutTheSocket()
    {
        QCOMPARE(ShellLink::socketPath(), CommandServer::socketPath());
        QVERIFY(!ShellLink::socketPath().isEmpty());
        qputenv("WAYLAND_DISPLAY", "../elsewhere/wayland-0");
        QCOMPARE(ShellLink::socketPath(), QString());
        qunsetenv("WAYLAND_DISPLAY");
        QCOMPARE(ShellLink::socketPath(), QString());
        // Nowhere to go, and nothing breaks.
        ShellLink link { QString() };
        link.start();
        link.setState(true, true);
    }

    void saysWhoItIsAndWhatItShows()
    {
        QLocalServer shell;
        QVERIFY(shell.listen(ShellLink::socketPath()));
        ShellLink link(ShellLink::socketPath());
        link.setState(true, true);
        link.start();
        QTRY_VERIFY(shell.hasPendingConnections());
        const std::unique_ptr<QLocalSocket> socket(shell.nextPendingConnection());
        QByteArray sent;
        const auto read = [&sent, &socket] { return sent += socket->readAll(); };
        QTRY_VERIFY(read().endsWith("textbox 1\n"));
        QCOMPARE(sent, "keyboard\nshown 1\ntextbox 1\n");

        // Only what changed.
        link.setState(true, true);
        link.setState(false, true);
        link.setState(false, false);
        sent.clear();
        QTRY_VERIFY(read().endsWith("textbox 0\n"));
        QCOMPARE(sent, "shown 0\ntextbox 0\n");
    }

    void obeysTheShell()
    {
        CommandServer shell;
        QVERIFY(shell.listen());
        auto link = std::make_unique<ShellLink>(ShellLink::socketPath());
        QSignalSpy hide(link.get(), &ShellLink::hideAsked);
        QSignalSpy backspace(link.get(), &ShellLink::backspaceAsked);
        link->setState(true, true);
        link->start();
        QTRY_VERIFY(shell.keyboardShown());
        shell.hideKeyboard();
        shell.sendBackspace();
        shell.sendBackspace();
        QTRY_COMPARE(backspace.count(), 2);
        QCOMPARE(hide.count(), 1);
        link.reset();
        QTRY_VERIFY(!shell.keyboardShown());
    }

    void comesBackWhenTheShellDoes()
    {
        ShellLink link(ShellLink::socketPath());
        link.start();
        link.setState(true, true);
        {
            // It started before the shell.
            QLocalServer shell;
            QVERIFY(shell.listen(ShellLink::socketPath()));
            QTRY_VERIFY(shell.hasPendingConnections());
        }
        // It changes while the shell is away.
        link.setState(false, true);
        QLocalServer again;
        QVERIFY(again.listen(ShellLink::socketPath()));
        QTRY_VERIFY(again.hasPendingConnections());
        const std::unique_ptr<QLocalSocket> socket(again.nextPendingConnection());
        QByteArray sent;
        const auto read = [&sent, &socket] { return sent += socket->readAll(); };
        QTRY_VERIFY(read().endsWith("textbox 1\n"));
        QCOMPARE(sent, "keyboard\nshown 0\ntextbox 1\n");
    }

    void ignoresAnythingElse()
    {
        QLocalServer shell;
        QVERIFY(shell.listen(ShellLink::socketPath()));
        ShellLink link(ShellLink::socketPath());
        QSignalSpy hide(&link, &ShellLink::hideAsked);
        link.start();
        QTRY_VERIFY(shell.hasPendingConnections());
        const std::unique_ptr<QLocalSocket> socket(shell.nextPendingConnection());
        socket->write("hide me\n\nsomething new\nhide\n");
        socket->flush();
        QTRY_COMPARE(hide.count(), 1);
        // A line that never ends.
        socket->write(QByteArray(100'000, 'x'));
        socket->flush();
        QTRY_COMPARE(socket->state(), QLocalSocket::UnconnectedState);
        QCOMPARE(hide.count(), 1);
    }
};

QTEST_GUILESS_MAIN(TestShellLink)
#include "tst_shelllink.moc"
