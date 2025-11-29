#include "App/Macros.hpp"
#ifndef SKIP_LABS_COMPILATION

#include <App/App.hpp>

#include <fmt/core.h>
#include <chrono>


void App::lab_find_face_in_video()
{
	cv::Mat frame; // for captured frame 
    do {
        //auto start = std::chrono::steady_clock::now();

        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected ? End of video ?");
            break;
        }

        // Find face
        auto faces = face_detector.find_faces(frame);
        cv::Point2f center(0);
        if (faces.size() > 0) {
            center = faces[0];
        }

        // Display result
        cv::Mat scene_cross = frame.clone();
        CV2Tools::draw_cross_normalized(scene_cross, center, 30, CV_RGB(0, 200, 255));
        cv::imshow("Face Detection", scene_cross);

        /*
        auto end = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double>(end - start).count() * 1000;
        fmt::println("Frame processed in {:.2f} ms", elapsed_ms);
		/**/

        if (fps_meter_main.is_updated()) fmt::println("FPS: {:.3f}", fps_meter_main.get());
        fps_meter_main.update();

	} while (cv::pollKey() != 27); //message loop untill ESC
}

#endif // !SKIP_LABS_COMPILATION
