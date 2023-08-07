cmake_minimum_required(VERSION 3.0.0)
project(Heron VERSION 0.1.0 LANGUAGES C CXX)

# set(GCC_COVERAGE_LINK_FLAGS     "-lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -pthread -lopengl32 -lgdi32 -lstdc++fs")

include(CTest)
enable_testing()

include_directories(include)
include_directories(include/freeimage)
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

add_executable(Heron ${heron_srcs} src/glad/glad/glad.c)

find_package(OpenGL REQUIRED)
include_directories( ${OPENGL_INCLUDE_DIRS} )   

# export MESA_GL_VERSION_OVERRIDE=4.5
target_link_libraries(Heron PRIVATE ${OPENGL_LIBRARIES})

target_link_libraries(Heron PRIVATE "freeimage")

find_package(glfw3 3.3 REQUIRED)
target_link_libraries(Heron PRIVATE "glfw")
find_package(Threads REQUIRED)
target_link_libraries(Heron PRIVATE Threads::Threads)

target_link_libraries(Heron PRIVATE ${CMAKE_DL_LIBS} stdc++fs)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(COPY shaders DESTINATION .)
file(COPY images DESTINATION .)
file(COPY fonts DESTINATION .)

file(COPY preferences.ini DESTINATION .)
file(COPY imgui.ini DESTINATION .)
file(COPY heron.jpg DESTINATION .)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog/previews)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/HeronCatalog/configs)


# sudo apt-get install libao-dev libmpg123-dev
# sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev 
# sudo apt-get install libglfw3-dev (not libglfw3? )     
