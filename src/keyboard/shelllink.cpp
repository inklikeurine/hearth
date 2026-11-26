// SPDX-License-Identifier: GPL-3.0-or-later
#include "shelllink.h"

#include <chrono>

using namespace Qt::StringLiterals;
using namespace std::chrono_literals;

namespace {

// No command is anywhere near this long.
constexpr qint64 longestLine = 256;

} // namespace

ShellLink::ShellLink(const QString &path, QObject *parent)
    : QObject(parent)
    , m_path(path)
{
    m_retry.setSingleShot(true);
    m_retry.setInterval(500ms);
    connect(&m_retry, &QTimer::timeout, this, &ShellLink::connectToShell);
    connect(&m_socket, &QLocalSocket::connected, this, [this] {
        m_socket.write("keyboard\n");
        send("shown", m_shown);
        send("textbox", m_textBox);
    });
    connect(&m_socket, &QLocalSocket::disconnected, &m_retry, qOverload<>(&QTimer::start));
    connect(&m_socket, &QLocalSocket::errorOccurred, &m_retry, qOverload<>(&QTimer::start));
    connect(&m_socket, &QLocalSocket::readyRead, this, &ShellLink::read);
}

QString ShellLink::socketPath()
{
    const QString folder = qEnvironmentVariable("XDG_RUNTIME_DIR");
    const QString display = qEnvironmentVariable("WAYLAND_DISPLAY");
    if (folder.isEmpty() || display.isEmpty() || display.contains(u'/')) {
        return { };
    }
    return folder + u"/hearth-shell-"_s + display + u".sock"_s;
}

void ShellLink::start()
{
    if (!m_path.isEmpty()) {
        connectToShell();
    }
}

void ShellLink::connectToShell()
{
    if (m_socket.state() == QLocalSocket::UnconnectedState) {
        m_socket.connectToServer(m_path);
    }
}

void ShellLink::setState(bool shown, bool textBox)
{
    const bool shownChanged = shown != m_shown;
    const bool textBoxChanged = textBox != m_textBox;
    m_shown = shown;
    m_textBox = textBox;
    if (m_socket.state() != QLocalSocket::ConnectedState) {
        return;
    }
    if (shownChanged) {
        send("shown", m_shown);
    }
    if (textBoxChanged) {
        send("textbox", m_textBox);
    }
}

void ShellLink::send(const char *what, bool on)
{
    m_socket.write(QByteArray(what) + (on ? " 1\n" : " 0\n"));
    m_socket.flush();
}

void ShellLink::read()
{
    while (m_socket.canReadLine()) {
        const QByteArray line = m_socket.readLine(longestLine).trimmed();
        if (line == "hide") {
            Q_EMIT hideAsked();
        } else if (line == "backspace") {
            Q_EMIT backspaceAsked();
        }
    }
    if (m_socket.bytesAvailable() > longestLine) {
        m_socket.abort();
    }
}
