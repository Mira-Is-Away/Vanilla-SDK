cmake_minimum_required(VERSION 3.22)
project(vanilla C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

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
    GIT_TAG        v0.1.1
)
FetchContent_MakeAvailable(mira_clarity)

find_package(Vulkan REQUIRED)
find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME})

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
        $<$<CONFIG:Debug>:/W4 /Zi /0d /utf-8 /permissive->
        $<$<CONFIG:Release>:/02 /DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>:/02 /Zi>
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Debug>:-Wall -Wextra -Wpedantic -Werror>
        $<$<CONFIG:Release>:-03 -DNDEBUG>
        $<$<CONFIG:RelWithDebInfo>: -02 -g>
        $<$<CONFIG:MinSizeRel>:-0s -DNDEBUG>
    )
endif()
