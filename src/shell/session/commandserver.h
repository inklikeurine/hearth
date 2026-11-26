// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QLocalServer>
#include <QObject>
#include <QPointer>
#include <QVariantMap>
#include <QtQml/qqmlregistration.h>

class QLocalSocket;

// Where labwc's key bindings (through hearth-send) and the on-screen keyboard
// reach the shell. Only this profile's own user can connect. One command a line:
//   home, back, backspace, menu, ...   passed on as command()
//   state                              answered with one line of JSON
//   keyboard                           this connection is the on-screen keyboard
// The keyboard then sends "shown 0|1" and "textbox 0|1", and is sent "hide" and
// "backspace".
class CommandServer : public QObject {
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool keyboardShown READ keyboardShown NOTIFY keyboardChanged)
    Q_PROPERTY(bool textBoxFocused READ textBoxFocused NOTIFY keyboardChanged)
    Q_PROPERTY(QVariantMap state MEMBER m_state NOTIFY stateChanged)

public:
    explicit CommandServer(QObject *parent = nullptr);
    ~CommandServer() override;
    Q_DISABLE_COPY_MOVE(CommandServer)

    // Empty outside a Wayland session. hearth-send has the same rule.
    static QString socketPath();
    Q_INVOKABLE bool listen(const QString &path = socketPath());

    [[nodiscard]] bool keyboardShown() const
    {
        return m_keyboardShown;
    }
    // A text box in the app in front has the keyboard focus.
    [[nodiscard]] bool textBoxFocused() const
    {
        return m_textBoxFocused;
    }

    Q_INVOKABLE void hideKeyboard();
    Q_INVOKABLE void sendBackspace();

Q_SIGNALS:
    void command(const QString &name);
    void keyboardChanged();
    void stateChanged();

private:
    void read(QLocalSocket *socket);
    void readFromKeyboard(const QByteArray &line);
    void setKeyboard(bool shown, bool textBox);
    void tellKeyboard(const QByteArray &line);

    QLocalServer m_server;
    QPointer<QLocalSocket> m_keyboard;
    bool m_keyboardShown = false;
    bool m_textBoxFocused = false;
    QVariantMap m_state;
};
