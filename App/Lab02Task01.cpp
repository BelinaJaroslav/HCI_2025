#include "App.hpp"

#include <fmt/core.h>

void App::lab_complex_behaviour()
{
    cv::Mat image_no_face = cv::imread("App/Resources/looking_for_user.jpg");
    cv::Mat image_warning = cv::imread("App/Resources/warning.jpg");

    cv::Mat frame; // For captured frame & for displaying

    do {
        // Get next frame
        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected? End of video?");
            break;
        }

        // Find faces
        auto face_centers = face_detector.find_faces(frame);
        auto n_faces_found = face_centers.size();

        // Act depending on number of faces found
        if (n_faces_found == 0) {
            // "Splashscreen"
            cv::imshow("Face Detection", image_no_face);
        }
        else if (n_faces_found == 1) {
            // Find red object and draw cross
            auto red_object_center = find_red_object_chroma(frame);
            draw_cross_normalized(frame, red_object_center, 30, CV_RGB(0, 200, 255)); // blue cross

            // Draw face crosses
            /*
            for (const auto& face_center : face_centers) {
                draw_cross_normalized(frame, face_center, 30, CV_RGB(203, 0, 248)); // pink cross
            }
            /**/

            // Display result
            cv::imshow("Face Detection", frame);
        }
        else {
            // "Lockscreen"
            cv::imshow("Face Detection", image_warning);
        }        

        // Measure FPS
        if (fps_meter.is_updated()) fmt::println("FPS: {:.3f}", fps_meter.get());
        fps_meter.update();

    } while (cv::pollKey() != 27); // Repeat until user presses ESC
}
