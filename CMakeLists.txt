cmake_minimum_required(VERSION 3.12)

project("HCI")
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MSVC_PARALLEL ON CACHE BOOL "" FORCE)

# Find packages
find_package(fmt           CONFIG REQUIRED)
find_package(glm           CONFIG REQUIRED)
find_package(GLEW                 REQUIRED)
find_package(glfw3         CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(imgui         CONFIG REQUIRED)

if(WIN32)
    find_package(OpenCV CONFIG REQUIRED PATHS "c:/HCI/opencv/build" "C:/Users/jaroslav/source/repos/HCI_2025/opencv/build")
elseif(UNIX)
    find_package(OpenCV CONFIG REQUIRED)
endif()

file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/App/*.cpp"
    "${PROJECT_SOURCE_DIR}/App/*.hpp"
    "${PROJECT_SOURCE_DIR}/App/*.vert" # Add this
    "${PROJECT_SOURCE_DIR}/App/*.frag" # Add this
)

# Add source to this project's executable
add_executable(HCI "HCI.cpp" ${SOURCES})

# Target libraries
target_link_libraries(HCI PRIVATE
    fmt::fmt
    GLEW::GLEW
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    imgui::imgui
    ${OpenCV_LIBS}
)

target_include_directories(HCI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# Copy resources into the build folder so relative paths work
add_custom_command(TARGET HCI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/App/Resources"
        "$<TARGET_FILE_DIR:HCI>/App/Resources"
    COMMENT "Copying shaders and resources to build directory..."
)
if(MSVC)
    target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()
