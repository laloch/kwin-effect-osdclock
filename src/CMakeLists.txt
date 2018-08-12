include(PkgConfigGetVar)

find_package (Qt5 REQUIRED COMPONENTS
    Core
    Gui
    DBus
)

find_package (KF5 REQUIRED COMPONENTS
    Service
    Completion
    WindowSystem
    GlobalAccel
    I18n
    ConfigWidgets
)

find_package (KWinEffects REQUIRED)

set(kwin_osdclock_SRCS plugin.cpp osdclock.cpp blur.cpp osdclockconfig.cpp)
add_library( kwin_osdclock SHARED ${kwin_osdclock_SRCS} )

PKGCONFIG_GETVAR(dbus-1 interfaces_dir INTERFACES_DIR)
set(kwin_osdclock_config_SRCS osdclock_config.cpp)
ki18n_wrap_ui( kwin_osdclock_config_SRCS osdclock_config.ui )
qt5_add_dbus_interface( kwin_osdclock_config_SRCS ${INTERFACES_DIR}/org.kde.kwin.Effects.xml kwineffects_interface )
kconfig_add_kcfg_files( kwin_osdclock_config_SRCS osdclockconfig.kcfgc )
add_library( kwin_osdclock_config MODULE ${kwin_osdclock_config_SRCS} )

add_dependencies(kwin_osdclock kwin_osdclock_config)

target_link_libraries(kwin_osdclock
    Qt5::DBus
    KF5::WindowSystem
    KF5::Completion
    KF5::ConfigWidgets
    KF5::I18n
    KF5::Service
    KWinEffects::KWinEffects
)

kcoreaddons_desktop_to_json(kwin_osdclock osdclock.desktop SERVICE_TYPES kwineffect.desktop)

target_link_libraries(kwin_osdclock_config
    Qt5::DBus
    KF5::Completion
    KF5::ConfigWidgets
    KF5::I18n
    KF5::Service
)

kcoreaddons_desktop_to_json(kwin_osdclock_config osdclock_config.desktop SERVICE_TYPES kcmodule.desktop)

install(
    TARGETS
        kwin_osdclock
    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/plugins
)

install(
    TARGETS
        kwin_osdclock_config
    DESTINATION
        ${PLUGIN_INSTALL_DIR}/kwin/effects/configs
)
