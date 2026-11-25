// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "themetokens.h"

#include <QPointer>
#include <QQuickAttachedPropertyPropagator>
#include <QtQml/qqml.h>

// Gives a screen its theme: Theme.tokens.colors.text, and so on. Everything
// inside an item gets the same one, so Appearance can show a preview in a theme
// that isn't in use by setting Theme.tokens on the preview only.
class Theme : public QQuickAttachedPropertyPropagator {
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Theme is an attached property")
    QML_ATTACHED(Theme)
    Q_PROPERTY(ThemeTokens *tokens READ tokens WRITE setTokens NOTIFY tokensChanged)

public:
    explicit Theme(QObject *parent = nullptr);

    static Theme *qmlAttachedProperties(QObject *object);

    [[nodiscard]] ThemeTokens *tokens() const;
    void setTokens(ThemeTokens *tokens);

Q_SIGNALS:
    void tokensChanged();

protected:
    void attachedParentChange(QQuickAttachedPropertyPropagator *newParent,
        QQuickAttachedPropertyPropagator *oldParent) override;

private:
    void use(ThemeTokens *tokens);

    QPointer<ThemeTokens> m_tokens;
    bool m_setHere = false;
};
