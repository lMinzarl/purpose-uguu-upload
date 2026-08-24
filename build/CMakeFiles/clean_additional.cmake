# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "src/CMakeFiles/uguuplugin_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/uguuplugin_autogen.dir/ParseCache.txt"
  "src/uguuplugin_autogen"
  )
endif()
