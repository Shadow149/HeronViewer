cmake_minimum_required(VERSION 3.0.0)
project(Heron VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

include_directories(include)
# include_directories(include/freeimage)
include_directories(include/glad)
include_directories(include/glad/glad)
include_directories(include/glad/KHR)
include_directories(include/GLFW)
include_directories(include/glm)
include_directories(include/heron)
include_directories(include/heron/modules)
include_directories(include/heron/common)
include_directories(include/heron/catalog)
include_directories(include/heron/opengl)
include_directories(include/imgui)
include_directories(include/ImGuiFileDialog)
include_directories(include/imnodes)
include_directories(include/implot)
include_directories(include/mini)
include_directories(include/stb)
include_directories(include/avir)

file(GLOB_RECURSE heron_srcs
    "src/glad/*.cpp"
    "src/glm/*.cpp"
    "src/heron/*.cpp"
    "src/imgui/*.cpp"
    "src/ImGuiFileDialog/*.cpp"
    "src/imnodes/*.cpp"
    "src/implot/*.cpp"
    "src/stb/*.cpp"
)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_executable(Heron ${heron_srcs} src/glad/glad/glad.c)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )   

# export MESA_GL_VERSION_OVERRIDE=4.5
target_link_libraries(Heron PRIVATE ${OPENGL_LIBRARIES})

# target_link_libraries(Heron PRIVATE "freeimage")

target_link_libraries(Heron PRIVATE -lraw)

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(Heron PRIVATE "glfw")
find_package(Threads REQUIRED)
target_link_libraries(Heron PRIVATE Threads::Threads)

target_link_libraries(Heron PRIVATE ${CMAKE_DL_LIBS} stdc++fs)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

add_custom_target(
        copy_shaders ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/shaders
                ${CMAKE_CURRENT_BINARY_DIR}/shaders)
add_custom_target(
        copy_images ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/images
                ${CMAKE_CURRENT_BINARY_DIR}/images)
add_custom_target(
        copy_fonts ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/fonts
                ${CMAKE_CURRENT_BINARY_DIR}/fonts)
add_custom_target(
        copy_icons ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/icon
                ${CMAKE_CURRENT_BINARY_DIR}/icon)

add_custom_target(
        copy_prefs ALL
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/preferences.ini
                ${CMAKE_CURRENT_BINARY_DIR}/preferences.ini)
add_custom_target(
        copy_imgui ALL
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/imgui.ini
                ${CMAKE_CURRENT_BINARY_DIR}/imgui.ini)

add_custom_target(
    make_catalog ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog)
add_custom_target(
    make_catalog_prevs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog/previews)
add_custom_target(
    make_catalog_configs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog/configs)


# sudo apt-get install libao-dev libmpg123-dev
# sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev 
# sudo apt-get install libglfw3-dev (not libglfw3? )     
