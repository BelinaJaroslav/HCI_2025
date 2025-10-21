#include <App/App.hpp>

#include <fmt/core.h>

#include <thread>
#include <condition_variable>


void App::lab_multithread()
{
	std::jthread t_worker(&App::tracker_thread, this);
	std::jthread t_renderer(&App::render_thread, this);

	// jthreads are joined automatically
}


void App::tracker_thread()
{
	/*
	This function contains a loop that must:
		* pick an image from already opened capture device (and verify success)
		* call the function responsible for object identification
		* send the result to the main thread
		* coordinate thread termination (if necessary)
	*/

	FPSMeter fps_meter_tracker_thread;

	cv::Mat frame; // For captured frame

	do {

		// Get next frame
		capture.read(frame);
		if (frame.empty()) {
			fmt::println("Cam disconnected? End of video?");
			continue;
		}

		// Find faces
		auto face_centers = face_detector.find_faces(frame);

		// Push into synced_deque
		synced_deque.push_back(std::make_tuple(frame, face_centers)); // DATA IS BEING COPIED HERE

		// Measure FPS: only frames from tracker thread
		if (fps_meter_tracker_thread.is_updated()) fmt::println("Tracker thread FPS: {:.3f}", fps_meter_tracker_thread.get());
		fps_meter_tracker_thread.update();

		// Repeat until App sets `do_terminate_worker_threads` to `true`
	} while (!do_terminate_worker_threads);
}


void App::render_thread() {
	/*
    Main thread:
	    * opens the camera (video file)
	    * creates the tracker thread
    (in the loop):
	    * prints the result in the console (or displays the image)
	    * tests key to end program
	    * coordinates thread termination
	    * at the end joins tracker thread
   */
	const auto window_name = "Face Detection";

	cv::Mat image_no_face = cv::imread("App/Resources/looking_for_user.jpg");
	cv::Mat image_warning = cv::imread("App/Resources/warning.jpg");

	do {

		// Try to get next frame from worker_thread
		auto tup = synced_deque.pop_front_wait(); // DATA IS BEING COPIED HERE

		auto& frame = std::get<0>(tup);
		auto& face_centers = std::get<1>(tup);

		auto n_faces_found = face_centers.size();

		// Act depending on number of faces found
		if (n_faces_found == 0) {
			// "Splashscreen"
			cv::imshow(window_name, image_no_face);
		}
		else if (n_faces_found == 1) {
			// Find red object
			//NOTE: This takes less than 2ms, but could be moved in a different thread in case of FPS emergency
			auto red_object_center = CV2Tools::find_red_object_chroma(frame);

			// Draw cross where the red object is
			CV2Tools::draw_cross_normalized(frame, red_object_center, 30, CV_RGB(0, 200, 255)); // blue cross

			// Draw face crosses
			/*
			for (const auto& face_center : face_centers) {
				CV2Tools::draw_cross_normalized(frame, face_center, 30, CV_RGB(203, 0, 248)); // pink cross
			}
			/**/

			// Display result
			cv::imshow(window_name, frame);
		}
		else {
			// "Lockscreen"
			cv::imshow(window_name, image_warning);
		}		

		// Measure FPS: use main fps meter to measure render_thread fps
		if (fps_meter_main.is_updated()) fmt::println("Render thread FPS: {:.3f}", fps_meter_main.get());
		fps_meter_main.update();

	} while (cv::pollKey() != 27);

	do_terminate_worker_threads = true;
}

