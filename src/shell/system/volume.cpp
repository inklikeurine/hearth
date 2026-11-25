// SPDX-License-Identifier: GPL-3.0-or-later
#include "volume.h"

#include <QRegularExpression>

#include <algorithm>
#include <utility>

using namespace Qt::StringLiterals;

namespace {

constexpr auto output = "@DEFAULT_AUDIO_SINK@"_L1;
constexpr qsizetype mostWaiting = 2;

} // namespace

Volume::Volume(QObject *parent)
    : QObject(parent)
{
    connect(&m_process, &QProcess::finished, this, [this](int code, QProcess::ExitStatus status) {
        if (code != 0 || status != QProcess::NormalExit) {
            m_waiting.clear();
            Q_EMIT failed();
        } else if (m_reading) {
            read();
        } else if (!m_waiting.isEmpty()) {
            const QStringList next = m_waiting.takeFirst();
            run(next);
        } else {
            m_reading = true;
            m_process.start(u"wpctl"_s, { u"get-volume"_s, output });
        }
    });
    connect(&m_process, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        if (error == QProcess::FailedToStart) {
            m_waiting.clear();
            Q_EMIT failed();
        }
    });
}

void Volume::up()
{
    // Never louder than the output's own full volume.
    run({ u"set-volume"_s, u"--limit"_s, u"1.0"_s, output, u"5%+"_s });
}

void Volume::down()
{
    run({ u"set-volume"_s, output, u"5%-"_s });
}

void Volume::toggleMute()
{
    run({ u"set-mute"_s, output, u"toggle"_s });
}

void Volume::run(const QStringList &arguments)
{
    if (m_process.state() != QProcess::NotRunning) {
        // Held down, the button repeats faster than a slow box can follow.
        if (m_waiting.size() < mostWaiting) {
            m_waiting << arguments;
        }
        return;
    }
    m_reading = false;
    m_process.start(u"wpctl"_s, arguments);
}

void Volume::read()
{
    m_reading = false;
    // "Volume: 0.40 [MUTED]"
    static const QRegularExpression format(u"^Volume: ([0-9]+\\.[0-9]+)( \\[MUTED\\])?"_s);
    const auto match = format.match(QString::fromUtf8(m_process.readAllStandardOutput()));
    if (!match.hasMatch()) {
        Q_EMIT failed();
        return;
    }
    m_percent = std::clamp(qRound(match.captured(1).toDouble() * 100), 0, 100);
    m_muted = match.hasCaptured(2);
    Q_EMIT changed();
    Q_EMIT shown();
    if (!m_waiting.isEmpty()) {
        const QStringList next = m_waiting.takeFirst();
        run(next);
    }
}
