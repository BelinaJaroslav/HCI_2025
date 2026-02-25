#pragma once

// C++ standard libraries
#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>

// 3rd party libraries
#include <opencv2/opencv.hpp>

// We need to import GLFW3 to use `GLFWwindow` etc.
// But importing just GLFW3 gives us bunch of errors (wrong import order),
// so we import other stuff aswell and import order is correct.
#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Imgui import 
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

// Our libraries
#include "AudioManager.hpp"
#include "Camera.hpp"
#include "CV2Tools.hpp"
#include "DefinesAndMacros.hpp"
#include "FaceDetector.hpp"
#include "FPSMeter.hpp"
#include "Model.hpp"
#include "ThreadPool.hpp"
#include "ShaderProgram.hpp"
#include "SyncedDeque.hpp"
#include "vertex.hpp"


class App {
public:

    App();
    bool init();
    void run();
    ~App();

    struct ProcessedFrame {
        int id;
        cv::Mat processed_image;  // e.g., decoded after compression
        cv::Mat original_image;   // the original captured frame
    };

    SyncedDeque<ProcessedFrame> result_queue;

private:

    struct Color { GLfloat r, g, b, a; };

    // Map keys that may be used across multiple methods
    const std::string key_shader_simple = "simple_shader";
    
    const std::string key_obj_cat = "obj_cat";
	const std::string key_obj_cow = "obj_cow";
    const std::string key_obj_cube = "obj_cube";
    const std::string key_obj_heightmap = "obj_heightmap";
    const std::string key_obj_teapot = "obj_teapot";
	const std::string key_obj_ufo = "obj_ufo";
    
    const std::string key_tex_cat = "tex_cat";
    const std::string key_tex_cow = "tex_cow";
    const std::string key_tex_singlecolor = "tex_singlecolor";
    const std::string key_tex_tileatlas = "tex_tileatlas";
	const std::string key_tex_ufo = "tex_ufo";
    const std::string key_tex_webcam = "tex_webcam";
    const std::string key_tex_woodbox = "tex_woodbox";

	const std::string key_snd_bgm = "snd_bgm";
	const std::string key_snd_cowmoo = "snd_cowmoo";
	const std::string key_snd_cowrip = "snd_cowrip";
    const std::string key_snd_meow = "snd_meow";
    const std::string key_snd_pop = "snd_pop";
    const std::string key_snd_teleport = "snd_teleport";
	const std::string key_snd_ufo = "snd_ufo";

    // == MEMBERS ==
    FaceDetector face_detector;
    FPSMeter fps_meter_main;
    SyncedDeque<std::tuple<cv::Mat, int>> synced_deque;
    cv::VideoCapture capture;
    Camera camera;
    AudioManager audio_manager;

    std::atomic<bool> do_terminate_worker_threads;
    std::atomic<bool> do_terminate_encoder_threads;

    int n_faces_found = 0;
    int webcamp_width = 0;
    int webcamp_height = 0;
    cv::Mat initial_frame;

    // OpenGL members
    GLFWwindow* window{};
    GLFWmonitor* monitor{};
    const GLFWvidmode* mode{};

    int win_width{};
    int win_height{};
    int win_xcor{};
    int win_ycor{};
    int win_width_memory{};
    int win_height_memory{};
    
    bool is_vsync_on{};
    bool is_fullscreen_on = false;
    bool is_flashlight_on = true;
	bool is_tractor_beam_on = false;
    bool is_mouselook_on{};
    bool is_antialiasing_on{};
    bool is_encoder_on{};
    bool do_draw_ufo = false;
	bool do_draw_cow = false;
    bool do_place_ufo = false;
	bool is_placing_cow = false;
    bool do_play_ufo_sound = false;
    
    double last_mouse_x = 0.0f;
    double last_mouse_y = 0.0f;
    bool is_first_mouse = true;
    float FOV{};
    bool is_camera_freeform = false;

	bool userPressedScreenshotKey = false;

    glm::mat4 mx_projection = glm::identity<glm::mat4>();

    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shader_library;
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_library;
    std::unordered_map<std::string, Model> scene;

    std::map<std::pair<float, float>, float> heightmap_heights;

    // Object dynamics
    // Teapot
    const float teapot_rotation_speed = 23.0f;
    // Cat
    const float cat_speed = 6.5f;
    const float cat_min_x = -8;
    const float cat_max_x = 10;
    const float cat_min_z = -3;
    const float cat_max_z = 13;
    glm::vec2 cat_direction{};
    bool did_cat_meow_last_frame = false;

    // == METHODS ==
    void init_assets();

    // Switches (do_update_bool is set to true when switching via key press and to false when switching via ImGui (because ImGui checkbox already updates the bool))
    void enable_or_disable_mouselook(bool do_update_bool);
    void enable_or_disable_fullscreen(bool do_update_bool);
    void enable_or_disable_vsync(bool do_update_bool);
    void enable_or_disable_antialiasing(bool do_update_bool);
    
    // AppRun
    void render_GUI(Color& triangle_color, Color& background_color);
    void update_projection_matrix();
    void update_and_draw_models(float delta_t);
    void process_camera(float delta_t);
    float get_heightmap_y(float position_x, float position_z);

    // Callbacks
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
    // Webcam service
    void webcam_thread();

    void save_screenshot();

    // Store OpenGL info
    const char* gl_info_vendor{};
    const char* gl_info_renderer{};
    const char* gl_info_version{};
    const char* gl_info_shading_version{};
    const char* gl_info_profile{};
    int gl_info_n_texture_units = -1;

    void render_tractor_beam(ShaderProgram& shader);

    // Faster compile time: These old Labs don't have to be re-compiled everytime we change anything in App.hpp or any of its imports
#ifndef SKIP_LABS_COMPILATION
    // Lab 01
    void lab_identify_object_by_luminance() const;
    void lab_find_red_object_in_image() const;
    void lab_find_red_object_in_video();
    void lab_find_face_in_video();    
    // Lab 02
    void lab_complex_behaviour();    
    // Lab 03
    void lab_multithread();    
    void tracker_thread();
    void render_thread();
    // Lab 04
    int lab_compression(); 
    std::vector<uchar> lossy_bw_limit(cv::Mat& input_img, size_t size_limit);
    std::vector<uchar> lossy_quality_limit(const cv::Mat& frame, const float target_coefficient);
#endif // !SKIP_LABS_COMPILATION

    // "constant quality video encoder" is part of the final assignment
    void grabber_thread();
    void process_frame(const cv::Mat& original, int id, int threshold, int quality, SyncedDeque<ProcessedFrame>& result_queue);
    int lab_compression_pool();
};
