// SPDX-License-Identifier: GPL-3.0-or-later
#include "desktopentry.h"

#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QStandardPaths>
#include <QTextStream>

using namespace Qt::StringLiterals;

namespace {

// A .desktop file is a few kilobytes. Anything huge isn't one.
constexpr qint64 largestFile = 262144;

QHash<QString, QString> readKeys(const QString &path)
{
    QHash<QString, QString> keys;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text) || file.size() > largestFile) {
        return keys;
    }
    QTextStream in(&file);
    bool inEntry = false;
    QString line;
    while (in.readLineInto(&line)) {
        line = line.trimmed();
        if (line.startsWith(u'[')) {
            inEntry = line == "[Desktop Entry]"_L1;
            continue;
        }
        const qsizetype equals = line.indexOf(u'=');
        if (inEntry && equals > 0 && !line.startsWith(u'#')) {
            keys.insert(line.first(equals).trimmed(), line.sliced(equals + 1).trimmed());
        }
    }
    return keys;
}

// The escapes every text value may use.
QString unescape(const QString &value)
{
    QString out;
    for (qsizetype i = 0; i < value.size(); ++i) {
        if (value.at(i) != u'\\' || i + 1 == value.size()) {
            out += value.at(i);
            continue;
        }
        ++i;
        switch (value.at(i).unicode()) {
        case u's':
            out += u' ';
            break;
        case u'n':
            out += u'\n';
            break;
        case u't':
            out += u'\t';
            break;
        case u'r':
            out += u'\r';
            break;
        case u'\\':
            out += u'\\';
            break;
        default:
            out += u'\\';
            out += value.at(i);
        }
    }
    return out;
}

// Name[pt_BR], then Name[pt], then Name.
QString localized(const QHash<QString, QString> &keys, const QString &key, const QString &language)
{
    for (const QString &code : { language, language.section(u'_', 0, 0) }) {
        const auto found = keys.constFind(u"%1[%2]"_s.arg(key, code));
        if (!code.isEmpty() && found != keys.constEnd()) {
            return unescape(*found);
        }
    }
    return unescape(keys.value(key));
}

bool isTrue(const QString &value)
{
    return value == "true"_L1 || value == "1"_L1;
}

bool programExists(const QString &program)
{
    return QFileInfo(program).isAbsolute() ? QFileInfo(program).isExecutable()
                                           : !QStandardPaths::findExecutable(program).isEmpty();
}

bool meantForHearth(const QHash<QString, QString> &keys)
{
    if (keys.value(u"Type"_s) != "Application"_L1) {
        return false;
    }
    for (const auto &key : { u"NoDisplay"_s, u"Hidden"_s, u"Terminal"_s }) {
        if (isTrue(keys.value(key))) {
            return false;
        }
    }
    const QString only = keys.value(u"OnlyShowIn"_s);
    if (!only.isEmpty() && !only.split(u';').contains("Hearth"_L1)) {
        return false;
    }
    if (keys.value(u"NotShowIn"_s).split(u';').contains("Hearth"_L1)) {
        return false;
    }
    const QString tryExec = unescape(keys.value(u"TryExec"_s));
    return tryExec.isEmpty() || programExists(tryExec);
}

// Fills in one argument's field codes (%u and the like). Nothing if it's only a
// code for a file or an address: the home screen opens apps with neither.
std::optional<QString> expandCodes(
    const QString &argument, const DesktopEntry &entry, const QString &path)
{
    static const QString files = u"fFuUdDnNvm"_s;
    if (argument.size() == 2 && argument.startsWith(u'%') && files.contains(argument.at(1))) {
        return std::nullopt;
    }
    QString out;
    for (qsizetype i = 0; i < argument.size(); ++i) {
        if (argument.at(i) != u'%' || i + 1 == argument.size()) {
            out += argument.at(i);
            continue;
        }
        ++i;
        const QChar code = argument.at(i);
        if (code == u'%') {
            out += u'%';
        } else if (code == u'c') {
            out += entry.name;
        } else if (code == u'k') {
            out += path;
        }
    }
    return out;
}

} // namespace

std::optional<QStringList> DesktopEntry::splitExec(const QString &exec)
{
    QStringList arguments;
    QString current;
    bool started = false;
    bool quoted = false;
    for (qsizetype i = 0; i < exec.size(); ++i) {
        const QChar c = exec.at(i);
        if (quoted) {
            if (c == u'"') {
                quoted = false;
            } else if (c == u'\\') {
                if (++i == exec.size()) {
                    return std::nullopt;
                }
                current += exec.at(i);
            } else {
                current += c;
            }
        } else if (c == u'"') {
            quoted = true;
            started = true;
        } else if (c == u' ') {
            if (started) {
                arguments << current;
            }
            current.clear();
            started = false;
        } else {
            current += c;
            started = true;
        }
    }
    if (quoted) {
        return std::nullopt;
    }
    if (started) {
        arguments << current;
    }
    return arguments;
}

std::optional<DesktopEntry> DesktopEntry::read(
    const QString &path, const QString &id, const QString &language)
{
    const QHash<QString, QString> keys = readKeys(path);
    if (!meantForHearth(keys)) {
        return std::nullopt;
    }
    DesktopEntry entry;
    entry.id = id;
    entry.name = localized(keys, u"Name"_s, language);
    entry.icon = localized(keys, u"Icon"_s, language);
    entry.windowClass = keys.value(u"StartupWMClass"_s);

    const auto arguments = splitExec(unescape(keys.value(u"Exec"_s)));
    if (!arguments || arguments->isEmpty() || entry.name.isEmpty()) {
        return std::nullopt;
    }
    for (const QString &argument : *arguments) {
        if (argument == "%i"_L1) {
            if (!entry.icon.isEmpty()) {
                entry.command << u"--icon"_s << entry.icon;
            }
        } else if (const auto expanded = expandCodes(argument, entry, path)) {
            entry.command << *expanded;
        }
    }
    if (entry.command.isEmpty() || entry.command.constFirst().isEmpty()) {
        return std::nullopt;
    }
    return entry;
}
