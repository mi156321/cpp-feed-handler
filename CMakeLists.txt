cmake_minimum_required(VERSION 3.16)
project(feed_handler CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# WITH_KDB links against the real kdb+ C API (k.h/k.c from KX, not
# redistributed here - copy them into third_party/kdb/ to enable this).
# When off, the build still works and falls back to ConsolePublisher.
option(WITH_KDB "Build against the kdb+ C API for live tickerplant publishing" OFF)

add_library(normalizer
    src/source_normalizer.cpp
    src/tick_publisher.cpp
    src/csv_curve_publisher.cpp
)
target_include_directories(normalizer PUBLIC include)

if(WITH_KDB)
    if(NOT EXISTS "${CMAKE_SOURCE_DIR}/third_party/kdb/k.h")
        message(FATAL_ERROR "WITH_KDB=ON but third_party/kdb/k.h is missing. "
                             "Copy k.h/k.c from your kdb+ install into third_party/kdb/.")
    endif()
    target_compile_definitions(normalizer PUBLIC WITH_KDB)
    target_include_directories(normalizer PUBLIC third_party/kdb)
    target_sources(normalizer PRIVATE third_party/kdb/k.c)
    if(UNIX AND NOT APPLE)
        target_link_libraries(normalizer PUBLIC pthread dl)
    endif()
endif()

add_executable(feed_handler src/main.cpp)
target_link_libraries(feed_handler PRIVATE normalizer)

enable_testing()
add_executable(test_normalize tests/test_normalize.cpp)
target_link_libraries(test_normalize PRIVATE normalizer)
add_test(NAME test_normalize COMMAND test_normalize)
