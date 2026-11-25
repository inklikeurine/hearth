// SPDX-License-Identifier: GPL-3.0-or-later
#include "theme.h"

Theme::Theme(QObject *parent)
    : QQuickAttachedPropertyPropagator(parent)
{
    initialize();
}

Theme *Theme::qmlAttachedProperties(QObject *object)
{
    return new Theme(object);
}

ThemeTokens *Theme::tokens() const
{
    return m_tokens ? m_tokens.data() : ThemeTokens::live();
}

void Theme::setTokens(ThemeTokens *tokens)
{
    m_setHere = true;
    use(tokens);
}

void Theme::use(ThemeTokens *tokens)
{
    if (m_tokens == tokens) {
        return;
    }
    m_tokens = tokens;
    const auto children = attachedChildren();
    for (QQuickAttachedPropertyPropagator *child : children) {
        auto *theme = qobject_cast<Theme *>(child);
        if (theme && !theme->m_setHere) {
            theme->use(tokens);
        }
    }
    Q_EMIT tokensChanged();
}

void Theme::attachedParentChange(
    QQuickAttachedPropertyPropagator *newParent, QQuickAttachedPropertyPropagator *)
{
    const auto *parentTheme = qobject_cast<Theme *>(newParent);
    if (parentTheme && !m_setHere) {
        use(parentTheme->m_tokens);
    }
}
