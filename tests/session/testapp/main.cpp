// SPDX-License-Identifier: GPL-3.0-or-later
//
// hearth-testapp NAME LOG [fixed|fullscreen|stubborn|text|boxes]...: an app for the session test.
// Its app ID is NAME, and it writes what happens to it to the file LOG.
//   fixed     a window that can't be resized
//   fullscreen  starts full screen
//   stubborn  asks about unsaved work in a dialog instead of closing
//   text      has a text box, focused at the start
//   boxes     has two, and something else that's focused; Down goes to the first,
//             Enter to the next one
#include <QFile>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>

class Log : public QObject {
    Q_OBJECT

public:
    explicit Log(const QString &path)
        : m_file(path)
    {
        if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append)) {
            qFatal("can't write %s", qPrintable(path));
        }
    }

    Q_INVOKABLE void write(const QString &line)
    {
        m_file.write(line.toUtf8() + '\n');
        m_file.flush();
    }

private:
    QFile m_file;
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    const QStringList arguments = QGuiApplication::arguments();
    if (arguments.size() < 3) {
        return 2;
    }
    QGuiApplication::setDesktopFileName(arguments.at(1));
    Log log(arguments.at(2));
    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        { QStringLiteral("log"), QVariant::fromValue(&log) },
        { QStringLiteral("name"), arguments.at(1) },
        { QStringLiteral("options"), arguments.sliced(3) },
    });
    engine.loadFromModule("Hearth.TestApp", "TestApp");
    return engine.rootObjects().isEmpty() ? 1 : QGuiApplication::exec();
}

#include "main.moc"
