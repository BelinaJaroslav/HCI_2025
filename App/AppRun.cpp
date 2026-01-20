// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

// Our App
#include "App.hpp"


void App::run()
{
    // Constant quality encoder service
    std::jthread t_compression;
    if (is_encoder_on) t_compression = std::jthread(&App::lab_compression_pool, this);

    // Webcam service
    std::jthread thread_webcam_service;
    thread_webcam_service = std::jthread(&App::webcam_thread, this);

    // Measuring delta time
    double current_timestamp = glfwGetTime();
    double last_frame_time = current_timestamp;
    float delta_time = 0.0f;

    // State
	Color teapot_color{ 1.0f, 0.6f, 1.0f, 1.0f }; // Pink
    Color background_color{ 0.549f, 0.823f, 0.858f }; // Sky color
    glClearColor(background_color.r, background_color.g, background_color.b, 1.0f);

    // Init view
    update_projection_matrix();
    glViewport(0, 0, win_width, win_height);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
        // Webcam service
        if (!synced_deque.empty()) {
            auto tup = synced_deque.pop_front();
            auto& frame = std::get<0>(tup);
            //fmt::println("GL FRAME PTR  = {}", (void*)frame.data);
            n_faces_found = std::get<1>(tup);
            texture_library.at(key_tex_webcam)->replace_image(frame);
        }

        // Measure delta time
        current_timestamp = glfwGetTime();
        delta_time = static_cast<float>(current_timestamp - last_frame_time);
        last_frame_time = current_timestamp;        

		// Start ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        // Measure FPS and render GUI
        fps_meter_main.update();
		render_GUI(teapot_color, background_color);

		// Clear canvas
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// = After clearing canvas =

		// Update camera position
        process_camera(delta_time);
        audio_manager.set_listener_position(camera.position.x, camera.position.y, camera.position.z, camera.front.x, camera.front.y, camera.front.z);

        // SHADER
        auto current_shader = shader_library.at(key_shader_simple);
        current_shader->activate();
        // Projection matrix is a member of App, it changes on window resize and on FOV change
        current_shader->set_uniform("u_projection_mx", mx_projection);
        
        // Model matrix is handled by each model individually

        // View matrix is handled by the camera
        glm::mat4 mx_view = camera.get_view_matrix();
        current_shader->set_uniform("u_view_mx", mx_view);       

        // DRAW MODELS FROM SCENE
        update_and_draw_models(delta_time);

        // IMGUI DRAW
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// poll events, call callbacks, flip back<->front buffer
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

    // closing graphics window -> app ends
    do_terminate_worker_threads = true;
    do_terminate_encoder_threads = true;
}


void App::update_and_draw_models(float delta_t)
{
    for (auto& [key, value] : scene) {

        // Rotating teapot
        if (key == key_obj_teapot) {
            value.rotation = glm::vec4(0.0f, 1.0f, 0.0f, teapot_rotation_speed * glfwGetTime());
        }
        // Cat movement
        else if (key == key_obj_cat) {
            auto x = value.position.x;
            auto y = value.position.y;
            auto z = value.position.z;
            
            bool is_x_oob = x < cat_min_x || x > cat_max_x;
            bool is_z_oob = z < cat_min_z || z > cat_max_z;

            if (is_x_oob || is_z_oob) { // Is cat out of bounds (oob)?
                if (did_cat_meow_last_frame) {
                    // Cat is oob and also was oob in the previous frame, so we teleport it instead of getting it "stuck on the edge"
                    x = 0.0f;
                    z = 0.0f;
                    audio_manager.play3D(key_snd_teleport, x, y, z);
                    did_cat_meow_last_frame = false;
                }
                else {
                    if (is_x_oob) {
                        cat_direction.x *= -1;
                    }
                    if (is_z_oob) {
                        cat_direction.y *= -1;
                    }
                    audio_manager.play3D(key_snd_meow, x, y, z);
                    did_cat_meow_last_frame = true;
                }
            }
            else {
                did_cat_meow_last_frame = false;
            }

            x += cat_direction.x * cat_speed * delta_t;
            z += cat_direction.y * cat_speed * delta_t;
            y = get_heightmap_y(x, z);
            value.position = glm::vec3(x, y, z);

            float angles = glm::degrees(atan2(-cat_direction.y, cat_direction.x)) + 90;
            value.rotation = glm::vec4(0.0f, 0.0f, 1.0f, angles);
        }

        value.draw();
    }
}


void App::process_camera(float delta_t)
{
    glm::vec3 movement = camera.process_input(window, delta_t, is_camera_freeform);
    camera.position += movement;

    if (!is_camera_freeform) {
        camera.position.y = get_heightmap_y(camera.position.x, camera.position.z);
        camera.position.y += 2.0f; // Add "player height"
    }
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


float App::get_heightmap_y(float position_x, float position_z)
{
    float X = position_x + HEIGHTMAP_SHIFT;
    float Z = position_z + HEIGHTMAP_SHIFT;
    float Y = 0.0f;

    float X_floor = std::floor(X);
    float Z_floor = std::floor(Z);

    float X_ceil = std::ceil(X);
    float Z_ceil = std::ceil(Z);

    if (X - X_floor < 0.5f && Z - Z_floor < 0.5f) {
        // In the lower-left triangle
        float x_fraction = X - X_floor;
        float y_fraction = Z - Z_floor;
        float common_height = heightmap_heights[{X_floor, Z_floor}];
        float x_difference = heightmap_heights[{X_ceil, Z_floor}] - common_height;
        float y_difference = heightmap_heights[{X_floor, Z_ceil}] - common_height;
        Y = common_height + x_fraction * x_difference + y_fraction * y_difference;
    }
    else {
        // In the upper-right triangle
        float x_fraction = X_ceil - X;
        float y_fraction = Z_ceil - Z;
        float common_height = heightmap_heights[{X_ceil, Z_ceil}];
        float x_difference = common_height - heightmap_heights[{X_floor, Z_ceil}];
        float y_difference = common_height - heightmap_heights[{X_ceil, Z_floor}];
        Y = common_height - x_fraction * x_difference - y_fraction * y_difference;
    }

    return Y * HEIGHTMAP_SCALE;
}


void App::webcam_thread()
{
    cv::Mat frame; // For captured frame

    do {
        // Get next frame
        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected? End of video?");
            continue;
        }

        // Find faces
        auto face_centers = face_detector.find_faces(frame);
        int _n_faces_found = static_cast<int>(face_centers.size());

        // Draw face crosses
        for (const auto& face_center : face_centers) {
            CV2Tools::draw_cross_normalized(frame, face_center, 30, CV_RGB(203, 0, 248)); // pink cross
        }
        
        //fmt::println("WEBCAM FRAME PTR = {}", (void*)frame.data);

        // Push into synced_deque
        synced_deque.push_back(std::make_tuple(frame.clone(), _n_faces_found)); // DATA IS BEING COPIED HERE

        
    } while (!do_terminate_worker_threads); // Repeat until App sets `do_terminate_worker_threads` to `true`
}
