cmake_minimum_required (VERSION 2.8)

add_unittest (unittest unittest.cpp)

install (DIRECTORY . DESTINATION "include/${CMAKE_PROJECT_NAME}"
         FILES_MATCHING PATTERN "*.h"
         PATTERN "examples" EXCLUDE)
