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
    
    const std::string key_obj_cube = "obj_cube";
    const std::string key_obj_heightmap = "obj_heightmap";
    const std::string key_obj_teapot = "obj_teapot";
    
    const std::string key_tex_singlecolor = "tex_singlecolor";
    const std::string key_tex_tileatlas = "tex_tileatlas";
    const std::string key_tex_webcam = "tex_webcam";
    const std::string key_tex_woodbox = "tex_woodbox";

    // == MEMBERS ==
    FaceDetector face_detector;
    FPSMeter fps_meter_main;
    FPSMeter fps_meter_encoder;
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

    int win_width;
    int win_height;
    
    bool is_vsync_on{};
    bool is_mouselook_on{};
    bool is_antialiasing_on{};
    bool is_encoder_on{};
    
    double last_mouse_x = 0.0f;
    double last_mouse_y = 0.0f;
    bool is_first_mouse = true;
    float FOV{};
    bool is_camera_freeform = false;

    glm::mat4 mx_projection = glm::identity<glm::mat4>();

    std::unordered_map<std::string, std::shared_ptr<ShaderProgram>> shader_library;
    std::unordered_map<std::string, std::shared_ptr<Texture>> texture_library;
    std::unordered_map<std::string, Model> scene;

    std::map<std::pair<float, float>, float> heightmap_heights;

    // == METHODS ==
    void init_assets();

    // Switches (do_update_bool is set to true when switching via key press and to false when switching via ImGui (because ImGui checkbox already updates the bool))
    void enable_or_disable_mouselook(bool do_update_bool);
    void enable_or_disable_vsync(bool do_update_bool);
    void enable_or_disable_antialiasing(bool do_update_bool);
    
    // AppRun
    void render_GUI(Color& triangle_color, Color& background_color);
    void update_projection_matrix();
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

    // Store OpenGL info
    const char* gl_info_vendor;
    const char* gl_info_renderer;
    const char* gl_info_version;
    const char* gl_info_shading_version;
    const char* gl_info_profile;
    int gl_info_n_texture_units = -1;

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
