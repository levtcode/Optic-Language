cmake_minimum_required(VERSION 3.20)

project(
    Optic
    VERSION 0.1.0
    LANGUAGES C CXX
)

# ===============================
# Global config
# ===============================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================
# Sources
# ===============================

file(GLOB_RECURSE OPTIC_COMPILER_SOURCES
    compiler/*.cpp
)

file(GLOB_RECURSE OPTIC_COMPILER_HEADERS
    compiler/*.hpp
)

add_executable(optic
    ${OPTIC_COMPILER_SOURCES}
    ${OPTIC_COMPILER_HEADERS}
)

# ===============================
# Includes
# ===============================

target_include_directories(optic
    PRIVATE
        ${CMAKE_SOURCE_DIR}/compiler
        ${CMAKE_SOURCE_DIR}/c_compat/headers
)

# ===============================
# Definitions
# ===============================

target_compile_definitions(optic
    PRIVATE
        OPTIC_VERSION="${PROJECT_VERSION}"
        _CRT_SECURE_NO_WARNINGS
        $<$<CONFIG:Debug>:OPTIC_DEBUG>
)

# ===============================
# Compiler-specific flags
# ===============================

if(MSVC)
    target_compile_options(optic
        PRIVATE
            /W4
            /permissive-
            $<$<CONFIG:Debug>:/Od /Zi>
            $<$<CONFIG:Release>:/O2>
    )
else()
    target_compile_options(optic
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            $<$<CONFIG:Debug>:-O0 -g>
            $<$<CONFIG:Release>:-O3>
    )
endif()

# ===============================
# Installation
# ===============================

install(TARGETS optic
    RUNTIME DESTINATION bin
)
