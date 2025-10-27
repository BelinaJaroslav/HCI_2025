# CMakeList.txt : CMake project for HCI, include source and define
# project specific logic here.
cmake_minimum_required(VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project("HCI")

set(CMAKE_BUILD_TYPE Release)

# [VCPKG] Find packages
find_package(fmt           CONFIG REQUIRED)
find_package(glm           CONFIG REQUIRED)
find_package(GLEW                 REQUIRED)
find_package(glfw3         CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

if(WIN32)
    find_package(OpenCV CONFIG REQUIRED PATHS "c:/HCI/opencv/build" "C:/Users/jaroslav/source/repos/HCI_2025/opencv/build")
elseif(UNIX)
    find_package(OpenCV CONFIG REQUIRED)
endif()

file(GLOB_RECURSE SOURCES 
    "${PROJECT_SOURCE_DIR}/App/*.cpp"    
)

# Add source to this project's executable.
add_executable(HCI "HCI.cpp" ${SOURCES})

# [VCPKG] target libraries
target_link_libraries(HCI PRIVATE
    fmt::fmt
    GLEW::GLEW
    glfw
    glm::glm
    nlohmann_json::nlohmann_json
    ${OpenCV_LIBS}
)

target_include_directories(HCI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
)

# Copy resources into the build folder so relative paths work
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/App/Resources 
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/App/)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET HCI PROPERTY CXX_STANDARD 20)
endif()

# Eventually add tests and install targets if needed.
