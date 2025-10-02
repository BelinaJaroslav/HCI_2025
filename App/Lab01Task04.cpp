#include "App.hpp"
#include <fmt/core.h>
#include <chrono>


void App::lab_find_face_in_video()
{
    if (!face_cascade.load("App/Resources/haarcascade_frontalface_default.xml")) {
        fmt::println("Error loading face cascade.");
    }

	cv::Mat frame; // for captured frame 
    do {
        //auto start = std::chrono::steady_clock::now();

        capture.read(frame);
        if (frame.empty()) {
            fmt::println("Cam disconnected ? End of video ?");
            break;
        }

        // Find face
        cv::Point2f center = find_face(frame);

        // Display result
        cv::Mat scene_cross = frame.clone();
        draw_cross_normalized(scene_cross, center, 30, CV_RGB(0, 200, 255));
        cv::imshow("Face Detection", scene_cross);

        /*
        auto end = std::chrono::steady_clock::now();
        double elapsed_ms = std::chrono::duration<double>(end - start).count() * 1000;
        fmt::println("Frame processed in {:.2f} ms", elapsed_ms);
		/**/

        if (fps_meter.is_updated()) fmt::println("FPS: {:.3f}", fps_meter.get());
        fps_meter.update();

	} while (cv::pollKey() != 27); //message loop untill ESC

}

cv::Point2f App::find_face(cv::Mat & frame)
{
    cv::Point2f center(0.0f, 0.0f); // for result

	cv::Mat scene_grey;
    cv::cvtColor(frame, scene_grey, cv::COLOR_BGR2GRAY);

	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(scene_grey, faces);
        
	if (faces.size() > 0)
	{
          // faces[0].x      -- absolute coordinates
          // faces[0].y      -- absolute coordinates
          // faces[0].width
          // faces[0].height
    
          // compute "center" as normalized coordinates of the face  
            center.x = (faces[0].x + faces[0].width / 2.0f) / frame.cols;
            center.y = (faces[0].y + faces[0].height / 2.0f) / frame.rows;
	}

    //fmt::println("Found face center at: ({:.2f}, {:.2f})", center.x, center.y);

    return center;      
}


