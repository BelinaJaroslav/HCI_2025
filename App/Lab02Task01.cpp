#include "App.hpp"

#include <fmt/core.h>

void App::lab_complex_behaviour()
{
    cv::Mat frame; // For captured frame & for displaying

    do {
        // Get next frame
        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected? End of video?");
            break;
        }

        // Find face
        cv::Point2f center(0);

        // Draw crosses
        draw_cross_normalized(frame, center, 30, CV_RGB(0, 200, 255));
        
        // Display result
        cv::imshow("Face Detection", frame);

        // Measure FPS
        if (fps_meter.is_updated()) fmt::println("FPS: {:.3f}", fps_meter.get());
        fps_meter.update();

    } while (cv::pollKey() != 27); // Repeat until user presses ESC
}
