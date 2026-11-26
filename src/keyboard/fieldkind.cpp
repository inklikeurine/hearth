// SPDX-License-Identifier: GPL-3.0-or-later
#include "fieldkind.h"

namespace {

// From text-input v3's definition (docs/research/K1-wayland-keyboard-protocols.md).
enum Purpose : uint32_t {
    PurposeDigits = 2,
    PurposeNumber = 3,
    PurposePhone = 4,
    PurposeUrl = 5,
    PurposeEmail = 6,
    PurposePassword = 8,
    PurposePin = 9,
    PurposeTerminal = 13,
};
constexpr uint32_t hintHiddenText = 0x40;
constexpr uint32_t hintSensitiveData = 0x80;

} // namespace

FieldKind ContentType::kind() const
{
    switch (purpose) {
    case PurposeDigits:
    case PurposePin:
        return FieldKind::Digits;
    case PurposeNumber:
        return FieldKind::Number;
    case PurposePhone:
        return FieldKind::Phone;
    case PurposeUrl:
        return FieldKind::Url;
    case PurposeEmail:
        return FieldKind::Email;
    case PurposePassword:
        return FieldKind::Password;
    default:
        return hints & hintHiddenText ? FieldKind::Password : FieldKind::Text;
    }
}

bool ContentType::isSensitive() const
{
    return purpose == PurposePassword || purpose == PurposePin
        || (hints & (hintHiddenText | hintSensitiveData));
}

bool ContentType::isTerminal() const
{
    return purpose == PurposeTerminal;
}
