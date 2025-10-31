// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App/App.hpp"


void App::run()
{
    // ------------------------------------------------------------------- //
    // This creates some redundant threads but it shouldn't matter because
    // this functionality won't be used in the future: the app will run in 
    // fullscreen and webcam footage will be part of the main window.
    const bool SHOW_WEBCAM_DETECTOR_WINDOW = false;
    const bool SHOW_WEBCAM_COMPRESSION_WINDOW = false;

    std::jthread t_detector;
    std::jthread t_compression;

    if (SHOW_WEBCAM_DETECTOR_WINDOW) t_detector = std::jthread(&App::lab_multithread, this);
    if (SHOW_WEBCAM_COMPRESSION_WINDOW) t_compression = std::jthread(&App::lab_compression_pool, this);
    // ------------------------------------------------------------------- //

    GLfloat r, g, b, a;
    r = 1.0f; g = 0.6f; b = 1.0f; a = 1.0f; // Pink

    // Activate shader program. There is only one program, so activation can be out of the loop. 
    // In more realistic scenarios, you will activate different shaders for different 3D objects.
    glUseProgram(shader_prog_ID);

    // Get uniform location in GPU program. This will not change, so it can be moved out of the game loop.
    GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
    if (uniform_color_location == -1) {
        fmt::println(stderr, "Uniform location is not found in active shader program. Did you forget to activate it?");
    }

    while (!glfwWindowShouldClose(window)) {
        // clear canvas
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // = After clearing canvas =
        
        // Mouselook: get cursor's offset from window center and the move it back to center
        if (is_mouselook_on) {
            //glfwGetCursorPos(window, &cursor_x, &cursor_y);
            //camera.ProcessMouseMovement(static_cast<GLfloat>(win_width / 2.0 - cursor_x), static_cast<GLfloat>(win_height / 2.0 - cursor_y));
            glfwSetCursorPos(window, win_width / 2.0, win_height / 2.0); // We have no camera yet, so this doesn't really do anything
        }

        // set uniform parameter for shader
        // (try to change the color in some callback)          
        glUniform4f(uniform_color_location, r, g, b, a);

        // bind 3d object data
        glBindVertexArray(VAO_ID);

        // draw all VAO data
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triangle_vertices.size()));

        // poll events, call callbacks, flip back<->front buffer
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

