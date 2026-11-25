// SPDX-License-Identifier: GPL-3.0-or-later
#include "iconprovider.h"

#include <QDir>
#include <QIcon>
#include <QUrl>

#include <algorithm>

IconProvider::IconProvider()
    : QQuickImageProvider(QQuickImageProvider::Pixmap)
{
}

QPixmap IconProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize)
{
    const QString name = QUrl::fromPercentEncoding(id.toLatin1());
    const int side = std::clamp(requestedSize.width(), 16, 1024);
    const QIcon icon = QDir::isAbsolutePath(name) ? QIcon(name) : QIcon::fromTheme(name);
    const QPixmap pixmap = icon.pixmap(QSize(side, side));
    if (size) {
        *size = pixmap.size();
    }
    return pixmap;
}
