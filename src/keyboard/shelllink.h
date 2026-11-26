// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QLocalSocket>
#include <QObject>
#include <QTimer>

// Tells the shell whether the keyboard is shown and whether a text box has focus,
// and hears from it when to close or delete. Connects again when the shell comes
// back. What's typed never goes this way.
class ShellLink : public QObject {
    Q_OBJECT

public:
    explicit ShellLink(const QString &path, QObject *parent = nullptr);

    // The shell's socket. CommandServer::socketPath() has the same rule.
    static QString socketPath();
    void start();
    void setState(bool shown, bool textBox);

Q_SIGNALS:
    void hideAsked();
    void backspaceAsked();

private:
    void connectToShell();
    void read();
    void send(const char *what, bool on);

    QString m_path;
    QLocalSocket m_socket;
    QTimer m_retry;
    bool m_shown = false;
    bool m_textBox = false;
};
