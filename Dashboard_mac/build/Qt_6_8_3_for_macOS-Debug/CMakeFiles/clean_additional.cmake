# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/PiRacerDashboard_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/PiRacerDashboard_autogen.dir/ParseCache.txt"
  "PiRacerDashboard_autogen"
  )
endif()
