// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDBusConnection>

// The message buses the shell may use. In a test session, or the dev box, the
// buses are the host desktop's: using them would switch off the developer's
// computer or take its notifications, so there are none.
namespace Buses {

bool isTestSession();
// The rule itself: SESSION_DEV is HEARTH_SESSION_DEV, empty when it isn't set.
bool isTestSession(const QString &sessionDev, bool inContainer);
QDBusConnection session();
QDBusConnection system();

} // namespace Buses
