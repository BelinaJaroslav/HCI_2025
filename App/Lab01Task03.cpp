#include "App.hpp"

#include <fmt/core.h>

cv::VideoCapture capture;

void init();


void App::FindRedObjectInVideo() const
{
    fmt::println("== Lab 01 Task 03 ==");

    init();

    cv::Mat frame, resized_frame;

	do {

        
        //TODO: Make "loop that grabs the image from a capture device until it is empty, calls find_object_***() and prints result, and test ESC key."
		// Measure time foreach frame, must be <10ms, resize the image so <10ms is possible
		/* 
		## Task 3: Find the red object in sequence of images - camera stream or video

			1. Open capture device(camera or file) stream in init() function, store capture device variable as protected member variable in App class.If camera is not found, open the video file.

			2. Use similar approach as in Task 2 to find a moving object.

			3. Modify App::run(), so that there is only loop that grabs the image from a capture device until it is empty, calls find_object_*** () and prints result, and test ESC key.

			4. Use sample code in time_measure.c to find the fastest approach.Try to spend at most * *10 milliseconds * *per frame.

			5. (optional)resize the image, if speed is too low(less pixels to analyze)

			```C+ +
			cv::Resize(source_frame, frame, cv::INTER_NEAREST); */ 
			//or cv::INTER_LINEAR etc.
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


        auto center_normalized = find_red_object_chroma(frame);

        draw_cross_normalized(frame, center_normalized, 20, CV_RGB(0, 200, 255));
        //draw_cross_normalized(resized_frame, center_normalized, 20, CV_RGB(0, 200, 255));


        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        fmt::println("Took {} milliseconds.", elapsed_seconds.count() * 1000);

        cv::imshow("Red Object Detection", frame);

	} while (cv::pollKey() != 27); // Message loop until user presses ESC
}


static void init()
{
	capture = cv::VideoCapture("App/Resources/video.mkv");

	if (!capture.isOpened())
	{
		std::cerr << "no camera" << std::endl;
		exit(EXIT_FAILURE);
	}
	else
	{
		std::cout << "Camera " <<
			": width=" << capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
			", height=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT) <<
			std::endl;
	}
}

