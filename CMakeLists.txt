cmake_minimum_required(VERSION 3.28)
project(PassMan LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── FetchContent: ftxui ───────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)

# ── Monocypher (vendored) ─────────────────────────────────────────────────────
add_library(monocypher STATIC
    ${CMAKE_SOURCE_DIR}/third-party/monocypher/monocypher.c
)
target_include_directories(monocypher PUBLIC
    ${CMAKE_SOURCE_DIR}/third-party/monocypher
)

# ── PassMan executable ────────────────────────────────────────────────────────
add_executable(PassMan
    ${CMAKE_SOURCE_DIR}/src/main.cpp
    ${CMAKE_SOURCE_DIR}/src/Data.cpp
    ${CMAKE_SOURCE_DIR}/src/MyStr.cpp
    ${CMAKE_SOURCE_DIR}/src/Organization.cpp
    ${CMAKE_SOURCE_DIR}/src/PassMan.cpp
    ${CMAKE_SOURCE_DIR}/src/Service.cpp
    ${CMAKE_SOURCE_DIR}/src/Token.cpp
    ${CMAKE_SOURCE_DIR}/src/User.cpp
)

target_include_directories(PassMan PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/includes
    ${CMAKE_SOURCE_DIR}/UI
)

target_link_libraries(PassMan PRIVATE
    monocypher
    ftxui::screen
    ftxui::dom
    ftxui::component
)

# ── Static runtime (Windows) ──────────────────────────────────────────────────
if(MSVC)
    set_property(TARGET PassMan PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
if(MINGW)
    target_link_options(PassMan PRIVATE -static -static-libgcc -static-libstdc++)
endif()
