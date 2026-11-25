// SPDX-License-Identifier: GPL-3.0-or-later
#include "commandserver.h"

#include <QFileInfo>
#include <QLocalSocket>
#include <QProcess>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QTest>

using namespace Qt::StringLiterals;

class TestCommands : public QObject {
    Q_OBJECT

private:
    QTemporaryDir m_dir;

    // Runs hearth-send as labwc would. Returns its answer.
    QByteArray send(const QStringList &arguments, int *status = nullptr)
    {
        QProcess process;
        process.start(QStringLiteral(HEARTH_SEND), arguments);
        // Not waitForFinished(): the shell it talks to is this program.
        const bool ended
            = QTest::qWaitFor([&process] { return process.state() == QProcess::NotRunning; }, 5000);
        if (status) {
            *status = ended ? process.exitCode() : -1;
        }
        return process.readAllStandardOutput();
    }

private Q_SLOTS:
    void initTestCase()
    {
        qputenv("XDG_RUNTIME_DIR", QFile::encodeName(m_dir.path()));
        qputenv("WAYLAND_DISPLAY", "wayland-test");
    }

    void onlyThisUserCanConnect()
    {
        CommandServer server;
        QVERIFY(server.listen());
        const auto others = QFileDevice::ReadGroup | QFileDevice::WriteGroup
            | QFileDevice::ReadOther | QFileDevice::WriteOther;
        QVERIFY(!(QFileInfo(CommandServer::socketPath()).permissions() & others));
    }

    void passesCommandsOn()
    {
        CommandServer server;
        QVERIFY(server.listen());
        QSignalSpy command(&server, &CommandServer::command);
        int status = -1;
        send({ u"back"_s }, &status);
        QCOMPARE(status, 0);
        QTRY_COMPARE(command.count(), 1);
        QCOMPARE(command.at(0).at(0).toString(), u"back"_s);
    }

    void answersState()
    {
        CommandServer server;
        QVERIFY(server.listen());
        server.setProperty("state", QVariantMap { { u"home"_s, true } });
        QCOMPARE(send({ u"state"_s }), "{\"home\":true}\n");
    }

    void sendFailsWithNoShell()
    {
        int status = 0;
        send({ u"back"_s }, &status);
        QCOMPARE(status, 1);
    }

    // A display name with a folder in it would reach a socket somewhere else.
    void refusesOddDisplayNames()
    {
        qputenv("WAYLAND_DISPLAY", "../elsewhere/wayland-0");
        QCOMPARE(CommandServer::socketPath(), QString());
        CommandServer server;
        QVERIFY(!server.listen());
        int status = 0;
        send({ u"back"_s }, &status);
        QCOMPARE(status, 1);
        qputenv("WAYLAND_DISPLAY", "wayland-test");
    }

    void takesOverFromAShellThatCrashed()
    {
        {
            QFile left(CommandServer::socketPath());
            QVERIFY(left.open(QIODevice::WriteOnly));
        }
        CommandServer server;
        QVERIFY(server.listen());
    }

    void knowsTheKeyboard()
    {
        CommandServer server;
        QVERIFY(server.listen());
        QSignalSpy command(&server, &CommandServer::command);
        QLocalSocket keyboard;
        keyboard.connectToServer(CommandServer::socketPath());
        QVERIFY(keyboard.waitForConnected());
        keyboard.write("keyboard\nshown 1\ntextbox 1\n");
        keyboard.flush();
        QTRY_VERIFY(server.keyboardShown());
        QTRY_VERIFY(server.textBoxFocused());

        server.hideKeyboard();
        server.sendBackspace();
        QTRY_COMPARE(keyboard.bytesAvailable(), 15);
        QCOMPARE(keyboard.readAll(), "hide\nbackspace\n");

        keyboard.write("shown 0\n");
        keyboard.flush();
        QTRY_VERIFY(!server.keyboardShown());
        QVERIFY(server.textBoxFocused());
        QCOMPARE(command.count(), 0);

        // If it crashes, Backspace must not stay "delete".
        keyboard.abort();
        QTRY_VERIFY(!server.textBoxFocused());
    }

    void dropsEndlessLines()
    {
        CommandServer server;
        QVERIFY(server.listen());
        QSignalSpy command(&server, &CommandServer::command);
        QLocalSocket socket;
        socket.connectToServer(CommandServer::socketPath());
        QVERIFY(socket.waitForConnected());
        socket.write(QByteArray(100'000, 'x'));
        socket.flush();
        QTRY_COMPARE(socket.state(), QLocalSocket::UnconnectedState);
        QCOMPARE(command.count(), 0);
    }
};

QTEST_GUILESS_MAIN(TestCommands)
#include "tst_commands.moc"
