file(GLOB_RECURSE C64_SOURCES
    "${PROJECT_SOURCE_DIR}/src/C64/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/C64/*.hpp"
)

add_library(C64 STATIC
  ${C64_SOURCES}
  ${UTIL_SOURCES}
)

target_include_directories(C64 PUBLIC "${CMAKE_SOURCE_DIR}/src")

# link dependencies
target_link_libraries(
  C64
  PUBLIC spdlog::spdlog imgui
)

target_compile_definitions(C64 PRIVATE SPDLOG_COMPILED_LIB)
