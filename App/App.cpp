// C++ standard
#include <fstream>
#include <filesystem>

// OpenCV - GL independent
#include <opencv2/opencv.hpp>

// OpenGL Extension Wrangler: allow all multiplatform GL functions
#include <GL/glew.h> 
// WGLEW = Windows GL Extension Wrangler :: platform specific functions (in this case Windows)
//#include <GL/wglew.h> // Not needed for our App

// GLFW toolkit
// Uses GL calls to open GL context, i.e. GLEW must be first.
#include <GLFW/glfw3.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// OpenGL error callback
#include "gl_err_callback.h"

// Non-OpenGL 3rd party libraries
#include <fmt/core.h>
#include <nlohmann/json.hpp>

// Our App
#include "App.hpp"


App::App()
    : do_terminate_worker_threads(false)
{
    fmt::println("Startujem.");
}


bool App::init() 
{
    // Disable OpenCV info messages
    cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_WARNING);

    // Open capture device (camera or file) stream here, store capture device variable as protected member variable in App class. If camera is not found, open the video file.
    capture.open(0);

    if (!capture.isOpened()) {
        fmt::println("No camera found, trying fallback video file...");
        capture.open("App/Resources/video.mkv");
    }

    if (!capture.isOpened()) {
        fmt::println("Error: Could not open camera or video file.");
        return false;
    }

    webcamp_width = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_WIDTH));
    webcamp_height = static_cast<int>(capture.get(cv::CAP_PROP_FRAME_HEIGHT));
    fmt::println("Initialized capture device. Width: {}, Height: {}", webcamp_width, webcamp_height);

    // Read one frame; we need it because when we change texture (webcamp footage in imgui) size and data format MUST match; so this will be set as initial texture
    capture.read(initial_frame);

    // Init FaceDetector
    if (!face_detector.load_classifier("App/Resources/haarcascade_frontalface_default.xml")) {
        fmt::println("Error loading face cascade classifier.");
    }
    fmt::println("Initialized face detector.");
    
    // Load JSON conf
    std::ifstream settings_file("App/Resources/app_settings.json");
    nlohmann::json settings = nlohmann::json::parse(settings_file);
    
    // - Default values
    std::string app_name = "App";
    win_width = 640;
    win_height = 480;
    is_vsync_on = true;
    is_mouselook_on = false;
    FOV = 110.0f;

    if (settings["app_name"].is_string()) {
        app_name = settings["app_name"].template get<std::string>();
    }
    if (settings["default_resolution"].is_object()) {
        if (settings["default_resolution"]["x"].is_number_integer()) {
            win_width = settings["default_resolution"]["x"].template get<int>();
        }
        if (settings["default_resolution"]["y"].is_number_integer()) {
            win_height = settings["default_resolution"]["y"].template get<int>();
        }
    }
    if (settings["vsync"].is_boolean()) {
        is_vsync_on = settings["vsync"].template get<bool>();
    }
    if (settings["mouselook"].is_boolean()) {
        is_mouselook_on = settings["mouselook"].template get<bool>();
    }
    if (settings["FOV"].is_number_integer()) {
        FOV = static_cast<float>(settings["FOV"].template get<int>());
    }

    // Init OpenGL
    try {
        // Set GLFW error callback
        glfwSetErrorCallback(error_callback);

        // Init GLFW :: https://www.glfw.org/documentation.html
        if (!glfwInit()) {
            return false;
        }

        // Set OpenGL version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        // Set OpenGL profile
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // Core, comment-out this line for Compatible

        // Comment these out to disable anti-aliasing
        glEnable(GL_MULTISAMPLE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        // ------------------------------------------

        // Window is hidden until everything is initialized
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);        

        // Open window (GL canvas) with no special properties :: https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(win_width, win_height, app_name.c_str(), NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(window, this);

        // Hide cursor according to JSON config
        if (is_mouselook_on) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        // These can be used later for switching Fullscreen On & Off
        monitor = glfwGetPrimaryMonitor(); // Get primary monitor
        mode = glfwGetVideoMode(monitor); // Get resolution of the monitor

        // Setup callbacks
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // Set V-Sync according to JSON config
        glfwSwapInterval(is_vsync_on);

        // Init GLEW :: http://glew.sourceforge.net/basic.html
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        //wglewInit();

        //...after ALL GLFW & GLEW init ...
        if (GLEW_ARB_debug_output) {
            glDebugMessageCallback(MessageCallback, 0);
            glEnable(GL_DEBUG_OUTPUT);

            // default is asynchronous debug output, use this to simulate glGetError() functionality
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            fmt::println("GL_DEBUG enabled.");
        }
        else fmt::println("GL_DEBUG NOT SUPPORTED!");

        // Set GL params
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        // Transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // First init OpenGL, THAN init assets: valid context MUST exist
        init_assets();

        // Show window after everything loads        
        glfwShowWindow(window);


        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // IF using Docking Branch

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
        ImGui_ImplOpenGL3_Init();
    }
    catch (std::exception const& e) {
        fmt::println(stderr, "Init failed: {}", e.what());
        exit(-1);
    }

    camera = Camera(glm::vec3(4.0f, 2.0f, 6.0f));

    fmt::println("App initialized.\n================");

    print_gl_info();

    return true;
}


App::~App()
{
    // Cleanup OpenCV
    cv::destroyAllWindows();
    if (capture.isOpened()) capture.release();

    // Bye
    fmt::println("\nNashle.");
}


void App::print_gl_info()
{
    std::cout << "\n======================= :: GL Info :: =======================\n";
    std::cout << "GL Vendor:\t\t" << glGetString(GL_VENDOR) << "\n";
    std::cout << "GL Renderer:\t\t" << glGetString(GL_RENDERER) << "\n";
    std::cout << "GL Version:\t\t" << glGetString(GL_VERSION) << "\n";
    std::cout << "GL Shading version:\t" << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
    
    int n_texture_units = -1;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &n_texture_units);
    std::cout << "No. of texture units:\t" << n_texture_units << "\n";

    std::cout << "\n";
    GLint profile;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
    if (const auto errorCode = glGetError()) {
        std::cout << "[!] Pending GL error while obtaining profile: " << errorCode << "\n";
    }
    if (profile & GL_CONTEXT_CORE_PROFILE_BIT) {
        std::cout << "Core profile" << "\n";
    }
    else {
        std::cout << "Compatibility profile" << "\n";
    }
    std::cout << "=============================================================\n\n";
}


void App::enable_or_disable_mouselook()
{
    ImGuiIO& io = ImGui::GetIO();

    is_mouselook_on = !is_mouselook_on;
    if (is_mouselook_on) {
        // Mouselook was enabled => cursor needs to be disabled
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }
    else {
        // Mouselook was disabled => cursor needs to be enabled
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }
}


void App::enable_or_disable_vsync()
{
    is_vsync_on = !is_vsync_on;
    glfwSwapInterval(is_vsync_on);
    fmt::println("VSync: {}", is_vsync_on);
}
