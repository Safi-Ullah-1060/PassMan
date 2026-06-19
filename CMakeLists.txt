cmake_minimum_required(VERSION 3.28)
project(PassMan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
  src/Data.cpp
  src/MyStr.cpp
  src/Organization.cpp
  src/PassMan.cpp
  src/Service.cpp
  src/Token.cpp
  src/User.cpp
)
include(FetchContent)
 
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
  GIT_TAG v6.1.9  # Replace with a version, tag, or commit hash
)
 
FetchContent_MakeAvailable(ftxui)
 
add_executable(main main.cpp ${SOURCES})
target_link_libraries(main
  PRIVATE ftxui::screen
  PRIVATE ftxui::dom
  PRIVATE ftxui::component
)
 
target_include_directories(main PRIVATE includes/)
