cmake_minimum_required(VERSION 3.22)
project(Emul-8)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

if (_WIN32)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(googletest)

SET(SOURCES
    "src/emul8.cpp"
    "src/emul8.hpp"
)

add_executable(Emul-8 ${SOURCES})

set(SFML_DIR "./external/SFML-2.5.1/lib/cmake/SFML/")
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5.1 COMPONENTS system window graphics audio REQUIRED)
target_link_libraries(Emul-8 sfml-system sfml-window sfml-graphics sfml-audio)
