cmake_minimum_required(VERSION 3.31.0 FATAL_ERROR)

project(C64_WIN)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

set(CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)

# everything is linked statically
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(MINGW)
  add_link_options(-static -static-libgcc -static-libstdc++)
endif()

# add dependencies
include(cmake/CPM.cmake)
include(cmake/warnings.cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")

CPMAddPackage(
  NAME SDL
  GITHUB_REPOSITORY libsdl-org/SDL
  GIT_TAG release-3.2.8
  OPTIONS "SDL_SHARED OFF" "SDL_STATIC ON" "SDL_TEST_LIBRARY OFF"
)

CPMAddPackage(
  NAME imgui
  GITHUB_REPOSITORY ocornut/imgui
  GIT_TAG v1.91.9-docking
  DOWNLOAD_ONLY YES
)

CPMAddPackage(
  NAME spdlog
  GITHUB_REPOSITORY gabime/spdlog
  VERSION 1.15.1
  OPTIONS "SPDLOG_BUILD_SHARED OFF" "SPDLOG_BUILD_STATIC ON"
)

CPMAddPackage(
  NAME googletest
  GITHUB_REPOSITORY google/googletest
  VERSION 1.14.0
  OPTIONS "INSTALL_GTEST OFF" "gtest_force_shared_crt OFF"
  EXCLUDE_FROM_ALL YES
)

# Klaus Dormanns 6502 Functional Test (GPL-3.0), nur als Testeingabe
CPMAddPackage(
  NAME dormann_tests
  GITHUB_REPOSITORY Klaus2m5/6502_65C02_functional_tests
  GIT_TAG 7954e2dbb49c469ea286070bf46cdd71aeb29e4b
  DOWNLOAD_ONLY YES
)

add_library(imgui STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_demo.cpp # optionally comment this out
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
  ${imgui_SOURCE_DIR}
  ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL3::SDL3-static)
target_compile_definitions(imgui PUBLIC -DIMGUI_DISABLE_OBSOLETE_FUNCTIONS) # optional imgui setting
set_target_properties(imgui PROPERTIES FOLDER third-party) # optoinal IDE dir

file(GLOB_RECURSE UTIL_SOURCES
    "${PROJECT_SOURCE_DIR}/src/Utility/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/Utility/*.hpp"
)

add_subdirectory(src/C64)

# add executable
add_executable(${PROJECT_NAME}
  src/main.cpp

  src/GUI.cpp
)

include(CTest)
enable_testing()

include(GoogleTest)

add_executable(CPU6502_test tests/CPU6502_test.cpp)
target_link_libraries(CPU6502_test PRIVATE C64 GTest::gtest_main)
target_compile_definitions(CPU6502_test PRIVATE
  SPDLOG_COMPILED_LIB
  DORMANN_FUNCTIONAL_TEST_BIN="${dormann_tests_SOURCE_DIR}/bin_files/6502_functional_test.bin"
)
gtest_discover_tests(CPU6502_test DISCOVERY_TIMEOUT 30 PROPERTIES TIMEOUT 300)

# link dependencies
target_link_libraries(
  ${PROJECT_NAME}
  C64
  imgui
  opengl32
  SDL3::SDL3-static
  spdlog::spdlog
)

target_compile_definitions(${PROJECT_NAME} PRIVATE SPDLOG_COMPILED_LIB)

# roms are loaded relative to the working directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/roms"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/roms"
  COMMENT "Copying roms to output directory"
)
