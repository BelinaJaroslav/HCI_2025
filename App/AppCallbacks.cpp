#include "App.hpp"


void App::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}


void App::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    if ((action == GLFW_PRESS) || (action == GLFW_REPEAT)) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            // Escape was pressed...
            // Exit The App
            glfwSetWindowShouldClose(window, GLFW_TRUE);
            break;
        }
    }
}


void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Scrollwheel was used
}


void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // Window was resized
}


void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    // If mouselook is laggy then move logic into App::run ?
}


void App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
    
    /*
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // LMB was pressed...
    }
    /**/

    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        // RMB was pressed...
        this_inst->is_mouselook_on = !this_inst->is_mouselook_on;
        if (this_inst->is_mouselook_on) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

