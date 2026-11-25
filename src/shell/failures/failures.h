// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

// What a failure screen says: what went wrong in one sentence, what to do next,
// and where the code's help page is.
struct FailureText {
    Q_GADGET
    QML_VALUE_TYPE(failureText)
    Q_PROPERTY(QString code MEMBER code)
    Q_PROPERTY(QString what MEMBER what)
    Q_PROPERTY(QString next MEMBER next)
    Q_PROPERTY(QUrl help MEMBER help)

public:
    QString code;
    QString what;
    QString next;
    QUrl help;
};

// Every error code the shell has. A code is HEARTH-AREA-NUMBER; a number is never
// used again for something else. Adding one means adding it to docs/error-codes.md
// too, where its help page starts; a test compares the two.
class Failures : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    using QObject::QObject;

    // SUBJECT is what it happened to, such as an app's name.
    Q_INVOKABLE static FailureText describe(const QString &code, const QString &subject = { });
    static QStringList codes();
};
