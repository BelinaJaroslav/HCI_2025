# Lab 05

## Task 1: create functional project with all libraries

- use: OpenCV, GLFW, GLEW, GLM, nlohmann_json
- use examples in _00 setup project and libraries_ to include libraries in your project
- use library demonstration code to verify functionality of all libraries

## Task 2: check your system

- use glewinfo.exe and visualinfo.exe (or similar programs on other platforms) to get list of supported GL versions and extensions
  - verify, that you can use debug_output
  - verify, that you can open OpenGL 4.6 context

## Task 3: draw single triangle in 3D

- explore the source code of the GL demonstration in _01 GL triangle app_ directory
- open OpenGL context <https://www.glfw.org/docs/latest/quick.html#quick_create_window>
  - version 4.6, CORE profile
- modify (merge the code) your application to draw a single triangle in separate 3D window

## Task 4: modify (extend) your app

- get basic info from created OpenGL context
  - verify, that you opened at least requested version and correct profile
  - optional: get version info from glfw, glm (or other interesting info)
