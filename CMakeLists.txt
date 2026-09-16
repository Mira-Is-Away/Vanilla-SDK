cmake_minimum_required(VERSION 3.22)
project(vanilla-sdk LANGUAGES C)

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
FetchContent_MakeAvailable(glfw)

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

# glslc compiler for SPIR-V shader compilation
find_program(GLSLC_EXE glslc
    HINTS $ENV{VULKAN_SDK}/bin $ENV{VULKAN_SDK}/RunTimeInstaller
    DOC "Path to glslc shader compiler"
)

if(NOT GLSLC_EXE)
    message(FATAL_ERROR "glslc not found! Please, make sure glslc is installed and included inside your PATH.")
endif()

set(SHADER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/vertex.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/fragment.frag
)

set(SPIRV_BIN_FILES "")

foreach(SHADER_SRC ${SHADER_SOURCES})
    get_filename_component(FILE_NAME ${SHADER_SRC} NAME)
    set(SPIRV_OUT "${CMAKE_CURRENT_BINARY_DIR}/shaders/${FILE_NAME}.spv")

    add_custom_command(
        OUTPUT ${SPIRV_OUT}
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/shaders"
        COMMAND ${GLSLC_EXE} ${SHADER_SRC} -o ${SPIRV_OUT}
        DEPENDS ${SHADER_SRC}
        COMMENT "Compiling shader ${FILE_NAME} -> ${FILE_NAME}.spv"
        VERBATIM
    )

    list(APPEND SPIRV_BIN_FILES ${SPIRV_OUT})
endforeach()

add_custom_target(CompileShaders DEPENDS ${SPIRV_BIN_FILES})

add_library(vanilla SHARED
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

add_dependencies(vanilla Format CompileShaders)

set_target_properties(vanilla PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    OUTPUT_NAME "vanilla"
)

target_include_directories(vanilla PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(vanilla PUBLIC
    glfw
    mira_clarity
    mira_darray
    Vulkan::Vulkan
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

target_compile_definitions(vanilla PRIVATE 
    VNL_BUILD_SHARED
    SHADER_DIR="${CMAKE_CURRENT_BINARY_DIR}/shaders/"
)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE
    vanilla
)

if(MSVC)
    target_compile_definitions(vanilla PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_compile_options(vanilla PRIVATE
        $<$<CONFIG:Debug>:/W4 /Zi /Od /utf-8 /permissive->
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:/O2 /Zi>
    )
    target_compile_definitions(${PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS VNL_USE_SHARED)
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:/W4 /Zi /Od /utf-8 /permissive->
        $<$<CONFIG:Release>:/O2 /DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:/O2 /Zi>
    )
else()
    target_compile_options(vanilla PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>: -O2 -g>
        $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CONFIG:Release>:-O3 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>: -O2 -g>
        $<$<CONFIG:MinSizeRel>:-Os -DNDEBUG>
    )
endif()