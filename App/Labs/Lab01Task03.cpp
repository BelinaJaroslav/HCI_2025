#include <App/App.hpp>

#include <fmt/core.h>

void App::lab_find_red_object_in_video()
{
    fmt::println("== Lab 01 Task 03 ==");

    cv::Mat frame, resized_frame;

	do {

        auto start = std::chrono::steady_clock::now();

        capture.read(frame);
        if (frame.empty())
        {
            fmt::println("Cam disconnected");
            break;
        }

        cv::imshow("grabbed", frame);

        //resize if too slow
        //cv::resize(frame, resized_frame, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR); // Reduce to 50%


        auto center_normalized = CV2Tools::find_red_object_chroma(frame);

        CV2Tools::draw_cross_normalized(frame, center_normalized, 20, CV_RGB(0, 200, 255));
        //CV2Tools::draw_cross_normalized(resized_frame, center_normalized, 20, CV_RGB(0, 200, 255));


        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        fmt::println("Took {} milliseconds.", elapsed_seconds.count() * 1000);

        cv::imshow("Red Object Detection", frame);

	} while (cv::pollKey() != 27); // Message loop until user presses ESC
}
