#pragma once

// C++ standard libraries
#include <atomic>
#include <tuple>

// 3rd party libraries
#include <opencv2/opencv.hpp>

// Currently (lab05), we need to import GLFW3 to use `GLFWwindow` etc.
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
#include "CV2Tools.hpp"
#include "FaceDetector.hpp"
#include "FPSMeter.hpp"
#include "SyncedDeque.hpp"
#include "ThreadPool.hpp"
#include "Assets.hpp"


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

    // == MEMBERS ==
    FaceDetector face_detector;
    FPSMeter fps_meter_main;
    SyncedDeque<std::tuple<cv::Mat, std::vector<cv::Point2f>>> synced_deque;

    cv::VideoCapture capture;

    std::atomic<bool> do_terminate_worker_threads;

    // OpenGL members
    GLFWwindow* window{};
    GLFWmonitor* monitor{};
    const GLFWvidmode* mode{};

    int win_width;
    int win_height;
    
    bool is_vsync_on{};
    bool is_mouselook_on{};

    GLuint shader_prog_ID{ 0 };
    GLuint VBO_ID{ 0 };
    GLuint VAO_ID{ 0 };
    std::vector<vertex> triangle_vertices =
    {
        {{0.0f,  0.5f,  0.0f}},
        {{0.5f, -0.5f,  0.0f}},
        {{-0.5f, -0.5f,  0.0f}}
    };

    // == METHODS ==
    void init_assets();
    void print_gl_info();
    
    // Callbacks
    static void error_callback(int error, const char* description);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    
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
    int lab_compression_pool();

    void grabber_thread();
    void process_frame(const cv::Mat& original, int id, int threshold, int quality, SyncedDeque<ProcessedFrame>& result_queue);
    std::vector<uchar> lossy_bw_limit(cv::Mat& input_img, size_t size_limit);
    std::vector<uchar> lossy_quality_limit(const cv::Mat& frame, const float target_coefficient);

    // AppRun
    void renderGUI(FPSMeter& fps_meter, Color& triangle_color, float background_color[3]);

};
