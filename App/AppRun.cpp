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

	// Colors
	Color teapot_color{ 1.0f, 0.6f, 1.0f }; // Pink ("unlocked")
	Color teapot_color_locked{ 0.8f, 0.0f, 0.0f }; // Red ("locked")
	Color background_color{ 0.549f, 0.823f, 0.858f }; // Sky color day ("unlocked")
	Color background_color_locked{ 0.0f, 0.0f, 0.0f }; // Sky color night ("locked")
	
	const auto SetClearColor = [](Color& c) {
		glClearColor(c.r, c.g, c.b, 1.0f);
	};

	const auto SetTeapotColor = [&](Color& c) {
		if (!texture_library.contains(key_tex_singlecolor)) return;
		texture_library.at(key_tex_singlecolor)->replace_color(
			glm::vec3(c.r * 255, c.g * 255, c.b * 255)
		);
	};

	SetClearColor(background_color);

	// Init view
	update_projection_matrix();
	glViewport(0, 0, win_width, win_height);

	// Start background music
    audio_manager.playBGM(key_snd_bgm, 0.15f);

	// Main game loop
	while (!glfwWindowShouldClose(window)) {
		// Webcam service
		if (!synced_deque.empty()) {
			auto tup = synced_deque.pop_front();
			auto& frame = std::get<0>(tup);
			n_faces_found = std::get<1>(tup);
			texture_library.at(key_tex_webcam)->replace_image(frame);
		}

		if (n_faces_found_debug_override >= 0) {
			n_faces_found = n_faces_found_debug_override;
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
		render_GUI();

		// Clear canvas
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// = After clearing canvas =

		// Update camera position
		process_camera(delta_time);
		audio_manager.set_listener_position(camera.position.x, camera.position.y, camera.position.z, camera.front.x, camera.front.y, camera.front.z);
		audio_manager.clean_finished_sounds();

		// SHADER
		auto current_shader = shader_library.at(key_shader_simple);
		current_shader->activate();
		// Projection matrix is a member of App, it changes on window resize and on FOV change
		current_shader->set_uniform("u_projection_mx", mx_projection);

		// Model matrix is handled by each model individually

		// View matrix is handled by the camera
		glm::mat4 mx_view = camera.get_view_matrix();
		current_shader->set_uniform("u_view_mx", mx_view);

		// Lighting
		current_shader->set_uniform("u_camera_position", camera.position);

		current_shader->set_uniform("u_material_ambient", glm::vec3(0.15f));
		current_shader->set_uniform("u_material_specular", glm::vec3(0.8f));
		current_shader->set_uniform("u_material_shininess", 96.0f);

		current_shader->set_uniform("u_dirlight_direction", glm::vec3(0.0f, -0.9f, -0.17f));
		current_shader->set_uniform("u_dirlight_diffuse", glm::vec3(0.8f));
		current_shader->set_uniform("u_dirlight_specular", glm::vec3(0.14f));

		current_shader->set_uniform("u_reflector.position", camera.position);
		current_shader->set_uniform("u_reflector.direction", camera.front);
		current_shader->set_uniform("u_reflector.cos_inner_cone", glm::cos(glm::radians(20.0f)));
		current_shader->set_uniform("u_reflector.cos_outer_cone", glm::cos(glm::radians(27.0f)));
		current_shader->set_uniform("u_reflector.diffuse", glm::vec3(0.7f));
		current_shader->set_uniform("u_reflector.specular", glm::vec3(0.56f));
		current_shader->set_uniform("u_reflector.is_on", is_flashlight_on);
		current_shader->set_uniform("u_reflector.constant", 1.0f);
		current_shader->set_uniform("u_reflector.linear", 0.07f);
		current_shader->set_uniform("u_reflector.exponent", 0.017f);

		// N_DETECTED_FACES REACTION
		if (is_unlocked) {
			if (n_faces_found == 0 && !is_placing_cow) {
				is_placing_cow = true;
				is_moo_requested = true;
			}
			if (n_faces_found >= 1 && is_placing_cow) {
				is_placing_cow = false;
				is_ufo_spawn_requested = true;
			}
			if (n_faces_found > 1) {
				is_unlocked = false;
				SetClearColor(background_color_locked);
				SetTeapotColor(teapot_color_locked);
				audio_manager.stopBGM();
			}
		}
		else if (n_faces_found <= 1) {
			is_unlocked = true;
			SetClearColor(background_color);
			SetTeapotColor(teapot_color);
			audio_manager.playBGM(key_snd_bgm, 0.15f);
		}

		if (!is_unlocked) {
			current_shader->set_uniform("u_dirlight_diffuse", glm::vec3(0.35f));
			current_shader->set_uniform("u_dirlight_specular", glm::vec3(0.0f));
		}

		// RENDER TRACTOR BEAM (UFO)
		render_tractor_beam(*current_shader);

		// DRAW MODELS FROM SCENE
		update_and_draw_models(delta_time);

		// IMGUI DRAW
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		// poll events, call callbacks, flip back<->front buffer

        if (did_user_press_screenshot_key) {
            save_screenshot();
			did_user_press_screenshot_key = false;
		}

		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	// closing graphics window -> app ends
	do_terminate_worker_threads = true;
	do_terminate_encoder_threads = true;
}


void App::update_and_draw_models(float delta_t)
{
	constexpr float COW_FLY_SPEED = 2.8f;
	constexpr float COW_DIST_FROM_CAMERA = 4.0f;

	// == COW + UFO update ==

	if (scene.contains(key_obj_cow) && scene.contains(key_obj_ufo)) {
		auto& cow = scene.at(key_obj_cow);
		auto& ufo = scene.at(key_obj_ufo);

		if (is_ufo_visible) {
			// UFO IS VISIBLE => COW IS BEING KIDNAPPED
			if (float stop_height = ufo.position.y - 1.0f; cow.position.y < stop_height) {
				// COW GOING UP
				cow.position.y += COW_FLY_SPEED * delta_t;
			}
			else {
				// COW REACHED UFO
				audio_manager.play3D(key_snd_teleport, ufo.position.x, ufo.position.y, ufo.position.z);
				is_cow_visible = false;
				is_ufo_visible = false;

				// Ignore changing number of detected faces when cow flys up by resetting appropriate vars after cow reaches ufo:
				is_placing_cow = false;
				is_ufo_spawn_requested = false;
			}
		}
		else {
			if (is_placing_cow) {
				// PLACING COW
				float distance_in_front = COW_DIST_FROM_CAMERA;
				glm::vec3 spawn_pos = camera.position + (camera.front * distance_in_front);
				spawn_pos.y = get_heightmap_y(spawn_pos.x, spawn_pos.z);
				cow.position = spawn_pos;
				float cow_angle = glm::degrees(atan2(-camera.front.z, camera.front.x));
				cow.rotation = glm::vec4(0.0f, 1.0f, 0.0f, cow_angle);
				is_cow_visible = true;
				if (is_moo_requested) {
					is_moo_requested = false;
					audio_manager.play3D(key_snd_cowmoo, cow.position.x, cow.position.y, cow.position.z);
				}
			}
			if (is_ufo_spawn_requested) {
				is_ufo_spawn_requested = false;
				if (!is_placing_cow && is_cow_visible) {
					// SPAWN UFO
					ufo.position = cow.position + glm::vec3(0.0f, 10.0f, 0.0f);
					is_ufo_visible = true;
					audio_manager.play3D(key_snd_cowrip, cow.position.x, cow.position.y, cow.position.z);
				}				
			}
		}
	}
	else {
		fmt::println(stderr, "!scene.contains(key_obj_cow) || !scene.contains(key_obj_ufo)");
	}	

	// == Update the rest ==

	for (auto& [key, value] : scene) {

		// Rotating teapot
		if (key == key_obj_teapot) {
			const float teapot_rotation_speed = (is_unlocked) ? 23.0f : -120.0f;
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

		// Do not draw UFO/Cow if they shouldn't be in the scene
		if (key == key_obj_ufo && !is_ufo_visible) continue;
		if (key == key_obj_cow && !is_cow_visible) continue;

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
	// The face detector is not the best, so,
	// we consider that number of faces has changed only if said number is detected for more than `N_FACES_CHANGE_FRAME_THRESH` frames.
	constexpr int N_FACES_CHANGE_FRAME_THRESH = 20;
	// Used to store the most recent output from face detector:
	int _current = 1;
	// `_prev` stores the value of `_current` from the previous frame:
	int _prev;
	// Used to count, how many consecutive frames had the same `_current` value:
	int _counter = 0;
	// This is what give to the app, changed only after `N_FACES_CHANGE_FRAME_THRESH` consecutive frames:
	int _n_faces_found = _current;
	// Used to store the captured frame:
	cv::Mat frame; 

	do {
		// Get next frame
		capture.read(frame);
		if (frame.empty()) {
			fmt::println("Cam disconnected? End of video?");
			continue;
		}

		// Find faces
		auto face_centers = face_detector.find_faces(frame);

		// Draw face crosses
		for (const auto& face_center : face_centers) {
			CV2Tools::draw_cross_normalized(frame, face_center, 30, CV_RGB(203, 0, 248)); // pink cross
		}

		_prev = _current;
		_current = static_cast<int>(face_centers.size());

		if (_prev == _current) {
			_counter++;
		}
		else {
			_counter = 0;
		}

		if (_counter > N_FACES_CHANGE_FRAME_THRESH) {
			_n_faces_found = _current;
		}

		cv::Mat safe_copy = frame.clone();
		// Push into synced_deque
		synced_deque.push_back(std::make_tuple(std::move(safe_copy), _n_faces_found));

	} while (!do_terminate_worker_threads); // Repeat until App sets `do_terminate_worker_threads` to `true`
}


void App::render_tractor_beam(ShaderProgram& shader)
{
	// 1. If the beam is off, tell the shader explicitly and exit
	if (!is_ufo_visible) {
		shader.set_uniform("u_tractor.is_on", 0);
		return;
	}

	// 2. Safety check for object existence
	if (!scene.contains(key_obj_ufo) || !scene.contains(key_obj_cow)) {
		shader.set_uniform("u_tractor.is_on", 0);
		return;
	}

	const auto& ufo = scene.at(key_obj_ufo);
	const auto& cow = scene.at(key_obj_cow);

	// 3. Set UFO as origin and point toward the cow
	shader.set_uniform("u_tractor.position", ufo.position);
	shader.set_uniform("u_tractor.direction", glm::normalize(cow.position - ufo.position));

	// 4. Cone Aperture (How wide the beam is)
	shader.set_uniform("u_tractor.cos_inner_cone", glm::cos(glm::radians(12.0f)));
	shader.set_uniform("u_tractor.cos_outer_cone", glm::cos(glm::radians(18.0f)));

	// 5. Green Alien Color
	shader.set_uniform("u_tractor.diffuse", glm::vec3(0.0f, 1.0f, 0.2f));
	shader.set_uniform("u_tractor.specular", glm::vec3(0.5f, 1.0f, 0.5f));

	// 6. Falloff (How far the light travels)
	shader.set_uniform("u_tractor.constant", 1.0f);
	shader.set_uniform("u_tractor.linear", 0.09f);
	shader.set_uniform("u_tractor.exponent", 0.032f);

	// 7. Enable this specific light slot
	shader.set_uniform("u_tractor.is_on", 1);
}
