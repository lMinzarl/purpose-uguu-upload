kcoreaddons_add_plugin(
    uguuplugin
    SOURCES uguuplugin.cpp
    INSTALL_NAMESPACE "kf6/purpose"
)

target_link_libraries(
    uguuplugin
    Qt6::Core
    Qt6::Network
    KF6::Purpose
    KF6::Notifications
    KF6::KIOCore
)