cmake_minimum_required(VERSION 3.22)
project(Emul-8)

set(CMAKE_CXX_STANDARD 20)

set(TEST_PROJECT "" OFF)

SET(SOURCES
    "src/common.hpp"
    "src/cpu.hpp"
    "src/emul8.cpp"
    "src/emul8.hpp"
    "src/display.cpp"
    "src/display.hpp"
)


add_executable(Emul-8 ${SOURCES} "src/main.cpp")
target_compile_options(Emul-8 PRIVATE -Wall -Wextra)

target_link_libraries(Emul-8 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic)

set(SDL2_DIR ${CMAKE_CURRENT_LIST_DIR}/external/SDL2/lib/cmake/SDL2)
find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIRS})
target_link_libraries(Emul-8 ${SDL2_LIBRARIES})

if (TEST_PROJECT)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
    )
    if (_WIN32)
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable(googletest)

    add_executable(runUnitTests "src/test/test_emul8.cpp" ${SOURCES})
    target_link_libraries(runUnitTests gtest gtest_main)
    add_test(runUnitTests runUnitTests)

    target_link_libraries(runUnitTests ${SDL2_LIBRARIES})
endif()
