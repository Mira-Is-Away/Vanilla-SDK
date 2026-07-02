cmake_minimum_required(VERSION 3.22)
project(vanilla C)

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

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)


# Enables automatic formatting if clang-format is installed.
find_program(CLANG_FORMAT_EXE NAMES clang-format)

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

add_executable(${PROJECT_NAME})

add_dependencies(${PROJECT_NAME} Format)

target_sources(${PROJECT_NAME} PRIVATE
    main.c
    src/glfw/window.c
    src/vanilla.c
    src/vulkan/vkcontext.c
    src/vnl_ds/vnl_ilist.c
    src/vnl_ds/vnl_list.c
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    mira_clarity
    Vulkan::Vulkan
    Threads::Threads
    ${CMAKE_DL_LIBS}
)

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