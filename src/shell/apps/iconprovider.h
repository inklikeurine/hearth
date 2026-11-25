// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QQuickImageProvider>

// An app's icon by its name in the icon theme, or by its file:
// "image://icon/firefox".
class IconProvider : public QQuickImageProvider {
public:
    IconProvider();

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override;
};
