# Textures

## Task 1: Antialiasing

Use GLFW hint to initialize antialiasing with level 4. Compare FPS with/without antialiasing.

## Task 2: Screenshot

Implement screenshot functionality. Create two screenshots - with and without antialiasing. Open them in an image viewer, zoom in and compare image quality.

## Task 3: Basic control of 3D scene from the camera tracker

There are multiple possibilities, how to interact with the app using tracker:

- pause the application, when the user is not looking on the screen. Use camera to detect a face. If there is not face, you can for example:
  - multiply delta_t by 0.0f (render loop is still running, but virtual time has stopped, so nothing is moving)
  - or: use __glfwWaitEventsTimeout()__ with reasonable interval instead of __glfwPollEvents()__ to slow down render loop (energy save mode)
  - show/hide some object
  - ...
- move some 3d object according to tracker coordinates

Implement some interaction.

## Task 4: Explore the source code of the GL texture demonstration

## Task 5: Display textured object

- use example to extend functionality of Model + App class
- use shader with texture support
- load OBJ model with texture coordinates
- ...enjoy

## Task 6: Display dynamic texture to show camera image (on an Model or by ImGUI)

- option 1: GUI as texture on a rectangle
  - create Model with rectangle (from two triangles forming triangle strip, with proper texture coordinates)
  - each frame:
    - switch to perspective projection
    - draw all 3d objects
    - switch to orthographic projection
    - draw camera image as a texture applied to the rectangle, placed in corner of the screen  
- option 2: ImGUI
  - use texture with ImGUI (<https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples#example-for-opengl-users>)
  - load as in Task 1
  - display

  ``` C++
  // get texture size (this needs to be done just once, we use immutable format)

  // show texture   
  ImGui::Begin("OpenGL Texture");
  ImGui::Image((ImTextureID)(intptr_t)texture_library.at("my_tex").get_name(), ImVec2(texture_library.at("my_tex").get_width(), texture_library.at("my_tex").get_height()));
  ImGui::End();
  ```

- [ ] Add screenshot functionality
- [ ] Compare screenshots with AA enabled/disabled, use the `B` key
- [ ] Basic control of 3D scene from the camera tracker (maybe change the teapot color/rotation_speed or something according to the number of detected faces)
