cmake_minimum_required(VERSION 3.16)
project(DungeonGame VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 5.5
)
FetchContent_MakeAvailable(raylib)

add_executable(DungeonGame WIN32
    src/main.cpp
    src/SaveSystem.cpp
    src/entities/Player.cpp
    src/entities/Enemy.cpp
    src/entities/Slime.cpp
    src/entities/Skeleton.cpp
    src/entities/EnemyFactory.cpp
    src/combat/SkillSystem.cpp
    src/combat/SkillDefinitions.cpp
    src/combat/SkillAssignments.cpp
    src/ui/Button.cpp
    src/scenes/MainMenuScene.cpp
    src/scenes/CharacterCreationScene.cpp
    src/scenes/BattleScene.cpp
    src/scenes/GalleryScene.cpp
)

target_include_directories(DungeonGame PRIVATE include)

target_link_libraries(DungeonGame PRIVATE raylib)

if (WIN32)
    target_link_libraries(DungeonGame PRIVATE winmm)
endif()
