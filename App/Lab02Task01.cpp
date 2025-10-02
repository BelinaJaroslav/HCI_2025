#include "App.hpp"

#include <fmt/core.h>

void App::lab_complex_behaviour()
{
    if (!face_cascade.load("App/Resources/haarcascade_frontalface_default.xml")) {
        fmt::println("Error loading face cascade.");
    }

    cv::Mat frame; // For captured frame 

    do {
        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected? End of video?");
            break;
        }

        // Find face
        cv::Point2f center = find_face(frame);

        // Display result
        cv::Mat scene_cross = frame.clone();
        draw_cross_normalized(scene_cross, center, 30, CV_RGB(0, 200, 255));
        cv::imshow("Face Detection", scene_cross);

        // Measure FPS
        if (fps_meter.is_updated()) fmt::println("FPS: {:.3f}", fps_meter.get());
        fps_meter.update();

    } while (cv::pollKey() != 27); // Repeat until user presses ESC
}
