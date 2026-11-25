// SPDX-License-Identifier: GPL-3.0-or-later
#include "buses.h"

#include <QFile>

namespace Buses {

bool isTestSession()
{
    // Unless told it's a real one, it isn't.
    return qEnvironmentVariable("HEARTH_SESSION_DEV") != QLatin1String("0")
        || QFile::exists(QStringLiteral("/run/.containerenv"));
}

QDBusConnection session()
{
    return isTestSession() ? QDBusConnection(QStringLiteral("none"))
                           : QDBusConnection::sessionBus();
}

QDBusConnection system()
{
    return isTestSession() ? QDBusConnection(QStringLiteral("none"))
                           : QDBusConnection::systemBus();
}

} // namespace Buses
