// SPDX-License-Identifier: GPL-3.0-or-later
#include "applauncher.h"

#include <utility>

namespace {

constexpr int closeWaitMs = 2000;

} // namespace

AppLauncher::AppLauncher(QObject *parent)
    : QObject(parent)
{
}

// Apps end with the shell: a profile's session is over when its shell is.
AppLauncher::~AppLauncher()
{
    const QHash<QString, QProcess *> running = std::exchange(m_running, { });
    for (QProcess *process : running) {
        process->disconnect(this);
        process->terminate();
    }
    for (QProcess *process : running) {
        if (!process->waitForFinished(closeWaitMs)) {
            process->kill();
            process->waitForFinished(closeWaitMs);
        }
    }
}

void AppLauncher::launch(AppList *apps, int index)
{
    if (!apps || index < 0 || index >= apps->count()) {
        return;
    }
    const DesktopEntry app = apps->apps().at(index);
    if (m_running.contains(app.id)) {
        return;
    }
    auto *process = new QProcess(this);
    // No shell: the arguments reach the program exactly as they are.
    process->setProgram(app.command.constFirst());
    process->setArguments(app.command.sliced(1));
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    connect(process, &QProcess::started, this, [this, id = app.id] { Q_EMIT started(id); });
    connect(process, &QProcess::finished, this, [this, process, id = app.id] {
        m_running.remove(id);
        process->deleteLater();
        Q_EMIT finished(id);
    });
    connect(process, &QProcess::errorOccurred, this,
        [this, process, id = app.id, name = app.name](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
                m_running.remove(id);
                process->deleteLater();
                Q_EMIT failed(id, name);
            }
        });
    m_running.insert(app.id, process);
    process->start();
}
