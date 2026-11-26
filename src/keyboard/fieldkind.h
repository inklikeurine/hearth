// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <cstdint>

// What kind of text a box takes, as far as its keys go.
enum class FieldKind { Text, Email, Url, Password, Number, Digits, Phone };

// What an app said about its text box: text-input v3's content purpose and hints.
struct ContentType {
    uint32_t hints = 0;
    uint32_t purpose = 0;

    [[nodiscard]] FieldKind kind() const;
    // A password or a PIN: typed letters are hidden, and nothing may be kept.
    [[nodiscard]] bool isSensitive() const;
    [[nodiscard]] bool isTerminal() const;

    bool operator==(const ContentType &) const = default;
};
