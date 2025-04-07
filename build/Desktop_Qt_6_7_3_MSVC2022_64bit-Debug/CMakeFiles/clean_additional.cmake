# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\Industry_Detection_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\Industry_Detection_autogen.dir\\ParseCache.txt"
  "Industry_Detection_autogen"
  )
endif()
