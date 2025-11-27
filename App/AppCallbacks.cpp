// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
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

		case GLFW_KEY_V:
			this_inst->is_vsync_on = !this_inst->is_vsync_on;
			glfwSwapInterval(this_inst->is_vsync_on);
			fmt::println("VSync: {}", this_inst->is_vsync_on);
			break;
		}
	}
}


void App::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// Scrollwheel was used
	auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	this_inst->FOV -= 10.0f * static_cast<float>(yoffset);      // Scrollwheel down == FOV++
	this_inst->FOV = std::clamp(this_inst->FOV, 70.0f, 140.0f); // Limit FOV to "reasonable" values
	this_inst->update_projection_matrix();
}


void App::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// Window was resized
	auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	this_inst->win_width = width;
	this_inst->win_height = height;
	// set viewport
	glViewport(0, 0, width, height);
	// now your canvas has [0,0] in bottom left corner, and its size is [width x height] 
	this_inst->update_projection_matrix();
}


void App::cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	auto this_inst = static_cast<App*>(glfwGetWindowUserPointer(window));
	if (!this_inst->is_mouselook_on)
	{
		// Reset firstMouse when mouselook is off
		this_inst->firstMouse = true;
		return;
	}

	if (this_inst->firstMouse)
	{
		this_inst->lastMouseX = xpos;
		this_inst->lastMouseY = ypos;
		this_inst->firstMouse = false;
	}

	double xoffset = xpos - this_inst->lastMouseX;
	double yoffset = this_inst->lastMouseY - ypos; // reversed

	this_inst->lastMouseX = xpos;
	this_inst->lastMouseY = ypos;

	this_inst->camera.ProcessMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
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

