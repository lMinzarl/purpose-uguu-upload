# CMake generated Testfile for 
# Source directory: /home/minz/purpose-uguu-upload
# Build directory: /home/minz/purpose-uguu-upload/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[appstreamtest]=] "/usr/bin/cmake" "-DAPPSTREAMCLI=/usr/bin/appstreamcli" "-DINSTALL_FILES=/home/minz/purpose-uguu-upload/build/install_manifest.txt" "-P" "/usr/share/ECM/kde-modules/appstreamtest.cmake")
set_tests_properties([=[appstreamtest]=] PROPERTIES  _BACKTRACE_TRIPLES "/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;177;add_test;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;195;appstreamtest;/usr/share/ECM/kde-modules/KDECMakeSettings.cmake;0;;/home/minz/purpose-uguu-upload/CMakeLists.txt;9;include;/home/minz/purpose-uguu-upload/CMakeLists.txt;0;")
subdirs("src")
