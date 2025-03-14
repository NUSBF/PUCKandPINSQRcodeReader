# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/USBpuckcamera_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/USBpuckcamera_autogen.dir/ParseCache.txt"
  "USBpuckcamera_autogen"
  )
endif()
