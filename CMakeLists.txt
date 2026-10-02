cmake_minimum_required(VERSION 3.22)
project(vanilla LANGUAGES C)

set(VANILLA_SDK_VERSION "1.0.0-beta.14")

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo;MinSizeRel" CACHE STRING "" FORCE)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Temporarily disable shared libraries so GLFW builds statically.
# This is so that the .so/.dll/.dylib bundles glfw with it.
set(BUILD_SHARED_LIBS_SAVED ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

# Restore the shared library setting
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_SAVED} CACHE BOOL "" FORCE)

FetchContent_Declare(
    mira_clarity
    GIT_REPOSITORY https://github.com/Mira-Is-Away/Clarity-Debugger.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(mira_clarity)

FetchContent_Declare(
    mira_darray
    GIT_REPOSITORY https://github.com/Mira-Is-Away/darray.git
    GIT_TAG        main
)
FetchContent_MakeAvailable(mira_darray)

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

# Enables automatic formatting if clang-format is installed.
find_program(CLANG_FORMAT_EXE NAMES clang-format DOC "Path to clang's autoformatter")

if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE ALL_SOURCE_FILES 
        CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c" 
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
    )

    add_custom_target(Format
        COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
        COMMENT "Formatting all source files with clang-format..."
        VERBATIM
    )
else()
    message(WARNING "clang-format not found. Automatic code formatting at compile-time will not be available.")
    add_custom_target(Format)
endif()

# slangc compiler for SPIR-V 1.4 shader compilation
find_program(SLANGC_EXE slangc
    HINTS $ENV{VULKAN_SDK}/bin $ENV{VULKAN_SDK}/RunTimeInstaller
    DOC "Path to slangc shader compiler"
)

if(NOT SLANGC_EXE)
    message(FATAL_ERROR "slangc not found! Please, make sure slangc is installed and included inside your PATH.")
endif()

# Helper script that converts a .spv binary into a C uint32_t array header.
# slangc does not support the -mfmt=c flag, so we generate the header
# ourselves. The script is invoked as a custom command during the build.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/spv_to_header.cmake" [=[
file(READ "${SPV_INPUT}" spv_content HEX)
string(LENGTH "${spv_content}" hex_len)
math(EXPR last "${hex_len} - 8")
set(words "")
foreach(i RANGE 0 ${last} 8)
    string(SUBSTRING "${spv_content}" ${i} 2 b0)
    math(EXPR i1 "${i} + 2")
    string(SUBSTRING "${spv_content}" ${i1} 2 b1)
    math(EXPR i2 "${i} + 4")
    string(SUBSTRING "${spv_content}" ${i2} 2 b2)
    math(EXPR i3 "${i} + 6")
    string(SUBSTRING "${spv_content}" ${i3} 2 b3)
    list(APPEND words "0x${b3}${b2}${b1}${b0}")
endforeach()
string(JOIN ",\n" c_array ${words})
file(WRITE "${HEADER_OUTPUT}" "{\n${c_array}\n}\n")
]=])

set(SHADER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.slang
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.slang
)

# Map each .slang source to its entry point and stage
set(SHADER_vertex_ENTRY  "main")
set(SHADER_vertex_STAGE  "vertex")
set(SHADER_fragment_ENTRY "main")
set(SHADER_fragment_STAGE "fragment")

set(SPIRV_BIN_FILES "")

foreach(SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER_SRC} NAME_WE)  # e.g. "vertex"
    set(SPIRV_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")
    set(SPIRV_HEADER "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv.h")

    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${SLANGC_EXE}
            ${SHADER_SRC}
            -target spirv
            -profile spirv_1_4
            -entry ${SHADER_${FILE_NAME}_ENTRY}
            -stage ${SHADER_${FILE_NAME}_STAGE}
            -o ${SPIRV_OUT}
        DEPENDS ${SHADER_SRC}
        COMMENT "Compiling Slang shader ${FILE_NAME}.slang -> SPIR-V 1.4"
        VERBATIM
    )

    add_custom_command(
        OUTPUT ${SPIRV_HEADER}
        COMMAND ${CMAKE_COMMAND}
            -DSPV_INPUT=${SPIRV_OUT}
            -DHEADER_OUTPUT=${SPIRV_HEADER}
            -P "${CMAKE_CURRENT_BINARY_DIR}/spv_to_header.cmake"
        DEPENDS ${SPIRV_OUT}
        COMMENT "Generating C header for ${FILE_NAME}.spv"
        VERBATIM
    )

    list(APPEND SPIRV_BIN_FILES ${SPIRV_OUT} ${SPIRV_HEADER})
endforeach()

add_custom_target(CompileShaders DEPENDS ${SPIRV_BIN_FILES})

option(BUILD_SHARED_LIBS "Build Vanilla as a shared library rather than static" ON)

add_library(vanilla
    src/winman/winman.c
    src/core/vanilla.c
    src/renderer/vulkan/vnl_renderer_vk.c
    src/renderer/vulkan/vkcontext.c
    src/renderer/vulkan/vkqueue.c
    src/renderer/vulkan/vkswapchain.c
    src/renderer/vulkan/vkimageview.c
    src/renderer/vulkan/vkrenderpass.c
    src/renderer/vulkan/vkshadermodules.c
    src/renderer/vulkan/vkpipeline.c
    src/renderer/vulkan/vkframebuffer.c
    src/renderer/vulkan/vkcommandpool.c
    src/renderer/vulkan/vkcommandbuffers.c
    src/renderer/vulkan/vksync.c
    src/vnl_ds/vnl_ilist.c
    src/vnl_ds/vnl_list.c
)

add_dependencies(${PROJECT_NAME} Format CompileShaders)

set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "${PROJECT_NAME}"
)

target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/shaders
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    mira_clarity
    mira_darray
    Vulkan::Vulkan
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME}
        PRIVATE     VNL_BUILD_SHARED
        INTERFACE   VNL_USE_SHARED
    )
endif()

option(VANILLA_BUILD_TESTS "Build the testing suite" ON)
if(VANILLA_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/W4 /Zi /Od /utf-8 /permissive->
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:/O2 /Zi>
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>: -O2 -g>
        $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
    )
endif()