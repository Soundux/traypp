cmake_minimum_required(VERSION 3.1)
project(tray VERSION 0.1 DESCRIPTION "A cross-platform C++ system tray library")

file(GLOB src
    "src/*.cpp"
    "src/*/*.cpp"
    "src/*/*/*.cpp"
)

add_library(tray STATIC ${src})

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(APPINDICATOR REQUIRED appindicator3-0.1)

    target_link_libraries(tray INTERFACE ${GTK3_LIBRARIES} ${APPINDICATOR_LIBRARIES})
    target_compile_options(tray INTERFACE -Wall -Wextra -Werror -pedantic -Wno-unused-lambda-capture)
    target_include_directories(tray SYSTEM PUBLIC ${GTK3_INCLUDE_DIRS} ${APPINDICATOR_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR})
endif()

target_include_directories(tray SYSTEM PUBLIC "src/")
target_include_directories(tray SYSTEM PUBLIC "lib/json/single_include/nlohmann")

target_compile_features(tray PUBLIC cxx_std_17)
set_target_properties(tray PROPERTIES CMAKE_CXX_STANDARD 17)
set_target_properties(tray PROPERTIES CMAKE_CXX_EXTENSIONS Off)
set_target_properties(tray PROPERTIES CMAKE_CXX_STANDARD_REQUIRED On)

set_target_properties(tray PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(tray PROPERTIES PROJECT_NAME ${PROJECT_NAME})