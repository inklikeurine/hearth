# SPDX-License-Identifier: GPL-3.0-or-later

include(GNUInstallDirs)
install(PROGRAMS hearth-session TYPE BIN)
install(FILES labwc/rc.xml DESTINATION ${CMAKE_INSTALL_DATADIR}/hearth/session/labwc)
install(FILES hearth-session.target DESTINATION lib/systemd/user)
install(FILES hearth-portals.conf DESTINATION ${CMAKE_INSTALL_DATADIR}/xdg-desktop-portal)
