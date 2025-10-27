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

// Non-OpenGL 3rd party libraries
#include <fmt/core.h>

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

    fmt::println("Initialized capture device. Width: {}, Height: {}",
        capture.get(cv::CAP_PROP_FRAME_WIDTH),
        capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    // Init FaceDetector
    if (!face_detector.load_classifier("App/Resources/haarcascade_frontalface_default.xml")) {
        fmt::println("Error loading face cascade classifier.");
    }
    fmt::println("Initialized face detector.");
    
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

        // Window is hidden until everything is initialized
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        // Open window (GL canvas) with no special properties :: https://www.glfw.org/docs/latest/quick.html#quick_create_window
        window = glfwCreateWindow(1280, 800, "HCI 2025", NULL, NULL);
        if (!window) {
            glfwTerminate();
            return false;
        }
        glfwSetWindowUserPointer(window, this);

        // Hide cursor
        //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // These can be used later for switching Fullscreen On & Off
        monitor = glfwGetPrimaryMonitor(); // Get primary monitor
        mode = glfwGetVideoMode(monitor); // Get resolution of the monitor

        // Setup callbacks
        glfwMakeContextCurrent(window);
        glfwSetKeyCallback(window, key_callback);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);

        // Set V-Sync ON.
        glfwSwapInterval(1);
        is_vsync_on = true;

        // Init GLEW :: http://glew.sourceforge.net/basic.html
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        }
        //wglewInit();

        //...after ALL GLFW & GLEW init ...
        if (GLEW_ARB_debug_output) {
            //glDebugMessageCallback(MessageCallback, 0);
            glEnable(GL_DEBUG_OUTPUT);

            // default is asynchronous debug output, use this to simulate glGetError() functionality
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            std::cout << "GL_DEBUG enabled.\n";
        }
        else std::cout << "GL_DEBUG NOT SUPPORTED!\n";

        // Set GL params
        glEnable(GL_DEPTH_TEST);

        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_POLYGON_SMOOTH);

        //glEnable(GL_CULL_FACE);

        // Transparency blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // First init OpenGL, THAN init assets: valid context MUST exist
        init_assets();

        // Show window after everything loads        
        glfwShowWindow(window);
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << "\n";
        exit(-1);
    }

    fmt::println("App initialized.\n================");
    return true;
}


void App::init_assets()
{
    // Lab 05
    lab05_init_assets();
}


void App::run()
{
    // Lab 01 Task 01
    //lab_identify_object_by_luminance();
    // Lab 01 Task 02
    //lab_find_red_object_in_image();
    // Lab 01 Task 03
    //lab_find_red_object_in_video();
    // Lab 01 Task 04
    //lab_find_face_in_video();

    // Lab 02
    //lab_complex_behaviour();
    
    // Lab 03
    //lab_multithread();

    // Lab 04
    //lab_compression();
    //lab_compression_pool();

    // Lab 05
    lab05_run();
}


App::~App()
{
    // Cleanup OpenCV
    cv::destroyAllWindows();
    if (capture.isOpened()) capture.release();

    // Cleanup OpenGL
    glDeleteProgram(shader_prog_ID);
    glDeleteBuffers(1, &VBO_ID);
    glDeleteVertexArrays(1, &VAO_ID);

    // Bye
    fmt::println("Nashle.");
}
