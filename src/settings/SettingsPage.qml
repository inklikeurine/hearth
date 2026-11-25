// SPDX-License-Identifier: GPL-3.0-or-later
import QtQuick
import Hearth.Apps
import Hearth.Controls

// What every settings page is: a column of settings.
FocusColumn {
    id: page

    required property ShellSettings settings

    // Back, inside the page. False: the page has nothing open, so leave it.
    function goBack(): bool {
        return false;
    }
}
