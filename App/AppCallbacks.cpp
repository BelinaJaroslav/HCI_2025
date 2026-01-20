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
			// `Escape` to exit the app
			this_inst->do_terminate_worker_threads = true;
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			break;

		case GLFW_KEY_F:
			// `F` to on/off fullscreen
			this_inst->enable_or_disable_fullscreen(true);
			break;

		case GLFW_KEY_C:
			// `C` to on/off freeform camera
			this_inst->is_camera_freeform = !this_inst->is_camera_freeform;
			break;		

		case GLFW_KEY_V:
			// `V` to on/off VSYNC
			this_inst->enable_or_disable_vsync(true);
			break;

		case GLFW_KEY_B:
			// `B` to on/off AA
			this_inst->enable_or_disable_antialiasing(true);
			break;

		case GLFW_KEY_P:
			// `P` to test the audio
			this_inst->audio_manager.play3D(this_inst->key_snd_pop, 0, 1, 0);
			break;

		// End switch
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
		// Reset is_first_mouse when mouselook is off
		this_inst->is_first_mouse = true;
		return;
	}

	if (this_inst->is_first_mouse)
	{
		this_inst->last_mouse_x = xpos;
		this_inst->last_mouse_y = ypos;
		this_inst->is_first_mouse = false;
	}

	double xoffset = xpos - this_inst->last_mouse_x;
	double yoffset = this_inst->last_mouse_y - ypos; // reversed

	this_inst->last_mouse_x = xpos;
	this_inst->last_mouse_y = ypos;

	this_inst->camera.process_mouse_movement(static_cast<float>(xoffset), static_cast<float>(yoffset));
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
		this_inst->enable_or_disable_mouselook(true);
	}
}

