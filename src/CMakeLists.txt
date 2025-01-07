cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project("GEOAProject")

# Add source files
add_executable(GEOAProject "FlyFish.cpp" "Game.cpp" "structs.cpp" "utils.cpp" "main.cpp"  "GameItem.h" "GameItem.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET GEOAProject PROPERTY CXX_STANDARD 20)
endif()

# Simple Directmedia Layer
set(SDL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2-2.30.9")
add_library(SDL STATIC IMPORTED)
set_target_properties(SDL PROPERTIES
    IMPORTED_LOCATION "${SDL_DIR}/lib/SDL2.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_DIR}/include"
)

# Simple Directmedia Layer TTF
set(SDL_TTF_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/SDL2_ttf-2.22.0")
add_library(SDL_TTF STATIC IMPORTED)
set_target_properties(SDL_TTF PROPERTIES
    IMPORTED_LOCATION "${SDL_TTF_DIR}/lib/SDL2_ttf.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${SDL_TTF_DIR}/include"
)

# Link SDL2main.lib explicitly if available
if(EXISTS "${SDL_DIR}/lib/SDL2main.lib")
    set_target_properties(SDL PROPERTIES
        IMPORTED_LINK_INTERFACE_LIBRARIES "${SDL_DIR}/lib/SDL2main.lib")
else()
    message(FATAL_ERROR "SDL2main.lib not found in ${SDL_DIR}/lib.")
endif()

target_link_libraries(GEOAProject PRIVATE SDL SDL_TTF opengl32)

file(GLOB_RECURSE DLL_FILES
    "${SDL_DIR}/lib/*.dll"
    "${SDL_DIR}/lib/*.manifest"
    "${SDL_TTF_DIR}/lib/*.dll"
    "${SDL_TTF_DIR}/lib/*.manifest"
)

foreach(DLL ${DLL_FILES})
    add_custom_command(TARGET GEOAProject POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${DLL}
        $<TARGET_FILE_DIR:GEOAProject>)
endforeach(DLL)
