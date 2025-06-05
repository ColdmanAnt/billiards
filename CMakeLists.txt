cmake_minimum_required(VERSION 3.31)
project(billiards LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# → SFML 3.0.0
FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
)
FetchContent_MakeAvailable(sfml)

# → Box2D 3.1.0
set(BOX2D_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BOX2D_BUILD_SHARED     OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG        v2.4.1          # или v2.4.3 — реальный релиз
)
FetchContent_MakeAvailable(box2d)

if (TARGET box2d AND NOT TARGET box2d::box2d)
    add_library(box2d::box2d ALIAS box2d)
endif()

# ─── Добавляем «глобально» include-пути для Box2D и SFML ─────────
# Это гарантирует, что CLion и компилятор точно увидят нужные каталоги:
include_directories(
        include/thirdparty/glm
        include/thirdparty/glad
        ${sfml_SOURCE_DIR}/include
        ${box2d_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/include   # ваши локальные заголовки
)

# ─── Добавляем исполняемый файл ───────────────────────────────────
add_executable(billiards
        src/main.cpp
        src/physics/World.cpp
        src/physics/Ball.cpp
        src/physics/Table.cpp
        src/render/Renderer.cpp
        src/core/InputController.cpp
        src/render/GLRenderer.cpp
        src/thirdparty/glad/glad.c
)

set(SHADER_SOURCE_DIR   "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_BINARY_DIR   "$<TARGET_FILE_DIR:billiards>/shaders")

add_custom_command(TARGET billiards POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SHADER_SOURCE_DIR}"
        "${SHADER_BINARY_DIR}"
)

# ─── Линкуем SFML и Box2D ─────────────────────────────────────────
target_link_libraries(billiards PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        box2d::box2d
)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})