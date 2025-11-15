// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App.hpp"


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

	FPSMeter fps_meter_main;
	// ------------------------------------------------------------------- //

	Color triangle_color{ 1.0f, 0.6f, 1.0f, 1.0f }; // Pink
    Color background_color{ 0.1f, 0.1f, 0.1f }; // Dark gray background

	// Activate shader program. There is only one program, so activation can be out of the loop. 
	// In more realistic scenarios, you will activate different shaders for different 3D objects.
	//glUseProgram(shader_prog_ID);

	// Get uniform location in GPU program. This will not change, so it can be moved out of the game loop.
	//GLint uniform_color_location = glGetUniformLocation(shader_prog_ID, "uniform_Color");
	//if (uniform_color_location == -1) {
	//	fmt::println(stderr, "Uniform location is not found in active shader program. Did you forget to activate it?");
	//}

	// ImGui state variables

	// Main game loop
	while (!glfwWindowShouldClose(window)) {

		fps_meter_main.update();

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		renderGUI(fps_meter_main, triangle_color, background_color);

		// Clear canvas
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// = After clearing canvas =

		// Mouselook: get cursor's offset from window center and the move it back to center
        // TODO: According to JJ, this is not a good approach
		if (is_mouselook_on) {
			//glfwGetCursorPos(window, &cursor_x, &cursor_y);
			//camera.ProcessMouseMovement(static_cast<GLfloat>(win_width / 2.0 - cursor_x), static_cast<GLfloat>(win_height / 2.0 - cursor_y));
			glfwSetCursorPos(window, win_width / 2.0, win_height / 2.0); // We have no camera yet, so this doesn't really do anything
		}

        // SHADER
        auto current_shader = shader_library.at("simple_shader");
        current_shader->activate();
        current_shader->setUniform("ucolor", glm::vec4(triangle_color.r, triangle_color.g, triangle_color.b, triangle_color.a));

		// set uniform parameter for shader
		// (try to change the color in some callback)          
		//glUniform4f(uniform_color_location, triangle_color.r, triangle_color.g, triangle_color.b, triangle_color.a);

		// bind 3d object data
		//glBindVertexArray(VAO_ID);		

		// draw all VAO data
		//glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(triangle_vertices.size()));

        for (auto& [key, value] : scene) {
            //value.update();
            value.draw();
        }


        // IMGUI DRAW
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// poll events, call callbacks, flip back<->front buffer
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
}

void App::renderGUI(FPSMeter& fps_meter, Color& triangle_color, Color& background_color) {

    ImGui::Begin("FPS Meter", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    {
        // Display current FPS value
        ImGui::Text("FPS: %.1f", fps_meter.get());

        // Show if the value was just updated
        if (fps_meter.is_updated()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0, 1, 0, 1), " (Updated)");
        }

        // FPS history graph
        static std::vector<float> fps_history;
        static const int history_size = 100;

        // FIX 1: Use fps_meter (the parameter) not fps_meter_main
        fps_history.push_back(static_cast<float>(fps_meter.get()));
        if (fps_history.size() > history_size) {
            fps_history.erase(fps_history.begin());
        }

        // Plot the FPS history
        ImGui::PlotLines("FPS History", fps_history.data(),
            static_cast<int>(fps_history.size()), 0,
            nullptr, 0.0f, 200.0f,
            ImVec2(200, 50));

        // Controls
        if (ImGui::Button("Reset FPS Counter")) {
            fps_meter.reset();
            fps_history.clear();
        }

        // Interval adjustment
        static float interval_seconds = 1.0f;
        // FIX 2: Use fps_meter (the parameter) not fps_meter_main
        if (ImGui::SliderFloat("Update Interval (s)", &interval_seconds, 0.1f, 5.0f)) {
            fps_meter.set_interval(std::chrono::duration<double>(interval_seconds));
        }
    }
    ImGui::End();

    // Controls Window
    ImGui::Begin("Render Controls");
    {
        float triangle_color_arr[3] = { triangle_color.r, triangle_color.g, triangle_color.b };
        if (ImGui::ColorEdit3("Triangle Color", triangle_color_arr)) {
            triangle_color.r = triangle_color_arr[0];
            triangle_color.g = triangle_color_arr[1];
            triangle_color.b = triangle_color_arr[2];
        }

        float background_color_arr[3] = { background_color.r, background_color.g, background_color.b };
        if (ImGui::ColorEdit3("Background Color", background_color_arr)) {
            background_color.r = background_color_arr[0];
            background_color.g = background_color_arr[1];
            background_color.b = background_color_arr[2];
            glClearColor(background_color_arr[0], background_color_arr[1], background_color_arr[2], 1.0f);
        }

        ImGui::Checkbox("Mouselook", &is_mouselook_on);

        if (ImGui::Button("Reset Colors")) {
            triangle_color = { 1.0f, 0.6f, 1.0f, 1.0f };
            background_color.r = 0.1f;
            background_color.g = 0.1f;
            background_color.b = 0.1f;
            glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
        }
    }
    ImGui::End();
}