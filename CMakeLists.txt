cmake_minimum_required(VERSION 3.29)

project(PurposeUguuUpload)

find_package(ECM 6.29.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDECMakeSettings)

find_package(Qt6 6.9.0 REQUIRED COMPONENTS Core Network)

find_package(KF6CoreAddons 6.29.0 REQUIRED)
find_package(KF6Purpose 6.29.0 REQUIRED)
find_package(KF6Notifications 6.29.0 REQUIRED)


add_subdirectory(src)