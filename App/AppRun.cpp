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
#ifndef SKIP_LABS_COMPILATION
    const bool SHOW_WEBCAM_DETECTOR_WINDOW = false;
	const bool SHOW_WEBCAM_COMPRESSION_WINDOW = false;

	std::jthread t_detector;
	std::jthread t_compression;

	if (SHOW_WEBCAM_DETECTOR_WINDOW) t_detector = std::jthread(&App::lab_multithread, this);
	if (SHOW_WEBCAM_COMPRESSION_WINDOW) t_compression = std::jthread(&App::lab_compression_pool, this);
#endif // !SKIP_LABS_COMPILATION

	FPSMeter fps_meter_main;
	// ------------------------------------------------------------------- //

    // Measuring delta time
    double current_timestamp = glfwGetTime();
    double last_frame_time = current_timestamp;
    float delta_time = 0.0f;

    // State
	Color triangle_color{ 1.0f, 0.6f, 1.0f, 1.0f }; // Pink
    Color background_color{ 0.1f, 0.1f, 0.1f }; // Dark gray background

    // Init view
    update_projection_matrix();
    glViewport(0, 0, win_width, win_height);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
        // Measure delta time
        current_timestamp = glfwGetTime();
        delta_time = static_cast<float>(current_timestamp - last_frame_time);
        last_frame_time = current_timestamp;

        // Measure FPS
		fps_meter_main.update();

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		render_GUI(fps_meter_main, triangle_color, background_color);

		// Clear canvas
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// = After clearing canvas =

		// Update camera position
        process_camera(delta_time);

        // SHADER
        auto current_shader = shader_library.at(key_shader_simple);
        current_shader->activate();
        // Projection matrix is a member of App, it changes on window resize and on FOV change
        current_shader->set_uniform("u_projection_mx", mx_projection);
        
        // Model matrix is handled by each model individually

        // View matrix is handled by the camera
        glm::mat4 mx_view = camera.get_view_matrix();

        current_shader->set_uniform("u_view_mx", mx_view);

        // TODO
        //current_shader->set_uniform("u_color", glm::vec4(triangle_color.r, triangle_color.g, triangle_color.b, triangle_color.a));        

        // DRAW MODELS FROM SCENE
        for (auto& [key, value] : scene) {
            
            if (key != key_obj_heightmap) {
                //value.update();
                value.relative_rotate(glm::vec3(0.0f, delta_time * 100.0f, 0.0f));
            }
            
            //fmt::print("Drawing {}: ", key);
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

void App::process_camera(float delta_t)
{
    glm::vec3 movement = camera.process_input(window, delta_t);
    camera.position += movement;
}


void App::render_GUI(FPSMeter& fps_meter, Color& triangle_color, Color& background_color)
{
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
        // Display current FOV value
        ImGui::Text("FOV: %.1f", FOV);

        ImGui::Separator();

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

        if (ImGui::Button("Reset Colors")) {
            triangle_color = { 1.0f, 0.6f, 1.0f, 1.0f };
            background_color.r = 0.1f;
            background_color.g = 0.1f;
            background_color.b = 0.1f;
            glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);
        }        
        
        ImGui::Separator();

        if (ImGui::Button("VSYNC on/off")) {
            enable_or_disable_vsync();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Checkbox("##readonly_checkbox_vsync", &is_vsync_on);
        ImGui::EndDisabled();

        if (ImGui::Button("Mouselook on/off")) {
            enable_or_disable_mouselook();
        }
        ImGui::SameLine();
        ImGui::BeginDisabled();
        ImGui::Checkbox("##readonly_checkbox_mouselook", &is_mouselook_on);
        ImGui::EndDisabled();
    }
    ImGui::End();
}

void App::update_projection_matrix()
{
    if (win_height < 1) win_height = 1; // avoid division by 0

    float ratio = static_cast<float>(win_width) / win_height;

    mx_projection = glm::perspective(
        glm::radians(FOV),
        ratio,               // Aspect Ratio. Depends on the size of your window.
        0.1f,                // Near clipping plane. Keep as big as possible, or you'll get precision issues.
        20000.0f             // Far clipping plane. Keep as little as possible.
    );
}
