// SPDX-License-Identifier: GPL-3.0-or-later
#include "failures.h"

#include <KLazyLocalizedString>
#include <KLocalizedString>
#include <QUrlQuery>

#include <algorithm>
#include <array>

using namespace Qt::StringLiterals;

namespace {

struct Failure {
    const char *code;
    KLazyLocalizedString what;
    KLazyLocalizedString next;
};

constexpr std::array failures {
    Failure { "HEARTH-APP-001", kli18n("%1 couldn't start."),
        kli18n("Try it again. If it still doesn't start, remove the app and install it again.") },
    Failure { "HEARTH-APP-002", kli18n("%1 closed before it opened."),
        kli18n("Try it again. If it keeps happening, restart the box.") },
    Failure { "HEARTH-APP-003", kli18n("%1 stopped working and had to close."),
        kli18n("Open it again. If it keeps happening, restart the box.") },
    // Last: what an unknown code shows, so a failure is never a blank screen.
    Failure { "HEARTH-SHELL-000", kli18n("Something went wrong."),
        kli18n("Try again. If it keeps happening, restart the box.") },
};

constexpr auto helpPages = "https://github.com/inklikeurine/hearth/wiki/"_L1;

} // namespace

FailureText Failures::describe(const QString &code, const QString &subject)
{
    auto found = std::find_if(failures.begin(), failures.end(),
        [&code](const Failure &failure) { return code == QLatin1String(failure.code); });
    if (found == failures.end()) {
        found = std::prev(failures.end());
    }
    const QString known = QString::fromLatin1(found->code);
    // Nothing else goes in: the address is on screen, and the box sends nothing.
    QUrl help(helpPages + known);
    help.setQuery(QUrlQuery { { u"version"_s, QStringLiteral(HEARTH_VERSION) } });
    const auto what = KLocalizedString(found->what);
    const bool named = QByteArrayView(found->what.untranslatedText()).contains("%1");
    return { known, named ? what.subs(subject).toString() : what.toString(),
        KLocalizedString(found->next).toString(), help };
}

QStringList Failures::codes()
{
    QStringList codes;
    for (const Failure &failure : failures) {
        codes << QString::fromLatin1(failure.code);
    }
    return codes;
}
