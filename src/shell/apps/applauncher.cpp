// SPDX-License-Identifier: GPL-3.0-or-later
#include "applauncher.h"

#include <csignal>
#include <unistd.h>
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
    for (QProcess *process : std::as_const(m_running)) {
        process->disconnect(this);
    }
    terminateAll();
    for (QProcess *process : std::as_const(m_running)) {
        if (!process->waitForFinished(closeWaitMs)) {
            killAll();
            process->waitForFinished(closeWaitMs);
        }
    }
}

void AppLauncher::start(const DesktopEntry &app)
{
    if (m_running.contains(app.id)) {
        return;
    }
    auto *process = new QProcess(this);
    // No shell: the arguments reach the program exactly as they are.
    process->setProgram(app.command.constFirst());
    process->setArguments(app.command.sliced(1));
    process->setProcessChannelMode(QProcess::ForwardedChannels);
    // Its own process group, so ending it also ends the programs it started.
    process->setUnixProcessParameters(QProcess::UnixProcessFlag::CreateNewSession);
    connect(process, &QProcess::started, this, [this, id = app.id] { Q_EMIT started(id); });
    connect(process, &QProcess::finished, this,
        [this, process, id = app.id](int code, QProcess::ExitStatus status) {
            m_running.remove(id);
            process->deleteLater();
            const bool crashed = status == QProcess::CrashExit;
            Q_EMIT finished(id, crashed || code != 0, crashed);
        });
    connect(process, &QProcess::errorOccurred, this,
        [this, process, id = app.id](QProcess::ProcessError error) {
            if (error == QProcess::FailedToStart) {
                m_running.remove(id);
                process->deleteLater();
                Q_EMIT failed(id);
            }
        });
    m_running.insert(app.id, process);
    process->start();
}

QString AppLauncher::appOfProcess(qint64 pid) const
{
    const pid_t session = pid > 0 ? ::getsid(pid_t(pid)) : -1;
    for (auto it = m_running.begin(); session > 0 && it != m_running.end(); ++it) {
        if (it.value()->processId() == session) {
            return it.key();
        }
    }
    return { };
}

void AppLauncher::signalAll(int signal)
{
    for (const QProcess *process : std::as_const(m_running)) {
        const auto pid = pid_t(process->processId());
        if (pid > 0) {
            ::kill(-pid, signal);
        }
    }
}

void AppLauncher::terminateAll()
{
    signalAll(SIGTERM);
}

void AppLauncher::killAll()
{
    signalAll(SIGKILL);
}
