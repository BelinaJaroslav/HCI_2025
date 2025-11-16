# GUI - User Interface

- [x] refactor ☺
- [x] check for OpenGL debug extension (if you succeeded to open GL context version 4.6, it should be present)
- [x] create debug callback (copy from lecture)
- [x] activate debug output
- [x] if the debug is too noisy, use filter to suppress notifications, etc.
- [x] create and register GLFW error callback to handle errors during library initialization
- [x] create and register additional callbacks
- [x] use callbacks and/or running time from `glfwGetTime()` to change triangle color by changing values
- [x] you can set color that is used to clear canvas – backgound color; use some other callbacks
- [x] VSYNC: load initial vsync on/off value from JSON
- [x] VSYNC: key callback to toggle on/off
- [x] Hide window during initialization
- [x] RMB to enable/disable "mouselook"
- [x] IMGUI: add to vcpkg
- [x] IMGUI: make it work
- [x] IMGUI: show FPS

Implement in ongoing semestral lab project.

## Task 1 - allow OpenGL debugging

- check for OpenGL debug extension (if you succeeded to open GL context version 4.6 in last Lab, it should be present)
- create debug callback (copy from lecture)
- activate debug output. If it is too noisy, use filter to suppress notifications, etc.

## Task 2 - use GLFW (a bit more) safely

- create and register GLFW error callback to handle errors during library initialization
- add proper error and quit handling (in C++ use exceptions)
    <https://www.glfw.org/docs/latest/quick.html#quick_capture_error>
    <https://www.glfw.org/docs/latest/quick.html#quick_key_input>

## Task 3 - Modify (extend) your app

- measure FPS (frame per second)
  - display FPS as a window title
- create and register additional callbacks

```C++
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, fbsize_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
```

- use callbacks and/or running time from glfwGetTime() to change triangle color by changing values in line

```C++
glUniform4f(uniform_color_location, r, g, b, a);
```

- You can set color that is used to clear canvas - backgound color. Use other callbacks and following command to do that.

```C++
glClearColor(r,g,b,a); 
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear with preset color
```

## Task 4 - implement VSync toggle

- create key callback to toggle vsync
  - modify window title to show vsync off, on
  - note: see the FPS difference  
- create and use JSON config file to set initial size of the GLFW window

## Task 5: Implement GUI, mouse cursor catch, hidden window during startup

1. When in fullscreen, you can not see FPS values in window title or debug output in console window. To overcome this, implement simple GUI using ImGUI library.

    To add ImGUI to your project:

    ```text
    vcpkg add port imgui[glfw-binding,opengl3-binding]
    ```

    - read _App.hpp_ to see added control variables.
    - read _App.cpp_ to see new includes, ImGUI initialization + termination, and expanded App::run loop

    You can find basic usage explanation <https://github.com/ocornut/imgui/wiki/Getting-Started>, online manual <https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html> and gallery of extensions implemented in ImGUI <https://github.com/ocornut/imgui/wiki/Useful-Extensions>.

2. When the app is in windowed mode and cursor is enabled, it can leave the app window, and mouse events will not be received. You can disable cursor - but than you can not click on close button in application title bar.

    - read _App::glfw_mouse_button_callback_ in _callbacks.cpp_ to see, how you can capture and release mouse button
    - you can modify the application logic: cursor can be released by e.g. TAB key, or first ESC (second ESC will terminate), etc.

3. During start-up, loading assets (models, textures, compiling shaders etc.) can take a long time. In the meantime, application window does not respond and is empty - and this could disturb the user (may think, that the app hang...).

    - read _App::init_ and _App::init_glfw_ in _App.cpp_ to see, how to hide the window during initialization

The other idea is to display some kind of loadscreen. That would also require some init & draw, so you must choose it carefully.
