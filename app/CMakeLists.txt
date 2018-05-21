cmake_minimum_required(VERSION 3.6.0)

project(ICU4CSample LANGUAGES CXX)

find_library(LOG_LIB log)
find_library(ICU_LIB icundk)

add_library(app
  SHARED
    src/main/cpp/app.cpp
    src/main/cpp/jnihelpers.cpp
    src/main/cpp/calendar.cpp
)

# Our CMake is too old to know what C++17 is, so we can't use CXX_STANDARD.
target_compile_options(app
  PRIVATE
    -std=c++17
    -DU_DISABLE_RENAMING=1
)

target_link_libraries(app
  PRIVATE
    ${LOG_LIB}
    ${ICU_LIB}
)