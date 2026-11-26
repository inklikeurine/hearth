// SPDX-License-Identifier: GPL-3.0-or-later
#include "commandserver.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>

using namespace Qt::StringLiterals;

namespace {

// No command is anywhere near this long.
constexpr qint64 longestLine = 256;

} // namespace

CommandServer::CommandServer(QObject *parent)
    : QObject(parent)
{
    m_server.setSocketOptions(QLocalServer::UserAccessOption);
    connect(&m_server, &QLocalServer::newConnection, this, [this] {
        while (QLocalSocket *socket = m_server.nextPendingConnection()) {
            connect(socket, &QLocalSocket::readyRead, this, [this, socket] { read(socket); });
            connect(socket, &QLocalSocket::disconnected, this, [this, socket] {
                if (socket == m_keyboard) {
                    m_keyboard = nullptr;
                    setKeyboard(false, false);
                }
                socket->deleteLater();
            });
            // What it sent may have arrived before anyone was listening.
            read(socket);
        }
    });
}

// The sockets close as m_server goes, after this object is half gone: they
// mustn't call back into it.
CommandServer::~CommandServer()
{
    for (const auto sockets = m_server.findChildren<QLocalSocket *>();
        QLocalSocket *socket : sockets) {
        socket->disconnect(this);
    }
}

QString CommandServer::socketPath()
{
    const QString folder = qEnvironmentVariable("XDG_RUNTIME_DIR");
    const QString display = qEnvironmentVariable("WAYLAND_DISPLAY");
    if (folder.isEmpty() || display.isEmpty() || display.contains(u'/')) {
        return { };
    }
    return folder + u"/hearth-shell-"_s + display + u".sock"_s;
}

bool CommandServer::listen(const QString &path)
{
    if (path.isEmpty()) {
        return false;
    }
    // Left behind by a shell that crashed: labwc runs one shell only.
    QLocalServer::removeServer(path);
    if (!m_server.listen(path)) {
        qWarning("can't listen on %s: %s", qPrintable(path), qPrintable(m_server.errorString()));
        return false;
    }
    return true;
}

void CommandServer::read(QLocalSocket *socket)
{
    while (socket->canReadLine()) {
        const QByteArray line = socket->readLine(longestLine).trimmed();
        if (socket == m_keyboard) {
            readFromKeyboard(line);
        } else if (line == "keyboard") {
            m_keyboard = socket;
            setKeyboard(false, false);
        } else if (line == "state") {
            socket->write(
                QJsonDocument(QJsonObject::fromVariantMap(m_state)).toJson(QJsonDocument::Compact)
                + '\n');
            socket->flush();
        } else if (!line.isEmpty()) {
            Q_EMIT command(QString::fromUtf8(line));
        }
    }
    if (socket->bytesAvailable() > longestLine) {
        socket->abort();
    }
}

void CommandServer::readFromKeyboard(const QByteArray &line)
{
    const bool on = line.endsWith('1');
    if (line.startsWith("shown ")) {
        setKeyboard(on, m_textBoxFocused);
    } else if (line.startsWith("textbox ")) {
        setKeyboard(m_keyboardShown, on);
    }
}

void CommandServer::setKeyboard(bool shown, bool textBox)
{
    if (shown != m_keyboardShown || textBox != m_textBoxFocused) {
        m_keyboardShown = shown;
        m_textBoxFocused = textBox;
        Q_EMIT keyboardChanged();
    }
}

void CommandServer::tellKeyboard(const QByteArray &line)
{
    if (m_keyboard) {
        m_keyboard->write(line + '\n');
        m_keyboard->flush();
    }
}

void CommandServer::hideKeyboard()
{
    tellKeyboard("hide");
}

void CommandServer::sendBackspace()
{
    tellKeyboard("backspace");
}
