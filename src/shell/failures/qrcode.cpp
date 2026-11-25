// SPDX-License-Identifier: GPL-3.0-or-later
#include "qrcode.h"

#include <QPainter>

#include <memory>
#include <qrencode.h>

namespace {

// The white edge the QR standard asks for, in modules.
constexpr int edge = 4;

} // namespace

void QrCode::setText(const QString &text)
{
    if (text == m_text) {
        return;
    }
    m_text = text;
    m_modules = 0;
    m_dark.clear();
    const std::unique_ptr<QRcode, decltype(&QRcode_free)> code(text.isEmpty()
            ? nullptr
            : QRcode_encodeString(text.toUtf8().constData(), 0, QR_ECLEVEL_M, QR_MODE_8, 1),
        &QRcode_free);
    if (code) {
        m_modules = code->width;
        m_dark.reserve(qsizetype(m_modules) * m_modules);
        for (int i = 0; i < m_modules * m_modules; ++i) {
            // The lowest bit says whether a module is dark.
            m_dark << (code->data[i] % 2 != 0);
        }
    }
    Q_EMIT textChanged();
    update();
}

void QrCode::paint(QPainter *painter)
{
    if (m_modules == 0) {
        return;
    }
    painter->fillRect(boundingRect(), Qt::white);
    // Whole pixels, so every module is the same size.
    const int size = int(qMin(width(), height())) / (m_modules + 2 * edge);
    const int left = (int(width()) - size * m_modules) / 2;
    const int top = (int(height()) - size * m_modules) / 2;
    for (int y = 0; y < m_modules; ++y) {
        for (int x = 0; x < m_modules; ++x) {
            if (m_dark.at(qsizetype(y) * m_modules + x)) {
                painter->fillRect(left + x * size, top + y * size, size, size, Qt::black);
            }
        }
    }
}
