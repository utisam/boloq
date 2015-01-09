cmake_minimum_required (VERSION 2.8)

add_unittest (unittest unittest.cpp)

install (DIRECTORY . DESTINATION "include"
         FILES_MATCHING PATTERN "*.h"
         PATTERN "examples" EXCLUDE
         PATTERN "doc" EXCLUDE)
